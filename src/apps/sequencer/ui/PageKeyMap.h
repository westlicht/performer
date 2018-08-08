#pragma once

#include "Key.h"

namespace PageKeyMap {

    enum Page {
        Clock           = Key::Tempo,
        Pattern         = Key::Pattern,
        Performer       = Key::Performer,

        Project         = Key::Track0,
        Layout          = Key::Track1,
        Track           = Key::Track2,
        Sequence        = Key::Track3,
        SequenceEdit    = Key::Track4,
        Song            = Key::Track6,
        Routing         = Key::Track7,

        UserScale       = Key::Step0,
        Monitor         = Key::Step6,
        System          = Key::Step7,
    };

    static bool isPageKey(int code) {
        switch (code) {
        case Clock:
        case Pattern:
        case Performer:

        case Project:
        case Layout:
        case Track:
        case Sequence:
        case SequenceEdit:
        case Song:
        case Routing:

        case UserScale:
        case Monitor:
        case System:
            return true;
        default:
            break;
        }

        return false;
    }

} // namespace PageKeyMap
