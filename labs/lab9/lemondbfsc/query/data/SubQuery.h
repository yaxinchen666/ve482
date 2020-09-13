//
// Created by Wang Yuanhao on 19-10-21.
//
#ifndef SUB_H
#define SUB_H

#include "../Query.h"
class SubQuery : public ComplexQuery {
    static constexpr const char *qname = "SUB";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif
