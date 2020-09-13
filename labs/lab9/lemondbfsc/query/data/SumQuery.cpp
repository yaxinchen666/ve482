//
// Created by Wang Yuanhao on 19-10-21.
//
#include <iostream>
#include "SumQuery.h"
#include "../../db/Database.h"

constexpr const char *SumQuery::qname;	//It seems that this line is a kind of Naming Format.
QueryResult::Ptr SumQuery::execute() {
	using namespace std;
	Database &db = Database::getInstance();
	 if (this->operands.size() == 0){
	 	return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),"Invalid number of operands (? operands)."_f % operands.size());
	 }
    try{	
    /*** The major function is written below. ***/
    	auto &table = db[this->targetTable];
        auto result = initCondition(table);

        // Grab the fields in : SUM ( fields ... ) 
        vector<Table::FieldIndex> field_index;			// The FieldIndex is not integer.
        vector<int> Results;
        for (unsigned i = 0; i < this->operands.size(); i++) {
            Results.push_back(0);
        }
        for (auto itr = this->operands.begin(); itr != this->operands.end(); itr++) 
            field_index.push_back(table.getFieldIndex(*itr));	// The fields ... are pushed into the vector.
        if (result.second == true) {
            for (auto itr = table.begin(); itr != table.end(); itr++) {
                if (this->evalCondition(*itr) == true) {
                	for (unsigned i = 0; i < this->operands.size(); i++) {
                        Results[i] = Results[i] + (*itr)[field_index[i]];	//Similar to SELECT.
                    }
                }
            }
        }
        return make_unique<SuccessMsgResult>(Results);
        
    // The rest part seems to be kind of Error Handling Standard. DO NOT MODIFY.
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

std::string SumQuery::toString() {	// What is this used for?? DEBUG??? I don't know it but I found something similar in UpdateQuery.cpp
    return "QUERY = SUM" + this->targetTable + "\"";
}
