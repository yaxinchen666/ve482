//
// Created by Xie Jinglei on 19-10-21.
//

#include "MaxQuery.h"
#include "../../db/Database.h"

constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
    using namespace std;

    Database &db = Database::getInstance();
    Table::SizeType counter = 0;

    // No operand is not valid
    if (this->operands.size() == 0)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );

    try {
        auto &table = db[this->targetTable];

        // Fields indices to be considered
        vector<Table::FieldIndex> fields_index;

        // Maximum numbers of each field
        vector<Table::ValueType> maxes(this->operands.size());

        // Get the filed indices
        for (size_t i = 0; i < this->operands.size(); i++) {
            fields_index.push_back(table.getFieldIndex(this->operands[i]));
        }

        /*
        // Initialize the maxes vector using the first row
        for (size_t i = 0; i < this->operands.size(); i++) {
            maxes.push_back((*table.begin())[fields_index[i]]);
        }
        */

        auto result = initCondition(table);

        if (result.second) {
            for (auto it = table.begin(); it != table.end(); ++it) {
                // Search for a row that satisfies the condition
                if (this->evalCondition(*it)) {
                    for (size_t j = 0; j < this->operands.size(); j++) {
                        // If greater, then replace
                        if ((*it)[fields_index[j]] > maxes[j] || counter == 0)
                            maxes[j] = (*it)[fields_index[j]];
                    }
                    // Count the number of rows satisfying the condition
                    counter++;
                }
            }
        }

        if (counter > 0)
            return make_unique<SuccessMsgResult>(maxes);
        else
            return make_unique<NullQueryResult>();       
    }
    catch (const TableNameNotFound &e) {
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

std::string MaxQuery::toString() {
    return "QUERY = MAX " + this->targetTable + "\"";
}
