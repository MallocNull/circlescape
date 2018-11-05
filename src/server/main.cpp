#include <iostream>
#include <string>
#include <ctime>
#include <thread>

#include "utils/string.hpp"
#include "db/database.hpp"
#include "hosts/master.hpp"
#include "hosts/slave.hpp"

static struct _slave_ctx {
    sosc::ScapeServer server;
    sosc::SlaveClientPool pool;
};

static struct {
    struct {
        sosc::IntraServer server;
        sosc::MasterIntraPool pool;
    } master_intra;

    struct {
        sosc::ScapeServer server;
        sosc::MasterIntraPool pool;
    } master_client;

    _slave_ctx* slaves;
    bool running;
} _ctx;

bool master_intra(uint16_t port, const sosc::poolinfo_t& info);
bool master_client(uint16_t port, const sosc::poolinfo_t& info);
bool slave(uint16_t port, const sosc::poolinfo_t& info);

int main(int argc, char **argv) {
    using namespace sosc;
    if(argc < 2)
        return -1;

    _ctx.running = true;
    std::vector<std::thread*> threads;

    if(argv[1][0] == 'm') {
        if(!db::init_databases(nullptr))
            return -1;

        threads.push_back(new std::thread([&] {
            master_client(8008, poolinfo_t());
        }));
        threads.push_back(new std::thread([&] {
            master_intra(1234, poolinfo_t());
        }));
    } else {
        threads.push_back(new std::thread([&] {
            slave(1234, poolinfo_t());
        }));
    }

    std::cout << "Server threads started. Type STOP to cancel." << std::endl;

    std::string input;
    while(true) {
        std::cin >> input;
        str::tolower(str::trim(&input));

        if(input == "stop")
            break;
    }

    _ctx.running = false;
    for(const auto& thread : threads) {
        thread->join();
        delete thread;
    }

    return 0;
}

bool master_intra(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;
    
    IntraServer server;
    IntraClient client;
    if(!server.Listen(port))
        return false;
    
    MasterIntraPool pool;
    pool.Configure(info);
    pool.Start();

    auto listenThread = std::thread([&] {
        while (server.Accept(&client))
            pool.AddClient(new MasterIntra(client));
    });

    while(_ctx.running);

    server.Close();
    listenThread.join();
    pool.Stop();

    return true;
}

bool master_client(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;

    ScapeServer server;
    ScapeConnection client;
    if(!server.Listen(port, true))
        return false;

    MasterClientPool pool;
    pool.Configure(info);
    pool.Start();

    auto listenThread = std::thread([&] {
        while(server.Accept(&client))
            pool.AddClient(new MasterClient(client));
    });

    while(_ctx.running);

    server.Close();
    listenThread.join();
    pool.Stop();

    return true;
}

bool slave(uint16_t port, const sosc::poolinfo_t& info) {
    using namespace sosc;

    ScapeServer server;
    ScapeConnection client;
    if(!server.Listen(port))
        return false;

    SlaveClientPool pool;
    pool.Configure(info);
    pool.Start();

    auto listenThread = std::thread([&] {
        while (server.Accept(&client))
            pool.AddClient(new SlaveClient(client));
    });

    while(_ctx.running);

    server.Close();
    listenThread.join();
    pool.Stop();

    return true;
}
