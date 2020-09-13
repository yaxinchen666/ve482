//
// Created by Wang Yuanhao on 19-10-21.
//
#include <queue>
#include <iostream>
#include "SelectQuery.h"
#include "../../db/Database.h"

constexpr const char *SelectQuery::qname;	//It seems that this line is a kind of Naming Format.
QueryResult::Ptr SelectQuery::execute() {
	using namespace std;
	Database &db = Database::getInstance();
	 if (this->operands.size() == 0){
	 	return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),"Invalid number of operands (? operands)."_f % operands.size());
	 }
    try{	/*** The major function is written below. ***/
    	auto &table = db[this->targetTable];
        auto result = initCondition(table);
        
        // Grab the fields in : SELECT (KEY field1 field2 ...)
        vector<Table::FieldIndex> field_index;			// The FieldIndex is not integer.
        for (auto itr = this->operands.begin(); itr != this->operands.end(); itr++) {
        	if(itr == this->operands.begin()) continue;	// The first operand in SELECT is "KEY". DO NOT NEED IT.
            field_index.push_back(table.getFieldIndex(*itr));	// The field1 field2 ... are pushed into the vector.
        }
        
        priority_queue<string, vector<string>, greater<string> > ready_out;
        if (result.second == true) {
            for (auto itr = table.begin(); itr != table.end(); itr++) {
                if (this->evalCondition(*itr) == true) {
                    string temp = "(";
                    temp = temp + " ";
                    temp = temp + itr->key();
                    //Since we do not push_back the 1st operand "KEY" into field_index, we have size-1 fields; Now we need to print the fields.
                    for (unsigned i = 0; i < (this->operands.size()-1) ; i++) {	  
                        temp = temp + " ";
                        //Find the exact current value. (*itr) represents the row, and its index is column
                        auto current_value = (*itr)[field_index[i]];	
                        temp = temp + to_string(current_value);	// Use function to_string (in c++11 "string.h")
                    }
                    temp = temp + " ";
                    temp = temp + ")";
                    temp = temp + "\n";
                    //cout<<"dealing:"<<temp;
                    ready_out.push(temp);
                }
            }
        }
        
        string out_return = "";
        while(ready_out.size() != 0){
            out_return = out_return + ready_out.top();
            ready_out.pop();
        }
        //cout<<out_return<<endl;
        return std::make_unique<SimpleResult>(out_return);
        
        
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

std::string SelectQuery::toString() {	// What is this used for?? DEBUG??? I don't know it but I found something similar in UpdateQuery.cpp
    return "QUERY = SELECT" + this->targetTable + "\"";
}
