//
// Created by cyx on 19-10-21.
//

#ifndef PROJECT_ADDQUERY_H
#define PROJECT_ADDQUERY_H

#include "../Query.h"

#include <vector>

class AddQuery : public ComplexQuery {
    static constexpr const char *qname = "ADD";
    size_t num; // number of string in (); i.e. size(fieldsId) = num - 1;
    std::vector<Table::FieldIndex> fieldsId;
    Table::FieldIndex DestfieldId;
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;


};

#endif //PROJECT_ADDQUERY_H
