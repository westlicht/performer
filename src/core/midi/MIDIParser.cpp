#include "MIDIParser.h"

#include "core/Debug.h"

static bool isStatusByte(uint8_t byte) {
    return byte & 0x80;
}

bool MIDIParser::feed(uint8_t data) {
    // DBG("%02x", data);
    if (isStatusByte(data)) {
        // DBG("status");
        if (MIDIMessage::isRealTimeMessage(data)) {
            // emit real-time message
            _message = MIDIMessage(data);
            return true;
        } else if (MIDIMessage::isSystemMessage(data)) {
            switch (MIDIMessage::systemMessage(data)) {
            case MIDIMessage::SystemExclusive:
                // start system exclusive receive
                _recvSystemExclusive = true;
                break;
            case MIDIMessage::TimeCode:
            case MIDIMessage::SongPosition:
            case MIDIMessage::SongSelect:
                _recvSystemExclusive = false;
                // update running status
                _status = data;
                // receive data
                _dataIndex = 0;
                _dataLength = MIDIMessage::systemMessageLength(MIDIMessage::systemMessage(data));
                break;
            case MIDIMessage::TuneRequest:
                _recvSystemExclusive = false;
                // emit tune-request message
                _message = MIDIMessage(data);
                return true;
            case MIDIMessage::EndOfExclusive:
                // end system exclusive receive
                _recvSystemExclusive = false;
                // TODO emit message
                break;
            }
        } else if (MIDIMessage::isChannelMessage(data)) {
            _recvSystemExclusive = false;
            // update running status
            _status = data;
            // receive data
            _dataIndex = 0;
            _dataLength = MIDIMessage::channelMessageLength(MIDIMessage::channelMessage(data));
        } else {
            // Unknown status byte -> ignore
        }
    } else {
        // DBG("data %d data length %d", _dataIndex, _dataLength);
        if (_recvSystemExclusive) {
            // TODO add to buffer
        } else if (_dataLength > 0) {
            _data[_dataIndex++] = data;
            if (_dataIndex == _dataLength) {
                _dataIndex = 0;
                // emit message
                _message = MIDIMessage(_status, _data[0], _dataLength > 1 ? _data[1] : 0);
                return true;
            }
        }
    }
    return false;
}
