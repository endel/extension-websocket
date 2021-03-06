#include <dmsdk/dlib/socket.h>
#include <dmsdk/dlib/sslsocket.h>
#include "websocket.h"

namespace dmWebsocket
{

dmSocket::Result WaitForSocket(WebsocketConnection* conn, dmSocket::SelectorKind kind, int timeout)
{
    dmSocket::Selector selector;
    dmSocket::SelectorZero(&selector);
    dmSocket::SelectorSet(&selector, kind, conn->m_Socket);
    return dmSocket::Select(&selector, timeout);
}

dmSocket::Result Send(WebsocketConnection* conn, const char* buffer, int length, int* out_sent_bytes)
{
    int total_sent_bytes = 0;
    int sent_bytes = 0;

    while (total_sent_bytes < length) {
        dmSocket::Result r;

        if (conn->m_SSLSocket)
            r = dmSSLSocket::Send(conn->m_SSLSocket, buffer + total_sent_bytes, length - total_sent_bytes, &sent_bytes);
        else
            r = dmSocket::Send(conn->m_Socket, buffer + total_sent_bytes, length - total_sent_bytes, &sent_bytes);

        if( r == dmSocket::RESULT_WOULDBLOCK )
        {
            r = dmSocket::RESULT_TRY_AGAIN;
        }

        if (r == dmSocket::RESULT_TRY_AGAIN)
            continue;

        if (r != dmSocket::RESULT_OK) {
            return r;
        }

        total_sent_bytes += sent_bytes;
    }
    if (out_sent_bytes)
        *out_sent_bytes = total_sent_bytes;
    return dmSocket::RESULT_OK;
}

dmSocket::Result Receive(WebsocketConnection* conn, void* buffer, int length, int* received_bytes)
{
    if (conn->m_SSLSocket)
        return dmSSLSocket::Receive(conn->m_SSLSocket, buffer, length, received_bytes);
    else
        return dmSocket::Receive(conn->m_Socket, buffer, length, received_bytes);
}

} // namespace