// 
// Copyright 2025 Andrew Haisley
// 
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see https://www.gnu.org/licenses/.
// 
#include <string.h>
#include <chrono>

#include "util.hpp"
#include "config.hpp"
#include "ssl_server_socket.hpp"
#include "run_state.hpp"

using namespace std;
using namespace chrono;
using namespace adns;

ssl_server_socket::ssl_server_socket(const socket_address &sa) : tcp_server_socket(sa)
{
    m_ctx = nullptr;
    m_ssl = nullptr;
}

string ssl_server_socket::ssl_error_string()
{
    char error[1024];

    ERR_error_string_n(ERR_get_error(), error, 1024);

    error[1023] = '\0';

    return string(error);
}

ssl_server_socket::~ssl_server_socket()
{
    if (m_ssl != nullptr)
    {
        SSL_free(m_ssl);
    }
    if (m_ctx != nullptr)
    {
        SSL_CTX_free(m_ctx);
    }
}

shared_ptr<tcp_server_socket> ssl_server_socket::accept()
{
    return shared_ptr<tcp_server_socket>(make_ssl_socket(tcp_server_socket::accept()));
}

ssl_server_socket::ssl_server_socket(tcp_server_socket *ts) : tcp_server_socket(ts->m_fd, ts->m_remote_address)
{
}


list<std::shared_ptr<tcp_server_socket>> ssl_server_socket::accept_many()
{
    list<shared_ptr<tcp_server_socket>> sockets;

    auto tsl = tcp_server_socket::accept_many();

    for (auto ts : tsl)
    {
        sockets.push_back(make_ssl_socket(ts));
    }

    return sockets;
}

int ssl_server_socket::alpn_cb(SSL *ssl,
             const unsigned char **out,
             unsigned char *outlen,
             const unsigned char *in,
             unsigned int inlen,
             void *arg)
{
    unsigned int i = 0;
    bool http_1_found = false;
    bool http_2_found = false;
    while (i < inlen) 
    {
        string s((char *)(&in[i + 1]), in[i]);
        if (s == "http/1.1")
        {
            http_1_found = true;
        }
        else if (s == "h2")
        {
            http_2_found = true;
        }
        i += in[i] + 1;
    }

    if (http_2_found)
    {
        out[0] = (unsigned char *)"h2";
        outlen[0] = 2;
        static_cast<ssl_server_socket *>(arg)->m_protocol = http_2_e;
        return SSL_TLSEXT_ERR_OK;
    }
    else if (http_1_found)
    {
        out[0] = (unsigned char *)"http/1.1";
        outlen[0] = 8;
        static_cast<ssl_server_socket *>(arg)->m_protocol = http_1_1_e;
        return SSL_TLSEXT_ERR_OK;
    }
    else
    {
        return SSL_TLSEXT_ERR_NOACK;
    }
}

shared_ptr<tcp_server_socket> ssl_server_socket::make_ssl_socket(const shared_ptr<tcp_server_socket> &ts)
{
    auto res = unique_ptr<ssl_server_socket>(new ssl_server_socket(ts.get()));

    // we're transferring the existing socket to a new instance so set the old one to -1
    // to prevent closure when the old instance is destructed
    ts->m_fd = -1;

    res->m_ctx = SSL_CTX_new(TLS_server_method());

    if (res->m_ctx == nullptr) 
    {
        THROW(ssl_accept_failed_exception, "SSL_CTX_new failed", ssl_error_string());
    }

    if (SSL_CTX_use_certificate_chain_file(res->m_ctx, config::ssl_server_chain_file().c_str()) <= 0) 
    {
        THROW(ssl_accept_failed_exception, "SSL_CTX_use_certificate_chain_file failed", ssl_error_string());
    }

    if (SSL_CTX_use_PrivateKey_file(res->m_ctx, config::ssl_server_key_file().c_str(), SSL_FILETYPE_PEM) <= 0) 
    {
        THROW(ssl_accept_failed_exception, "SSL_CTX_use_PrivateKey_file failed", ssl_error_string());
    }

    res->m_ssl = SSL_new(res->m_ctx);

    if (res->m_ssl == nullptr) 
    {
        THROW(ssl_accept_failed_exception, "SSL_new failed", ssl_error_string());
    }

    // no error codes are returned from this call
    SSL_CTX_set_alpn_select_cb(res->m_ctx, alpn_cb, res.get());
    
    if (SSL_set_fd(res->m_ssl, res->m_fd) != 1)
    {
        THROW(ssl_accept_failed_exception, "SSL_set_fd failed", ssl_error_string());
    }

    if (SSL_accept(res->m_ssl) <= 0) 
    {
        THROW(ssl_accept_failed_exception, "SSL_accept failed", ssl_error_string());
    }

    return shared_ptr<tcp_server_socket>(res.release());
}

buffer ssl_server_socket::read(int n, uint timeout_ms)
{
    if (m_ssl == nullptr)
    {
        THROW(ssl_socket_exception, "trying to read from an unconnected socket");
    }

    if (n == 0)
    {
        return buffer();
    }

    unique_ptr<octet[]> data(new octet[n]);
    int ret, total_read = 0;

    time_point<steady_clock> start_time = steady_clock::now();
    duration<int, milli> timeout(timeout_ms);
    
    while (total_read < n)
    {
        ret = SSL_read(m_ssl, data.get() + total_read, n - total_read);

        if (ret > 0)
        {
            total_read += ret;
        }
        else // ret <= 0
        {
            if (SSL_get_error(m_ssl, ret) != SSL_ERROR_WANT_READ)
            {
                THROW(ssl_socket_exception, "SSL_read failed", ssl_error_string(), ret);
            }
        }

        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(ssl_socket_exception, "shutdown");
        }

        time_point<steady_clock> time_now = steady_clock::now();

        if ((time_now - start_time) > timeout)
        {   
            THROW(ssl_socket_exception, "read timed out");
        }
    }

    return buffer(n, data.get());
}

void ssl_server_socket::write(const octet *data, uint n, uint timeout_ms)
{
    if (m_ssl == nullptr)
    {
        THROW(ssl_socket_exception, "trying to write to an unconnected socket");
    }

    if (n == 0)
    {
        return;
    }

    int ret;
    size_t written_so_far = 0;

    time_point<steady_clock> start_time = steady_clock::now();
    duration<int, milli> timeout(timeout_ms);

    while (written_so_far < n)
    {
        ret = SSL_write(m_ssl, data + written_so_far, n - written_so_far);

        if (ret > 0)
        {
            written_so_far += ret;
        }
        else // ret <= 0
        {
            if (SSL_get_error(m_ssl, ret) != SSL_ERROR_WANT_WRITE)
            {
                THROW(ssl_socket_exception, "SSL_write failed", ssl_error_string(), ret);
            }
        }

        if (run_state::o_state == run_state::shutdown_e)
        {
            THROW(ssl_socket_exception, "shutdown");
        }

        time_point<steady_clock> time_now = steady_clock::now();

        if ((time_now - start_time) > timeout)
        {   
            THROW(ssl_socket_exception, "read timed out");
        }
    }
}
