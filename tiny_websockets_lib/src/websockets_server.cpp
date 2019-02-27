#include <tiny_websockets/server.hpp>
#include <tiny_websockets/internals/wscrypto/crypto.hpp>
#include <map>

namespace websockets {
    WebsocketsServer::WebsocketsServer(network::TcpServer* server) : _server(server) {}

    bool WebsocketsServer::available() {
        return this->_server->available();
    }

    void WebsocketsServer::listen(uint16_t port) {
        this->_server->listen(port);
    }

    bool WebsocketsServer::poll() {
        return this->_server->poll();
    }

    struct ParsedHandshakeParams {
        WSString head;
        std::map<WSString, WSString> headers;
    };

    ParsedHandshakeParams recvHandshakeRequest(network::TcpClient& client) {
        ParsedHandshakeParams result;

        result.head = client.readLine();
        // std::cout << "Head: " << result.head << std::endl;

        WSString line = client.readLine();
        do {
            // std::cout << "Got Line: " << line << std::endl;
            WSString key = "", value = "";
            size_t idx = 0;

            // read key
            while(idx < line.size() && line[idx] != ':') {
                key += line[idx];
                idx++;
            }

            // skip key and whitespace
            idx++;
            while(idx < line.size() && line[idx] == ' ' || line[idx] == '\t') idx++;

            // read value (until \r\n)
            while(idx < line.size() && line[idx] != '\r') {
                value += line[idx];
                idx++;
            }

            // store header
            result.headers[key] = value;

            line = client.readLine();
        } while(client.available() && line != "\r\n");

        return result;
    }

    WebsocketsClient WebsocketsServer::accept() {
        auto tcpClient = _server->accept();
        if(tcpClient->available() == false) return {};
        
        // std::cout << "Got Connection" << std::endl;

        auto params = recvHandshakeRequest(*tcpClient);
        // std::cout << "Parsed Handshake" << std::endl;
        
        if(params.headers["Connection"] != "Upgrade") return {}; 
        if(params.headers["Upgrade"] != "websocket") return {}; 
        if(params.headers["Sec-WebSocket-Version"] != "13") return {}; 
        if(params.headers["Sec-WebSocket-Key"] == "") return {};

        // std::cout << "After Handshake Validation" << std::endl;
        
        auto serverAccept = crypto::websocketsHandshakeEncodeKey(
            params.headers["Sec-WebSocket-Key"]
        );

        // std::cout << "accept: " << serverAccept << std::endl;
        // std::cout << "Head: " << params.head << std::endl;
        // for(auto p : params.headers) {
        //     std::cout << "<" << p.first << ">\t<" << p.second << ">" << std::endl;
        // }

        tcpClient->send("HTTP/1.1 101 Switching Protocols\r\n");
        tcpClient->send("Connection: Upgrade\r\n");
        tcpClient->send("Upgrade: websocket\r\n");
        tcpClient->send("Sec-WebSocket-Version: 13\r\n");
        tcpClient->send("Sec-WebSocket-Accept: " + serverAccept + "\r\n");
        tcpClient->send("\r\n");
        
        return WebsocketsClient(tcpClient);
    }

    WebsocketsServer::~WebsocketsServer() {
        this->_server->close();
    }

} //websockets