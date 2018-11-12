#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <sock/pool.hpp>

#include "db/database.hpp"
#include "hosts/master.hpp"
#include "hosts/slave.hpp"
#include "utils/ini.hpp"
#include "utils/string.hpp"

template<class T, class U>
struct _server_ctx {
    std::thread thread;
    T server;
    U pool;
};

typedef _server_ctx<sosc::IntraServer, sosc::MasterIntraPool>
    master_intra_ctx;
typedef _server_ctx<sosc::ScapeServer, sosc::MasterClientPool>
    master_client_ctx;
typedef _server_ctx<sosc::ScapeServer, sosc::SlaveClientPool>
    slave_ctx;

static struct {
    sosc::poolinfo_t default_info;

    master_intra_ctx* master_intra = nullptr;
    master_client_ctx* master_client = nullptr;
    slave_ctx* slaves = nullptr;
    int slave_count = 0;
} _ctx;

bool master_intra_start(uint16_t port, const sosc::poolinfo_t& info);
bool master_client_start(uint16_t port, const sosc::poolinfo_t& info);
bool slave_start(uint16_t port, const sosc::poolinfo_t& info, slave_ctx* ctx);

void master_intra_stop();
void master_client_stop();
void slave_stop(slave_ctx* ctx);

void configure_poolinfo(sosc::poolinfo_t* info,
    const sosc::ini::File::SectionList::Section& section) 
{
    *info = sosc::poolinfo_t(_ctx.default_info);

    if(section.HasKey("inital count"))
        info->initial_count = (int)section["initial count"];
    if(section.HasKey("inital size"))
        info->initial_size = (int)section["initial size"];
    if(section.HasKey("size growth"))
        info->size_growth = (int)section["size growth"];

    if(section.HasKey("max size"))
        info->max_size = (int)section["max size"];
    if(section.HasKey("max count"))
        info->max_count = (int)section["max count"];
    if(section.HasKey("max total"))
        info->max_total = (int)section["max total"];
    if(section.HasKey("tolerance"))
        info->tolerance = (int)section["tolerance"];
}

int main(int argc, char **argv) {
    using namespace sosc;
    if(argc < 2)
        return -1;

    ini::File* config;
    try {
        config = ini::File::Open(SOSC_RESC("config.ini"), {
            ini::Rule("general", true, false, {
                ini::Field("run master", ini::Field::BOOL),
                ini::Field("master host", ini::Field::STRING),
                ini::Field("master port", ini::Field::UINT32),
            }),
            ini::Rule("defaults", true, false, {
                ini::Field("initial count", ini::Field::UINT32),
                ini::Field("initial size", ini::Field::UINT32),
                ini::Field("size growth", ini::Field::UINT32),

                ini::Field("max size", ini::Field::INT32),
                ini::Field("max count", ini::Field::INT32),
                ini::Field("max total", ini::Field::INT32),
                ini::Field("tolerance", ini::Field::INT32),
            }),
            ini::Rule("master to client", false, false, {
                ini::Field("port", ini::Field::UINT32),
            }),
            ini::Rule("master to slave", false, false, {
                ini::Field("port", ini::Field::UINT32),
            }),
            ini::Rule("slave", false, true, {
                ini::Field("port", ini::Field::UINT32),
                ini::Field("name", ini::Field::STRING),
            })
        });
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    poolinfo_t info;
    configure_poolinfo(&_ctx.default_info, (*config)["defaults"][0]);

    if((*config)["master"]["run master"]) {
        if(!config->HasSection("master to client") ||
           !config->HasSection("master to slave"))
        {
            std::cout << "'MASTER TO CLIENT' and 'MASTER TO SLAVE' sections "
                      << "must exist if 'RUN MASTER' is true." << std::endl;
            return -1;
        }

        if(!db::init_databases(nullptr)) {
            std::cout << "Could not initialized master database.";
            return -1;
        }

        configure_poolinfo(&info, (*config)["master to slave"][0]);
        _ctx.master_intra = new master_intra_ctx;
        master_intra_start(
            (uint16_t)(*config)["master"]["intra port"],
            poolinfo_t()
        );

        configure_poolinfo(&info, (*config)["master to client"][0]);
        _ctx.master_client = new master_client_ctx;
        master_client_start(
            (uint16_t)(*config)["master"]["client port"],
            poolinfo_t()
        );
    }

    if(config->HasSection("slave")) {
        _ctx.slave_count = (*config)["slave"].SectionCount();
        _ctx.slaves = new slave_ctx[_ctx.slave_count];

        for(int i = 0; i < _ctx.slave_count; ++i) {
            configure_poolinfo(&info, (*config)["slave"][i]);
            slave_start(
                (uint16_t)(*config)["slave"][i]["port"],
                poolinfo_t(),
                _ctx.slaves + i
            );
        }
    }

    std::cout << "Server threads started. Type STOP to cancel." << std::endl;

    std::string input;
    while(true) {
        std::cin >> input;
        str::tolower(str::trim(&input));

        if(input == "stop")
            break;
    }

    master_client_stop();
    master_intra_stop();
    for(int i = 0; i < _ctx.slave_count; ++i)
        slave_stop(_ctx.slaves + i);

    return 0;
}

bool master_intra_start(uint16_t port, const sosc::poolinfo_t& info) {
    if(!_ctx.master_intra->server.Listen(port))
        return false;

    _ctx.master_intra->pool.Configure(info);
    _ctx.master_intra->pool.Start();

    _ctx.master_intra->thread = std::thread([&] {
        sosc::IntraClient client;
        while (_ctx.master_intra->server.Accept(&client))
            _ctx.master_intra->pool.AddClient(new sosc::MasterIntra(client));
    });

    return true;
}

bool master_client_start(uint16_t port, const sosc::poolinfo_t& info) {
    if(!_ctx.master_client->server.Listen(port, true))
        return false;

    _ctx.master_client->pool.Configure(info);
    _ctx.master_client->pool.Start();

    _ctx.master_client->thread = std::thread([&] {
        sosc::ScapeConnection client;
        while(_ctx.master_client->server.Accept(&client))
            _ctx.master_client->pool.AddClient(new sosc::MasterClient(client));
    });

    return true;
}

bool slave_start(uint16_t port, const sosc::poolinfo_t& info, slave_ctx* ctx) {
    if(!ctx->server.Listen(port))
        return false;

    ctx->pool.Configure(info);
    ctx->pool.Start();

    ctx->thread = std::thread([&] {
        sosc::ScapeConnection client;
        while (ctx->server.Accept(&client))
            ctx->pool.AddClient(new sosc::SlaveClient(client));
    });

    return true;
}

void master_intra_stop() {
    if(_ctx.master_intra == nullptr)
        return;

    _ctx.master_intra->server.Close();
    _ctx.master_intra->thread.join();
    _ctx.master_intra->pool.Stop();

    delete _ctx.master_intra;
    _ctx.master_intra = nullptr;
}

void master_client_stop() {
    if(_ctx.master_client == nullptr)
        return;

    _ctx.master_client->server.Close();
    _ctx.master_client->thread.join();
    _ctx.master_client->pool.Stop();

    delete _ctx.master_client;
    _ctx.master_client = nullptr;
}

void slave_stop(slave_ctx* ctx) {
    if(ctx == nullptr)
        return;

    ctx->server.Close();
    ctx->thread.join();
    ctx->pool.Stop();

    delete ctx;
}