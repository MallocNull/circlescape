#include "master.hpp"
#include "../db/database.hpp"
#include <mutex>

static struct {
    std::mutex license_check_mtx;
} _ctx;

/** MASTERINTRAPOOL CODE **/

sosc::MasterIntraPool::MasterIntraPool() {
#define QRY_LICENSE_CHECK 0
    this->queries.push_back(new db::Query(
        "SELECT COUNT(*) FROM `SERVER_LICENSES` "
        "WHERE `KEY_ID` = ? AND `SECRET` = ?"
    ));

#define QRY_LICENSE_LIMIT 1
    this->queries.push_back(new db::Query(
        "SELECT `ALLOWANCE` FROM `SERVER_LICENSES` WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_ACTIVE_COUNT 2
    this->queries.push_back(new db::Query(
        "SELECT COUNT(*) FROM `SERVER_LIST` WHERE `LICENSE` = ?"
    , DB_USE_MEMORY));

#define QRY_LICENSE_ADD 3
    this->queries.push_back(new db::Query(
        "INSERT OR IGNORE INTO `SERVER_LICENSES` "
        "(`KEY_ID`, `SECRET`, `ALLOWANCE`) "
        "VALUES (?, RANDOMBLOB(512), ?)"
    ));

#define QRY_LICENSE_REMOVE 4
    this->queries.push_back(new db::Query(
        "DELETE FROM `SERVER_LICENSES` "
        "WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_MODIFY 5
    this->queries.push_back(new db::Query(
        "UPDATE `SERVER_LICENSES` "
        "SET `ALLOWANCE` = ? WHERE `KEY_ID` = ?"
    ));

#define QRY_SERVER_LIST_ADD 6
    this->queries.push_back(new db::Query(
        "INSERT INTO `SERVER_LIST` "
        "(`NAME`, `LICENSE`, `IP_ADDR`, `PORT`) "
        "VALUES (?, ?, ?, ?)"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_DELETE 7
    this->queries.push_back(new db::Query(
        "DELETE FROM `SERVER_LIST` WHERE `ID` = ?"
    , DB_USE_MEMORY));

}

void sosc::MasterIntraPool::Stop() {
    Pool<MasterIntra>::Stop();

    for(auto& query : this->queries) {
        query->Close();
        delete query;
    }
}

/** MASTERINTRA CODE **/

sosc::MasterIntra::MasterIntra(const IntraClient& client) {
    this->sock = client;
    this->authed = false;
    this->auth_attempts = 0;
}

bool sosc::MasterIntra::Process(const db::QueryList* queries) {
    Packet pck;
    int status = this->sock.Receive(&pck);
    if(status == PCK_ERR)
        return this->Close();
    else if(status == PCK_MORE)
        return true;

    this->queries = queries;
    switch(pck.GetId()) {
        case kInitAttempt:
            return this->InitAttempt(pck);
        case kAuthentication:
            return this->Authentication(pck);
        case kStatusUpdate:
            return this->StatusUpdate(pck);
        default:
            return this->Close();
    }
}

bool sosc::MasterIntra::InitAttempt(sosc::Packet &pck) {
    if(!pck.Check(1, key.key_size_bytes))
        return this->Close(Packet(kEncryptionError, { "\x01" }));

    Packet response;
    if(!this->key.ParseRequest(pck, &response, kKeyExchange))
        return this->Close(Packet(kEncryptionError, { "\x02" }));

    this->sock.Send(response);
}

bool sosc::MasterIntra::Authentication(sosc::Packet &pck) {
    if(this->authed)
        return true;

    std::string packetId = BYTESTR(kAuthentication);
    if(!pck.Check(3, PCK_ANY, PCK_ANY, 512))
        return this->Close();

    db::Query* query = this->queries->at(QRY_LICENSE_CHECK);
    query->Reset();
    query->BindText(pck[1], 0);
    query->BindBlob(pck[2], 1);
    if(query->ScalarInt32() == 0)
        return AuthenticationFailure(packetId, 2);

    _ctx.license_check_mtx.lock();

    int limit;
    query = this->queries->at(QRY_LICENSE_LIMIT);
    query->Reset();
    query->BindText(pck[1], 0);
    if((limit = query->ScalarInt32()) != 0) {
        query = this->queries->at(QRY_LICENSE_ACTIVE_COUNT);
        query->Reset();
        query->BindText(pck[1], 0);
        if(query->ScalarInt32() < limit) {
            _ctx.license_check_mtx.unlock();
            return AuthenticationFailure(packetId, 3);
        }
    }

    _ctx.license_check_mtx.unlock();

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
            Packet(kNegativeAck, { packetId, net::htonv<uint16_t>(1) })
        );
    }
}

bool sosc::MasterIntra::StatusUpdate(sosc::Packet &pck) {

}

bool sosc::MasterIntra::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterIntra::Close(const Packet &message) {
    this->sock.Send(message);
    this->Close();
}