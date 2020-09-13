//
// Created by liu on 18-10-21.
//

#include "run_query.h"
#include "query/QueryParser.h"
#include "query/QueryBuilders.h"

#include <getopt.h>
#include <iostream>
#include <string>
#include <sstream>

struct {
    std::string listen;
    long threads = 0;
} parsedArgs;

std::string extractQueryString(std::istream &is) {
    std::string buf;
    do {
        int ch = is.get();
        if (ch == ';') return buf;
        if (ch == EOF) throw std::ios_base::failure("End of input");
        buf.push_back((char) ch);
    } while (true);
}

int run_query(const char *buf, const char *tableFilePath, const char *table, const char *file_path) {
    // Assume only C++ style I/O is used in lemondb
    // Do not use printf/fprintf in <cstdio> with this line

    std::ostringstream out;

    out << buf<<";\n";


    std::istringstream is(out.str());

    std::ostringstream os2(file_path);


    QueryParser p;

    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());

    size_t counter = 0;

    while (is) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString(is);
            Query::Ptr query = p.parseQuery(queryStr);
            QueryResult::Ptr result = query->execute();
            std::cout << ++counter << "\n";
            if (result->success()) {
                if (result->display()) {
                    os2 << *result;
                    std::cout << *result;
                    std::cout.flush();
                } else {
#ifndef NDEBUG
                    std::cout.flush();
                    std::cerr << *result;
#endif
                }
            } else {
                std::cout.flush();
                std::cerr << "QUERY FAILED:\n\t" << *result;
            }
        } catch (const std::ios_base::failure &e) {
            // End of input
            break;
        } catch (const std::exception &e) {
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}
