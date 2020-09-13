# VE482 Lab8
## PGroup-pair 6
Xie Jinglei 517370910022 <br>
Chen Yaxin  517021911020

## Changes
Our changes mainly deal with the differences of old and new Linux versions. To make dadfs compile and run on our own computers, we read kernel source codes of both v4.15 (Linux kernel of our own computers) and older versions and alter the `base.c` file according to the discoveries. Here we show part of our patch file to illustrate the changes.

Also we add some new functions to fix the bug of "Cannot locate journal superblock".

### Add new header
```diff
@@ -15,6 +15,9 @@
 #include <linux/parser.h>
 #include <linux/blkdev.h>
 
+//TODO
+#include <linux/uio.h>
+
 #include "sblock.h"
 
 #ifndef f_dentry
 ```

 ### Change `dadfs_read`
 Add new syntax so that it can run in the new kernel version.

 ```diff
 @@ -268,12 +271,25 @@
 	return inode_buffer;
 }
 
+//TODO
+#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+ssize_t dadfs_read(struct kiocb *kiocb, struct iov_iter *to)
+#else
 ssize_t dadfs_read(struct file * filp, char __user * buf, size_t len,
 		      loff_t * ppos)
+#endif
 {
 	/* After the commit dd37978c5 in the upstream linux kernel,
 	 * we can use just filp->f_inode instead of the
 	 * f->f_path.dentry->d_inode redirection */
+
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		struct file *filp = kiocb->ki_filp;
+		size_t len = to->count;
+		loff_t *ppos = &(kiocb->ki_pos);
+	#endif
+
 	struct dadfs_inode *inode =
 	    DADFS_INODE(filp->f_path.dentry->d_inode);
 	struct buffer_head *bh;
```

Replace `copy_to_user` with `copy_to_iter` for the new verson. Note that in the new version, an error would cause the function to return 0.

```diff
@@ -298,7 +314,13 @@
 	buffer = (char *)bh->b_data;
 	nbytes = min((size_t) inode->file_size, len);
 
-	if (copy_to_user(buf, buffer, nbytes)) {
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		if (!copy_to_iter(buffer, nbytes, to))	// when error occurs, return 0
+	#else
+		if (copy_to_user(buf, buffer, nbytes)) 
+	#endif
+	{
 		brelse(bh);
 		printk(KERN_ERR
 		       "Error copying file contents to the userspace buffer\n");
```

### Change `dadfs_write`
Add new syntax so that it can run in the new kernel version.

```diff
@@ -352,8 +374,13 @@
 
 /* FIXME: The write support is rudimentary. I have not figured out a way to do writes
  * from particular offsets (even though I have written some untested code for this below) efficiently. */
+//TODO
+#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+ssize_t dadfs_write(struct kiocb *kiocb, struct iov_iter *from)
+#else
 ssize_t dadfs_write(struct file * filp, const char __user * buf, size_t len,
 		       loff_t * ppos)
+#endif
 {
 	/* After the commit dd37978c5 in the upstream linux kernel,
 	 * we can use just filp->f_inode instead of the
```

Change the syntax of `generic_write_checks` function. In the new version, the arguments of the function are different.

```diff
@@ -369,13 +396,27 @@
 
 	int retval;
 
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		struct file * filp = kiocb->ki_filp;
+		size_t len = from->count;
+		loff_t *ppos = &(kiocb->ki_pos);
+	#endif
+
 	sb = filp->f_path.dentry->d_inode->i_sb;
 	sfs_sb = DADFS_SB(sb);
 
 	handle = jbd2_journal_start(sfs_sb->journal, 1);
 	if (IS_ERR(handle))
 		return PTR_ERR(handle);
-	retval = generic_write_checks(filp, ppos, &len, 0);
+
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		retval = generic_write_checks(kiocb, from);
+	#else
+		retval = generic_write_checks(filp, ppos, &len, 0);
+	#endif
+	
 	if (retval)
 		return retval;
```

Replace `copy_from_user` with `copy_from_iter` for the new verson. Note that in the new version, an error would cause the function to return 0.

```diff
@@ -402,7 +443,13 @@
 		return retval;
 	}
 
-	if (copy_from_user(buffer, buf, len)) {
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		if (!copy_from_iter(buffer, len, from))	// when error occurs, return 0
+	#else
+		if (copy_from_user(buffer, buf, len)) 
+	#endif
+	{
 		brelse(bh);
 		printk(KERN_ERR
 		       "Error copying file contents from the userspace buffer to the kernel space\n");
```

### Change struct `file_operations`
Change the struct according to the API defined in the new kernel source code. Function pointers `.read_iter` and `.write_iter` are used in the new version.

```diff
@@ -447,8 +494,19 @@
 }
 
 const struct file_operations dadfs_file_operations = {
-	.read = dadfs_read,
-	.write = dadfs_write,
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		.read_iter = dadfs_read,
+	#else
+		.read = dadfs_read,
+	#endif
+
+	//TODO
+	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
+		.write_iter = dadfs_write,
+	#else
+		.write = dadfs_write,
+	#endif
 };
```

### Fix the "Cannot locate journal superblock" error
Previously we cannot `mount` dadfs onto the directory because of this error. After altering the code, it seems to work now.
```diff
@@ -613,6 +671,19 @@
 	return dadfs_create_fs_object(dir, dentry, mode);
 }
 
+// TODO
+static sector_t dadfs_bmap(struct address_space *mapping, sector_t block)
+{
+	//return generic_block_bmap(mapping,block,dadfs_get_blocks);
+	return 1;
+}
+
+static const struct address_space_operations dadfs_aops = {
+	//.readpage = efs_readpage,
+	.bmap = dadfs_bmap
+};
+
+
 static struct inode *dadfs_iget(struct super_block *sb, int ino)
 {
 	struct inode *inode;
@@ -633,6 +704,10 @@
 		printk(KERN_ERR
 					 "Unknown inode type. Neither a directory nor a file");
 
+	//TODO: 
+	inode->i_mapping->a_ops = &dadfs_aops;
+	if(!inode->i_mapping->a_ops->bmap) printk(KERN_ERR "nomap!");
+
 	/* FIXME: We should store these times to disk and retrieve them */
 	inode->i_atime = inode->i_mtime = inode->i_ctime =
 			current_time(inode);
```

## Results
Please see the report `ve482_lab8_XieJinglei_ChenYaxin.pdf`.