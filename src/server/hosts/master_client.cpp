#include "master.hpp"

static struct {
    std::mutex user_key_mtx;
} _ctx;

/** MASTERCLIENTPOOL CODE **/

void sosc::MasterClientPool::SetupQueries(db::Queries *queries) {
#define QRY_USER_REG_CHECK 0
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USERS` "
        "WHERE `USERNAME` = ? OR `EMAIL` = ?"
    ));

#define QRY_USER_REGISTER 1
    queries->push_back(new db::Query(
        "INSERT INTO `USERS` "
        "(`USERNAME`, `PASS_HASH`, `EMAIL`, `ACTIVATED`, `JOINED`) "
        "VALUES (?, ?, ?, 0, CURRENT_TIMESTAMP)"
    ));

#define QRY_USER_GET_PWD_HASH 2
    queries->push_back(new db::Query(
        "SELECT `ID`, `PASS_HASH` FROM `USERS` "
        "WHERE `USERNAME` = ?"
    ));

#define QRY_USER_GENERATE_KEY 3
    queries->push_back(new db::Query(
        "INSERT OR IGNORE INTO `USER_KEYS` "
        "(`ID`, `SECRET`) VALUES (?, RANDOMBLOB(128))"
    ));

#define QRY_USER_GET_KEY 4
    queries->push_back(new db::Query(
        "SELECT `SECRET` FROM `USER_KEYS` "
        "WHERE `ID` = ?"
    ));

#define QRY_USER_CHECK_KEY 5
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USER_KEYS` "
        "WHERE `ID` = ? AND `SECRET` = ?"
    ));
}

/** MASTERCLIENT CODE **/

sosc::MasterClient::MasterClient(const ScapeConnection &client) {
    this->sock = client;
    this->authed = false;
    this->auth_attempts = 0;
}

bool sosc::MasterClient::Process(const db::Queries *queries) {
    if(!this->sock.Handshaked())
        return (this->sock.Handshake() != WS_SHAKE_ERR);

    Packet pck;
    int status = this->sock.Receive(&pck);
    if(status == PCK_ERR)
        return this->Close();
    else if(status == PCK_MORE)
        return true;

    this->queries = queries;
    switch(pck.GetId()) {
        case kLoginRequest:

            break;
        case kRegisterRequest:

            break;
        case kServerListRequest:

            break;
        default:
            return this->Close();
    }
}

bool sosc::MasterClient::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterClient::Close(const Packet& message) {
    this->sock.Send(message);
    this->Close();
}