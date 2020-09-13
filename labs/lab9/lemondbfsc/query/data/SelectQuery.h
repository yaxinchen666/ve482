//
// Created by Wang Yuanhao on 19-10-21.
//
#ifndef SQ_H
#define SQ_H

#include "../Query.h"
class SelectQuery : public ComplexQuery {
    static constexpr const char *qname = "SELECT";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif
