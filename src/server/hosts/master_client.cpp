#include "master.hpp"

static struct {
    std::mutex user_key_mtx;
} _ctx;

/** MASTERCLIENTPOOL CODE **/

void sosc::MasterClientPool::SetupQueries(db::Queries *queries) {
#define QRY_USER_NAME_REG_CHECK 0
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USERS` "
        "WHERE `USERNAME` = ?"
    ));

#define QRY_USER_MAIL_REG_CHECK 1
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USERS` "
        "WHERE `USERNAME` = ?"
    ));

#define QRY_USER_REGISTER 2
    queries->push_back(new db::Query(
        "INSERT INTO `USERS` "
        "(`USERNAME`, `PASS_HASH`, `EMAIL`, `ACTIVATED`, `JOINED`) "
        "VALUES (?, ?, ?, 0, CURRENT_TIMESTAMP)"
    ));

#define QRY_USER_NAME_EXISTS 3
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USERS` "
        "WHERE LOWER(`USERNAME`) = LOWER(?)"
    ));

#define QRY_USER_GET_PWD_HASH 4
    queries->push_back(new db::Query(
        "SELECT `ID`, `PASS_HASH` FROM `USERS` "
        "WHERE LOWER(`USERNAME`) = LOWER(?)"
    ));

#define QRY_USER_GENERATE_KEY 5
    queries->push_back(new db::Query(
        "INSERT OR IGNORE INTO `USER_KEYS` "
        "(`ID`, `SECRET`) VALUES (?, RANDOMBLOB(128))"
    , DB_USE_MEMORY));

#define QRY_USER_GET_KEY 6
    queries->push_back(new db::Query(
        "SELECT `SECRET` FROM `USER_KEYS` "
        "WHERE `ID` = ?"
    , DB_USE_MEMORY));

#define QRY_USER_CHECK_KEY 7
    queries->push_back(new db::Query(
        "SELECT COUNT(*) FROM `USER_KEYS` "
        "WHERE `ID` = ? AND `SECRET` = ?"
    , DB_USE_MEMORY));
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
            return ProcessLogin(pck);
        case kRegisterRequest:
            return ProcessRegistration(pck);
        case kServerListRequest:
            return ListServers(pck);
        default:
            return this->Close();
    }
}

bool sosc::MasterClient::ProcessLogin(Packet &pck) {
    if(this->authed)
        return true;
    if(!pck.Check(2, PCK_ANY, PCK_ANY))
        return false;
    pck.TrimRegions();

    db::ResultSet* results = nullptr;
    db::Query* query = this->queries->at(QRY_USER_NAME_EXISTS);
    query->Reset();
    query->BindText(pck[0], 1);
    if(query->ScalarInt32() == 0)
        return LoginError(0x101);

    query = this->queries->at(QRY_USER_GET_PWD_HASH);
    query->Reset();
    query->BindText(pck[0], 1);
    results = query->GetResults();
    results->Step();

    int64_t user_id = results->GetInt64(0);
    if(!cgc::bcrypt_check(pck[1], results->GetText(1)))
        return LoginError(0x102);

    query = this->queries->at(QRY_USER_GENERATE_KEY);
    query->Reset();
    query->BindInt64(user_id, 1);
    query->NonQuery();

    query = this->queries->at(QRY_USER_GET_KEY);
    query->Reset();
    query->BindInt64(user_id, 1);
    auto secret = query->ScalarBlob();

    this->sock.Send(Packet(kLoginResponse, {"\1", secret}));
    this->authed = true;
    return true;
}

bool sosc::MasterClient::LoginError(uint16_t error_code) {
    if(++this->auth_attempts < MAX_AUTH_ATTEMPTS) {
        this->sock.Send(
            Packet(kLoginResponse, {std::string(1, '\0'), HTONUS(error_code)})
        );
        return true;
    } else {
        return this->Close(
            Packet(kLoginResponse, {std::string(1, '\0'), HTONUS(0x100)})
        );
    }
}

bool sosc::MasterClient::ProcessRegistration(Packet &pck) {
    if(!pck.Check(3, PCK_ANY, PCK_ANY, PCK_ANY))
        return false;
    pck.TrimRegions();

    db::Query* query = this->queries->at(QRY_USER_NAME_REG_CHECK);
    query->Reset();
    query->BindText(pck[0], 1);
    if(query->ScalarInt32() > 0)
        return RegistrationError(0x100);

    query = this->queries->at(QRY_USER_MAIL_REG_CHECK);
    query->Reset();
    query->BindText(pck[2], 1);
    if(query->ScalarInt32() > 0)
        return RegistrationError(0x101);

    query = this->queries->at(QRY_USER_REGISTER);
    query->Reset();
    query->BindText(pck[0], 1);
    query->BindText(cgc::bcrypt_hash(pck[1]), 2);
    query->BindText(pck[2], 3);
    query->NonQuery();

    this->sock.Send(Packet(kRegisterResponse, {"\1", 0x000}));
    return true;
}

bool sosc::MasterClient::RegistrationError(uint16_t error_code) {
    this->sock.Send(
        Packet(kRegisterResponse, {std::string(1, '\0'), HTONUS(error_code)})
    );
    return true;
}

bool sosc::MasterClient::ListServers(Packet &pck) {

    return true;
}

bool sosc::MasterClient::Close() {
    this->sock.Close();
    return false;
}

bool sosc::MasterClient::Close(const Packet& message) {
    this->sock.Send(message);
    return this->Close();
}