#pragma once

#include "common.h"
#include "websockets/data_frame.h"

enum MessageType {
    // Data opcdoes
    Text = 0x1,
    Binary = 0x2,

    // Control opcodes
    Close = 0x8,
    Ping = 0x9,
    Pong = 0xA
};

// The class the user will interact with as a message
// This message can be partial (so practically this is a Frame and not a message)
struct WebsocketsMessage {
    WebsocketsMessage(MessageType type, String data, bool partial = false) : _type(type), _data(data), _isPartial(partial) {}
    static WebsocketsMessage CreateBinary(String data, bool partial = false) {
        return WebsocketsMessage(MessageType::Binary, data, partial);
    }
    static WebsocketsMessage CreateText(String data, bool partial = false) {
        return WebsocketsMessage(MessageType::Text, data, partial);
    }

    static WebsocketsMessage CreateFromFrame(WebsocketsFrame frame) {
        return WebsocketsMessage(
            (MessageType) frame.opcode,
            frame.payload,
            !frame.fin
        );
    }
    
    bool isText() { return this->_type == MessageType::Text; }
    bool isBinary() { return this->_type == MessageType::Binary; }

    MessageType type() { return this->_type; }
    String data() { return this->_data; }

private:
    MessageType _type;
    String _data;
    bool _isPartial;
};
