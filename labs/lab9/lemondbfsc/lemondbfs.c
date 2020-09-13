/*
*
* Author: Mr. Frown
* Creation date: 2018.12.23
*
*/

#define FUSE_USE_VERSION 26

#define T_NAME_LENGTH 50
#define M_bit_LENGTH 50
#define T_LENGTH 50

#define LM_DATA ((struct lemondb_state *) fuse_get_context()->private_data)

#include "run_query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <fuse/fuse.h>
#include <linux/limits.h>


struct lemondb_state{
    int num_tbl;
    char *table_name[T_LENGTH];
    int M_bit[M_bit_LENGTH];
    char *db_dir;
};

void lemonDB_usage()
{
    fprintf(stderr, "usage:  lemonDBfs [FUSE and mount options] rootDir mountPoint\n");
    abort();
}

void db_fullpath(char *dpath, char *tmp_path)
// dpath := *db_dir*/mtable0
{
    strcpy(dpath, LM_DATA->db_dir);
    strcat(dpath, tmp_path);
}

static void *lemonDBfs_init(struct fuse_conn_info *fi)
{
    (void) fi;
    return LM_DATA;
}

// FUSE function implementations.
static int lemonDBfs_getattr(const char *path, struct stat *stbuf)
{
    printf("getattr(\"%s\"\n", path);

    char fpath[PATH_MAX];	// full path
    memset(fpath, 0, sizeof(char)*PATH_MAX);
    memset(stbuf, 0, sizeof(struct stat));

    // If euqual to "/" then is a directory
    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    char tmp_path[PATH_MAX];
    memset(tmp_path, 0, sizeof(char)*PATH_MAX);

    for (int i = 0; i < LM_DATA->num_tbl; i++)
    {
        char table_path[PATH_MAX];
        memset(table_path, 0, sizeof(char)*PATH_MAX);
        strcpy(table_path, "/");
        strcat(table_path, LM_DATA->table_name[i]); // table_path := /mtable0 (directory)

        // A table
        strcpy(tmp_path, table_path);	// tmp_path := /mtable0 (directory)

        // directory (mtable0)
        if (strcmp(path, tmp_path) == 0)
        {
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            return 0;
        }

        // table file (orig.tbl = mtable0.tbl)
        db_fullpath(fpath, table_path); // fpath := *db_dir*/mtable0
        strcat(fpath, ".tbl"); // fpath := *db_dir*/mtable0.tbl
        strcat(tmp_path, "/orig.tbl"); // tmp_path := /mtable0/orig.tbl (file)
        if (strcmp(path, tmp_path) == 0)
            return lstat(fpath, stbuf);

        // A query
        memset(tmp_path, 0, sizeof(char)*PATH_MAX);
        strcpy(tmp_path, table_path);
        strcat(tmp_path, "/.query"); // tmp_path := /mtable0/.query (file)

        // query file (.query)
        if (strcmp(path, tmp_path) == 0)
        {
            stbuf->st_mode = S_IFREG | 0644;
            stbuf->st_nlink = 1;
            stbuf->st_size = 0;
            return 0;
            /*stbuf->st_mode = S_IFREG | 0777;
            stbuf->st_nlink = 1;
            stbuf->st_size = 4096;
            return 0;*/
        }

        // dump file (_dump.tbl = res.tbl)
        if (LM_DATA->M_bit[i])
        {
            //db_fullpath(fpath, table_path); // fpath := *db_dir*/mtable0
            //strcat(fpath, "_dump.tbl"); // fpath := *db_dir*/mtable0_dump.tbl

            memset(tmp_path, 0, sizeof(char)*PATH_MAX);
            strcpy(tmp_path, table_path);
            strcat(tmp_path, "/res.tbl"); // tmp_path := /mtable0/res.tbl (file)
            if (strcmp(path, tmp_path) == 0)
                //return lstat(fpath, stbuf);
            {
                return lstat(fpath, stbuf);
                /*stbuf->st_mode = S_IFREG | 0777;
                stbuf->st_nlink = 1;
                stbuf->st_size = 4096;
                return 0;*/
            }
        }
    }

    fprintf(stderr, "no match path for getattr(%s)", path);
    return -1;
}

