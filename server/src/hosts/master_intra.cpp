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

#define QRY_LICENSE_VERIFY 1
    this->queries.push_back(new db::Query(
            "SELECT COUNT(*) FROM `SERVER_LICENSES` "
            "WHERE `KEY_ID` = ?"
        ));

#define QRY_LICENSE_LIMIT 2
    this->queries.push_back(new db::Query(
        "SELECT `ALLOWANCE` FROM `SERVER_LICENSES` WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_ACTIVE_COUNT 3
    this->queries.push_back(new db::Query(
        "SELECT COUNT(*) FROM `SERVER_LIST` WHERE `LICENSE` = ?"
    , DB_USE_MEMORY));

#define QRY_LICENSE_ADD 4
    this->queries.push_back(new db::Query(
        "INSERT OR IGNORE INTO `SERVER_LICENSES` "
        "(`KEY_ID`, `SECRET`, `ALLOWANCE`) "
        "VALUES (?, RANDOMBLOB(512), ?)"
    ));

#define QRY_LICENSE_REMOVE 5
    this->queries.push_back(new db::Query(
        "DELETE FROM `SERVER_LICENSES` "
        "WHERE `KEY_ID` = ?"
    ));

#define QRY_LICENSE_MODIFY 6
    this->queries.push_back(new db::Query(
        "UPDATE `SERVER_LICENSES` "
        "SET `ALLOWANCE` = ? WHERE `KEY_ID` = ?"
    ));

#define QRY_SERVER_LIST_ADD 7
    this->queries.push_back(new db::Query(
        "INSERT INTO `SERVER_LIST` "
        "(`NAME`, `LICENSE`, `IP_ADDR`, `PORT`) "
        "VALUES (?, ?, ?, ?)"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_GET_ID 8
    this->queries.push_back(new db::Query(
        "SELECT MAX(`ID`) FROM `SERVER_LIST`"
    , DB_USE_MEMORY));

#define QRY_SERVER_LIST_DELETE 9
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

bool sosc::MasterIntra::InitAttempt(sosc::Packet& pck) {
    if(!pck.Check(1, key.key_size_bytes))
        return this->Close(
            Packet(kEncryptionError, { net::htonv<uint16_t>(0x100) }));

    Packet response;
    if(!this->key.ParseRequest(pck, &response, kKeyExchange))
        return this->Close(
            Packet(kEncryptionError, { net::htonv<uint16_t>(0x101) }));

    this->cipher = cgc::Cipher(this->key);
    this->sock.Send(response);
    this->sock.SetCipher(&this->cipher);
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

    if(!pck.Check(2, 2, 2))
        return this->Close();



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