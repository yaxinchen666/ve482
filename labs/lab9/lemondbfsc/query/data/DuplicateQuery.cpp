//
// Created by Xie Jinglei on Oct. 21st 2019
//

#include "DuplicateQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *DuplicateQuery::qname;

QueryResult::Ptr DuplicateQuery::execute() {
    using namespace std;

    // Operands for duplicate must be empty
    if (!this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );

    Database &db = Database::getInstance();
    Table::SizeType counter = 0;

    try {
        auto &table = db[this->targetTable];

        auto result = initCondition(table);
        if (result.second) {
            auto it_orig = table.end();
            size_t num_field = table.field().size();
            vector<Table::KeyType> keys;
            vector<vector<Table::ValueType>> values;

            for (auto it = table.begin(); it != it_orig; it++) {
                // Search for a row that satisfies the condition   

                if (this->evalCondition(*it)) {
                    // Get the key
                    Table::KeyType key = it->key();
                    key = key + "_copy";

                    // If the key has already been copied, then skip
                    bool copied = false;
                    for (auto it2 = it; it2 != it_orig; it2++) {
                        if (it2->key() == key) {
                            copied = true;
                            break;
                        }
                    }
                    if (copied == true) 
                        continue;

                    keys.push_back(key);

                    // Get the data
                    vector<Table::ValueType> row_data;
                    for (size_t i = 0; i < num_field; i++) {
                        row_data.push_back((*it)[i]);
                    }
                    values.push_back(row_data);

                    counter++;
                }
            }
            // Insert
            for (size_t j = 0; j < counter; j++) {
                table.insertByIndex(keys[j], move(values[j]));
            }
        }

        return make_unique<RecordCountResult>(counter);
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

std::string DuplicateQuery::toString() {
    return "QUERY = DUPLICATE " + this->targetTable + "\"";
}