#include "MidiParser.h"

#include "core/Debug.h"

static bool isStatusByte(uint8_t byte) {
    return byte & 0x80;
}

bool MidiParser::feed(uint8_t data) {
    // DBG("%02x", data);
    if (isStatusByte(data)) {
        // DBG("status");
        if (MidiMessage::isRealTimeMessage(data)) {
            // emit real-time message
            _message = MidiMessage(data);
            return true;
        } else if (MidiMessage::isSystemMessage(data)) {
            switch (MidiMessage::systemMessage(data)) {
            case MidiMessage::SystemExclusive:
                // start system exclusive receive
                _recvSystemExclusive = true;
                break;
            case MidiMessage::TimeCode:
            case MidiMessage::SongPosition:
            case MidiMessage::SongSelect:
                _recvSystemExclusive = false;
                // update running status
                _status = data;
                // receive data
                _dataIndex = 0;
                _dataLength = MidiMessage::systemMessageLength(MidiMessage::systemMessage(data));
                break;
            case MidiMessage::TuneRequest:
                _recvSystemExclusive = false;
                // emit tune-request message
                _message = MidiMessage(data);
                return true;
            case MidiMessage::EndOfExclusive:
                // end system exclusive receive
                _recvSystemExclusive = false;
                // TODO emit message
                break;
            }
        } else if (MidiMessage::isChannelMessage(data)) {
            _recvSystemExclusive = false;
            // update running status
            _status = data;
            // receive data
            _dataIndex = 0;
            _dataLength = MidiMessage::channelMessageLength(MidiMessage::channelMessage(data));
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
                _message = (_dataLength == 1) ? MidiMessage(_status, _data[0]) : MidiMessage(_status, _data[0], _data[1]);
                return true;
            }
        }
    }
    return false;
}
