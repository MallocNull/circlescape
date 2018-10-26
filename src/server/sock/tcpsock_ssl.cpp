#include "tcpsock.hpp"

struct {
    SSL_CTX* server;
    std::mutex server_mtx;

    SSL_CTX* client;
    std::mutex client_mtx;
} _ssl_ctx;

static bool ssl_init() {
    static bool is_inited = false;
    if(is_inited) return true;

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    _ssl_ctx.server = SSL_CTX_new(SSLv23_server_method());
    if(!_ssl_ctx.server)
        return false;

    _ssl_ctx.client = SSL_CTX_new(SSLv23_client_method());
    if(!_ssl_ctx.client)
        return false;

    SSL_CTX_set_ecdh_auto(_ssl_ctx.server, 1);
    SSL_CTX_set_ecdh_auto(_ssl_ctx.client, 1);

    int success = 0;
    success |= SSL_CTX_use_certificate_file(
        _ssl_ctx.server,
        SOSC_RESC("ssl/cert.pem").c_str(),
        SSL_FILETYPE_PEM
    );
    success |= SSL_CTX_use_PrivateKey_file(
        _ssl_ctx.server,
        SOSC_RESC("ssl/key.pem").c_str(),
        SSL_FILETYPE_PEM
    );
    if(success <= 0) {
        SSL_CTX_free(_ssl_ctx.client);
        SSL_CTX_free(_ssl_ctx.server);
        return false;
    }

    is_inited = true;
    return true;
}