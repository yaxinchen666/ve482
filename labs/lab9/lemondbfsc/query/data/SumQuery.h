//
// Created by Wang Yuanhao on 19-10-21.
//
#ifndef SUM_H
#define SUM_H

#include "../Query.h"
class SumQuery : public ComplexQuery {
    static constexpr const char *qname = "SUM";
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif
