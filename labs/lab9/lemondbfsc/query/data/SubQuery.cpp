//
// Created by Wang Yuanhao on 19-10-21.
//
#include <iostream>
#include "SubQuery.h"
#include "../../db/Database.h"

constexpr const char *SubQuery::qname;	//It seems that this line is a kind of Naming Format.
QueryResult::Ptr SubQuery::execute() {
	using namespace std;
	Database &db = Database::getInstance();
	 if (this->operands.size() <= 1){
	 	return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),"Invalid number of operands (? operands)."_f % operands.size());
	 }
    try{	
    /*** The major function is written below. ***/
    	auto &table = db[this->targetTable];
        auto result = initCondition(table);
        int num = 0;
        int temp= 0;
        // Grab the fields in : SUB (  fieldSrc fields ... destField ) 
        vector<Table::FieldIndex> field_index;			// The FieldIndex is not integer.
        for (auto itr = this->operands.begin(); itr != this->operands.end(); itr++) 
            field_index.push_back(table.getFieldIndex(*itr));	// The fieldsrc fields ... destField are pushed into the vector.
        if (result.second == true) {
            for (auto itr = table.begin(); itr != table.end(); itr++) {
                if (this->evalCondition(*itr) == true) {
                	num++;								// Each time num plus one.					
                    auto fieldsrc_itr = field_index.begin();
                    auto itp = fieldsrc_itr;
                    itp++;								//Skip the fieldsrc. Count from the field1.
                    auto destfield_itr= field_index.end();
                    destfield_itr--; 					//The end is point to empty. We need minus 1 to get destfield.
                    temp = (*itr)[*fieldsrc_itr];		// Grab value from fieldsrc
                    
                    while(itp != destfield_itr){
                    	temp = temp - (*itr)[*itp];
                    	itp++;
                    }
                    (*itr)[*destfield_itr] = temp;		// Give value to destfield
                }
            }
        }
        return make_unique<RecordCountResult>(num);
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

std::string SubQuery::toString() {	// What is this used for?? DEBUG??? I don't know it but I found something similar in UpdateQuery.cpp
    return "QUERY = SUB" + this->targetTable + "\"";
}
