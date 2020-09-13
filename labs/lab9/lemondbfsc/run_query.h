//
// Created by cyx on 19-12-15.
//

#ifndef LEMONDBFSC_EXECUTE_QUERY_H
#define LEMONDBFSC_EXECUTE_QUERY_H

#ifdef __cplusplus
extern "C" {
#endif

int run_query(const char *buf, const char *tableFilePath, const char *table, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif //LEMONDBFSC_EXECUTE_QUERY_H
