//
// Created by liu on 18-10-25.
//

#include "UpdateQuery.h"
#include "../../db/Database.h"

constexpr const char *UpdateQuery::qname;

QueryResult::Ptr UpdateQuery::execute() {
    using namespace std;

    // In the Update command, the number of operands can only be 2
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );

    // Get the database
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;

    try {
        // Get the target table in db
        auto &table = db[this->targetTable];

        // If the value to be updated is a "KEY"
        if (this->operands[0] == "KEY") {
            this->keyValue = this->operands[1];
        } else {
            // If the value to be updated is a field
            this->fieldId = table.getFieldIndex(this->operands[0]);
            this->fieldValue = (Table::ValueType) strtol(this->operands[1].c_str(), nullptr, 10);
        }

        // Check the condition for table. Return a <key, bool>
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                // Search for a row that satisfies the condition
                if (this->evalCondition(*it)) {
                    // check whether this update is to change the keyname
                    if (this->keyValue.empty()) {
                        // Change the filed value
                        (*it)[this->fieldId] = this->fieldValue;
                    } else {
                        // Change the key value
                        it->setKey(this->keyValue);
                    }
                    ++counter;
                }
            }
        }

        // Affected ? rows
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

std::string UpdateQuery::toString() {
    return "QUERY = UPDATE " + this->targetTable + "\"";
}
