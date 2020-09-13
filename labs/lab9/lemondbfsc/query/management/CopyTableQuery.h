//
//  create by Tian on 19-10-20
//

#ifndef PROJECT_COPYTABLEQUERY_H
#define PROJECT_COPYTABLEQUERY_H

#include "../Query.h"

class CopyTableQuery: public Query {
    static constexpr const char *qname = "COPYTABLE";
    const std::string newTable;
public:
    CopyTableQuery(std::string table, std::string newtable)
        : Query(std::move(table)), newTable(std::move(newtable)) {}

    QueryResult::Ptr execute() override;

    std::string toString() override;

    //std::string get_newTable_name() override;

};

#endif