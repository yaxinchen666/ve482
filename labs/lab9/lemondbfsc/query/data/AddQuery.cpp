//
// Created by cyx on 19-10-21.
//

#include "AddQuery.h"
#include "../../db/Database.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>

constexpr const char *AddQuery::qname;

QueryResult::Ptr AddQuery::execute() {
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        this->num = this->operands.size();
        for(size_t i = 0; i < this->num - 1; i++){
            Table::FieldIndex temp = table.getFieldIndex(this->operands[i]);
            this->fieldsId.push_back(temp);
        }
        this->DestfieldId = table.getFieldIndex(this->operands[this->num-1]);

        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    Table::ValueType sum = 0;
                    for(size_t i = 0; i < this->num - 1; i++){
                        sum += (*it)[this->fieldsId[i]];
                    }
                    (*it)[this->DestfieldId] = sum;
                    ++counter;
                }
            }
        }
        return make_unique<RecordCountResult>(counter);

    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}


std::string AddQuery::toString() {
    return "QUERY = ADD " + this->targetTable + "\"";
}
