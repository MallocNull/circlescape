#include "master.hpp"

static struct {
    std::mutex license_check_mtx;
} _ctx;

/** MASTERINTRAPOOL CODE **/

void sosc::MasterIntraPool::SetupQueries(Queries* queries) {
#define QRY_LICENSE_CHECK 0
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `SERVER_LICENSES` "
        "WHERE `KEY_ID` = ? AND `SECRET` = ?"
    ));

#define QRY_LICENSE_VERIFY 1
    queries->push_back(new db::Query(
            "SELECT COUNT(*) FROM `SERVER_LICENSES` "
            "WHERE `KEY_ID` = ?"
        ));

#define QRY_LICENSE_LIMIT 2
    queries->push_back(new db::Query(
        "SELECT `ALLOWANCE` FROM `SERVER_LICENSES` WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_ACTIVE_COUNT 3
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `SERVER_LIST` WHERE `LICENSE` = ?"
    , DB_USE_MEMORY));

#define QRY_LICENSE_ADD 4
    queries->push_back(new db::Query(
        "INSERT OR IGNORE INTO `SERVER_LICENSES` "
        "(`KEY_ID`, `SECRET`, `ALLOWANCE`) "
        "VALUES (?, RANDOMBLOB(512), ?)"
    ));

#define QRY_LICENSE_REMOVE 5
    queries->push_back(new db::Query(
        "DELETE FROM `SERVER_LICENSES` "
        "WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_MODIFY 6
    queries->push_back(new db::Query(
        "UPDATE `SERVER_LICENSES` "
        "SET `ALLOWANCE` = ? WHERE `KEY_ID` = ?"
    ));

#define QRY_SERVER_LIST_ADD 7
    queries->push_back(new db::Query(
        "INSERT INTO `SERVER_LIST` "
        "(`NAME`, `LICENSE`, `IP_ADDR`, `PORT`) "
        "VALUES (?, ?, ?, ?)"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_GET_ID 8
    queries->push_back(new db::Query(
        "SELECT MAX(`ID`) FROM `SERVER_LIST`"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_MODIFY 9
    queries->push_back(new db::Query(
        "UPDATE `SERVER_LIST` SET "
        "`USERS` = ?, `MAX_USERS` = ? "
        "WHERE ID = ?"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_DELETE 10
    queries->push_back(new db::Query(
        "DELETE FROM `SERVER_LIST` WHERE `ID` = ?"
    , DB_USE_MEMORY));
}

/** MASTERINTRA CODE **/

sosc::MasterIntra::MasterIntra(const IntraClient& client) {
    this->sock = client;
    this->authed = false;
    this->auth_attempts = 0;
}

bool sosc::MasterIntra::Process(const Queries* queries) {
    Packet pck;
    int status = this->sock.Receive(&pck);
    if(status == PCK_ERR)
        return this->Close();
    else if(status == PCK_MORE)
        return true;

    this->queries = queries;
    switch(pck.GetId()) {
        case kAuthentication:
            return this->Authentication(pck);
        case kStatusUpdate:
            return this->StatusUpdate(pck);
        default:
            return this->Close();
    }
}

bool sosc::MasterIntra::Authentication(sosc::Packet& pck) {
    if(this->authed)
        return true;

    std::string packetId = BYTESTR(kAuthentication);
    if(!pck.Check(4, PCK_ANY, 2, PCK_ANY, 512))
        return this->Close();

    db::Query* query = this->queries->at(QRY_LICENSE_CHECK);
    query->Reset();
    query->BindText(pck[2], 0);
    query->BindBlob(pck[3], 1);
    if(query->ScalarInt32() == 0)
        return AuthenticationFailure(packetId, 0x101);

    _ctx.license_check_mtx.lock();

    int limit;
    query = this->queries->at(QRY_LICENSE_LIMIT);
    query->Reset();
    query->BindText(pck[2], 0);
    if((limit = query->ScalarInt32()) != 0) {
        query = this->queries->at(QRY_LICENSE_ACTIVE_COUNT);
        query->Reset();
        query->BindText(pck[2], 0);
        if(query->ScalarInt32() < limit) {
            _ctx.license_check_mtx.unlock();
            return AuthenticationFailure(packetId, 0x102);
        }
    }

    query = this->queries->at(QRY_SERVER_LIST_ADD);
    query->Reset();
    query->BindText(pck[0], 0);
    query->BindText(pck[2], 1);
    query->BindText(this->sock.GetIpAddress(), 2);
    query->BindInt32(net::ntohv<uint16_t>(pck[1]), 3);
    query->NonQuery();

    query = this->queries->at(QRY_SERVER_LIST_GET_ID);
    query->Reset();
    this->server_id = query->ScalarInt32();

    _ctx.license_check_mtx.unlock();

    this->sock.Send(Packet(kPositiveAck, { packetId }));
    this->license = pck[2];
    this->authed = true;
    return true;
}

bool sosc::MasterIntra::AuthenticationFailure
    (const std::string& packetId, uint16_t errorCode)
{
    if(++this->auth_attempts < MAX_AUTH_ATTEMPTS) {
        this->sock.Send(
            Packet(kNegativeAck, { packetId , net::htonv(errorCode) })
        );
        return true;
    } else {
        return this->Close(
            Packet(kNegativeAck, { packetId, net::htonv<uint16_t>(0x100) })
        );
    }
}

bool sosc::MasterIntra::NotAuthorized(const std::string& packetId) {
    return this->Close(
        Packet(kNegativeAck, { packetId, net::htonv<uint16_t>(0x200) })
    );
}

bool sosc::MasterIntra::StatusUpdate(sosc::Packet &pck) {
    std::string packetId = BYTESTR(kStatusUpdate);
    if(!this->authed)
        return this->NotAuthorized(packetId);

    db::Query* query = this->queries->at(QRY_LICENSE_VERIFY);
    query->Reset();
    query->BindText(this->license, 0);
    if(query->ScalarInt32() == 0)
        return this->NotAuthorized(packetId);

    if(!pck.Check(2, 2, 2))
        return this->Close();

    query = this->queries->at(QRY_SERVER_LIST_MODIFY);
    query->Reset();
    query->BindInt32(net::ntohv<uint16_t>(pck[0]), 0);
    query->BindInt32(net::ntohv<uint16_t>(pck[1]), 1);
    query->BindInt32(this->server_id, 2);
    query->NonQuery();
    return true;
}

bool sosc::MasterIntra::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterIntra::Close(const Packet& message) {
    this->sock.Send(message);
    this->Close();
}