static int lemonDBfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    printf("readdir(\"%s\"\n", path);
    (void) offset;
    (void) fi;


    // root
    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        for (int i = 0;i < LM_DATA->num_tbl;++i)
            filler(buf, LM_DATA->table_name[i], NULL, 0);
        return 0;
    }

    // dir for each table
    char tmp_path[PATH_MAX] = {0};
    for (int i = 0;i < LM_DATA->num_tbl;++i) {
        strcpy(tmp_path, "/");
        strcat(tmp_path, LM_DATA->table_name[i]); // tmp_path := /mtable0 (directory)
        if (strcmp(path, tmp_path) == 0) {
            filler(buf, ".", NULL, 0);
            filler(buf, "..", NULL, 0);
            filler(buf, "orig.tbl", NULL, 0);
            filler(buf, ".query", NULL, 0);
            if (LM_DATA->M_bit[i])
                filler(buf, "res.tbl", NULL, 0);
            return 0;
        }
    }

    fprintf(stderr, "no match path for readdir(%s)", path);
    return -1;
}


static int lemonDBfs_open(const char *path, struct fuse_file_info *fi)
{
    printf("open(\"%s\"\n", path);
    int fd;
    char fpath[PATH_MAX];

    char tmp_path[PATH_MAX];
    memset(tmp_path, 0, sizeof(char)*PATH_MAX);

    // Go through all the files
    for (int i = 0; i < LM_DATA->num_tbl; i++)
    {
        char table_path[PATH_MAX];
        memset(table_path, 0, sizeof(char)*PATH_MAX);
        strcpy(table_path, "/");
        strcat(table_path, LM_DATA->table_name[i]); // table_path := /mtable0 (directory)

        strcpy(tmp_path, table_path);
        strcat(tmp_path, "/orig.tbl"); // tmp_path := /mtable0/orig.tbl (file)

        // If matches, then open it
        if (strcmp(path, tmp_path) == 0)
        {
            db_fullpath(fpath, table_path); // fpath := *db_dir*/mtable0
            strcat(fpath, ".tbl"); // fpath := *db_dir*/mtable0.tbl
            fd = open(fpath, fi->flags);
            if (fd < 0) return -1;
            fi->fh = fd;
            return 0;
        }

        if (LM_DATA->M_bit[i])
        {
            strcpy(tmp_path, table_path);
            strcat(tmp_path, "/res.tbl"); // tmp_path := /mtable0/res.tbl (file)

            // open dump file for write
            if (strcmp(path, tmp_path) == 0)
            {
                db_fullpath(fpath, table_path); // fpath := *db_dir*/mtable0
                strcat(fpath, "_dump.tbl"); // fpath := *db_dir*/mtable0.tbl
                fd = open(fpath, fi->flags);
                if (fd < 0) return -1;
                fi->fh = fd;
                return 0;
            }
        }

        strcpy(tmp_path, table_path);
        strcat(tmp_path, "/.query"); // tmp_path := /mtable0/.query (file)
        if (strcmp(path, tmp_path) == 0)
        {
            return 0;
        }
    }

    fprintf(stderr, "no match path for open(%s)", path);
    return -1;
}

static int lemonDBfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;

    printf("read(\"%s\"\n", path);

    char tmp_path[PATH_MAX];
    memset(tmp_path, 0, sizeof(char)*PATH_MAX);

    for (int i = 0;i < LM_DATA->num_tbl; i++)
    {
        char table_path[PATH_MAX];
        memset(table_path, 0, sizeof(char)*PATH_MAX);
        strcpy(table_path, "/");
        strcat(table_path, LM_DATA->table_name[i]); // table_path := /mtable0 (directory)

        strcpy(tmp_path, table_path);
        strcat(tmp_path, "/orig.tbl"); // tmp_path := /mtable0/orig.tbl (file)
        if (strcmp(path, tmp_path) == 0)
            return pread(fi->fh, buf, size, offset);

        strcpy(tmp_path, table_path);
        strcat(tmp_path, "/.query"); // tmp_path := /mtable0/orig.tbl (file)
        if (strcmp(path, tmp_path) == 0)
            return pread(fi->fh, buf, size, offset);

        if (LM_DATA->M_bit[i])
        {
            strcpy(tmp_path, table_path);
            strcat(tmp_path, "/res.tbl"); // tmp_path := /mtable0/res.tbl (file)
            if (strcmp(path, tmp_path) == 0)
                return pread(fi->fh, buf, size, offset);
        }
    }
    fprintf(stderr, "no contents to be read!\n");
    return -1;
}

int lemonDBfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
/*
 * when a query is written to a .query file, apply it the orig.tbl
     and create the file res.tbl containing the result of the query
		 eg. upon a write on mTable10/.query run the query (error on wrong
		 query) and create mTable10/res.tbl with the result of the query
 * */
{
    (void) fi;

    int rt = -1;

    for(int i = 0; i < LM_DATA->num_tbl; i++){
        char query_path[PATH_MAX] = {0};
        char load_path[PATH_MAX] = {0};
        char dump_path[PATH_MAX] = {0};
        char temp_path[PATH_MAX] = {0};

        strcpy(query_path, "/");
        strcat(query_path, LM_DATA->table_name[i]);

        strcpy(load_path, query_path);
        strcpy(dump_path, query_path);

        strcpy(temp_path, query_path);

        strcat(query_path, "/.query");
        if(strcmp(query_path, path) == 0){
            LM_DATA->M_bit[i] = 1;

            db_fullpath(load_path, temp_path);
            strcat(load_path, ".tbl");

            strcat(dump_path, "/_dump.tbl");

            run_query(buf, load_path, LM_DATA->table_name[i], dump_path);
            // run the query TODO
            // create res.tbl to store result TODO
            rt = pwrite(fi->fh, buf, size, offset);
            break;
        }
    }

    return rt;
}

static int lemonDBfs_flush(const char *path, struct fuse_file_info *fi)
{
    printf("flush(\"%s\"\n", path);
    return 0;
}

static int lemonDBfs_truncate(const char *path, off_t newsize)
{
    printf("truncate(\"%s\"\n", path);
    char fpath[PATH_MAX] = {0};
    char tmp_path[PATH_MAX] = {0};

    for(int i = 0; i < LM_DATA->num_tbl; i++){
        strcpy(tmp_path, "/");
        strcat(tmp_path, LM_DATA->table_name[i]); // tmp_path := /mtable0 (directory)
        db_fullpath(fpath, tmp_path); // fpath := *db_dir*/mtable0
        strcat(fpath, ".tbl"); // fpath := *db_dir*/mtable0.tbl
        strcat(tmp_path, "/orig.tbl"); // tmp_path := /mtable0/orig.tbl (file)
        if(strcmp(tmp_path, path) == 0){
            truncate(fpath, newsize);
        }
    }
    return 0;
}

static void lemonDBfs_destroy(void *userdata)
{
    printf("lemonDBfs destroy!\n");
    char dpath[PATH_MAX];
    for (int i = 0;i < LM_DATA->num_tbl;++i) {
        if (LM_DATA->M_bit[i]) {
            char tmp_path[PATH_MAX] = {0};
            strcpy(tmp_path, "/");
            strcat(tmp_path, LM_DATA->table_name[i]); // tmp_path := /mtable0 (directory)
            db_fullpath(dpath, tmp_path); // dpath := *db_dir*/mtable0
            strcat(dpath, "_dump.tbl"); // dpath := *db_dir*/mtable0_dump.tbl

            if (access(dpath, F_OK) != -1) remove(dpath);
        }
    }
}


static struct fuse_operations lemonDBfs_oper = {
        .init = lemonDBfs_init,
        .getattr = lemonDBfs_getattr,
        .readdir = lemonDBfs_readdir,
        .open = lemonDBfs_open,
        .read = lemonDBfs_read,
        .write = lemonDBfs_write,
        .flush = lemonDBfs_flush,
        .truncate = lemonDBfs_truncate,
        .destroy = lemonDBfs_destroy,
};

int main(int argc, char **argv)
{

    struct lemondb_state *lemondb_data;

    if ((getuid() == 0) || (geteuid() == 0)) {
        fprintf(stderr, "Running lemonDBfs as root opens unnacceptable security holes\n");
        return 1;
    }

    if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
        lemonDB_usage();


    lemondb_data = malloc(sizeof(struct lemondb_state));
    if (lemondb_data == NULL) {
        perror("main calloc");
        abort();
    }

    // Pull the rootdir out of the argument list and save it in my
    // internal data

    lemondb_data->db_dir = realpath(argv[argc-2], NULL);
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;

    // read all the table name
    lemondb_data->num_tbl = 0;
    DIR *dir;
    struct dirent *ptr;
    if ((dir=opendir(lemondb_data->db_dir)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }
    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
        lemondb_data->table_name[lemondb_data->num_tbl] = malloc(sizeof(char)*T_NAME_LENGTH);
        memset(lemondb_data->table_name[lemondb_data->num_tbl], '\0', sizeof(char)*T_NAME_LENGTH);
        strncpy(lemondb_data->table_name[lemondb_data->num_tbl], ptr->d_name, strlen(ptr->d_name)-4);
        lemondb_data->M_bit[lemondb_data->num_tbl] = 0;
        lemondb_data->num_tbl++;
    }
    closedir(dir);

    /*for(int i = 0; i < lemondb_data->num_tbl; i++){
        printf("%s\n", lemondb_data->table_name[i]);
    }*/

    return fuse_main(argc, argv, &lemonDBfs_oper, lemondb_data);
}