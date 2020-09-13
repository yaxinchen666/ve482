//
//  create by Tian on 19-10-20
//

#include "CopyTableQuery.h"
#include "../../db/Database.h"


constexpr const char *CopyTableQuery::qname;

QueryResult::Ptr CopyTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &target = db[this->targetTable];
        auto newTable = make_unique<Table>(this->newTable, target);
        db.registerTable(std::move(newTable));
        return make_unique<SuccessMsgResult>(qname, targetTable);
    } catch(const exception& e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

std::string CopyTableQuery::toString() {
    return "QUERY = Copy TABLE \"" + this->targetTable + "\"";
}

/*std::string get_newTable_name() {
    return this->newTable;
}*/