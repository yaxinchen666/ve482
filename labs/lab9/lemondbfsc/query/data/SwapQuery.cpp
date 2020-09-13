//
// Created by TIan on 19-10-20
//

#include "SwapQuery.h"
#include "../../db/Database.h"

constexpr const char *SwapQuery::qname;

QueryResult::Ptr SwapQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2 || this->operands[0] == "KEY" || this->operands[1] == "KEY")
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;

    try {
        auto &table = db[this->targetTable];
        this->fieldId1 = table.getFieldIndex(this->operands[0]);
        this->fieldId2 = table.getFieldIndex(this->operands[1]);
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    Table::ValueType  temp = (*it)[this->fieldId1];
                    (*it)[this->fieldId1] =  (*it)[this->fieldId2];
                    (*it)[this->fieldId2] = temp;
                    ++counter;
                }
            }
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

std::string SwapQuery::toString() {
    return "QUERY = SWAP " + this->targetTable + "\"";
}