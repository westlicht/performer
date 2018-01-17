#pragma once

#include "TopPage.h"
#include "MainPage.h"
#include "TrackPage.h"
#include "SequenceSetupPage.h"
#include "MutePage.h"
#include "ValuePage.h"

struct Pages {
    TopPage top;
    MainPage main;
    TrackPage track;
    SequenceSetupPage sequenceSetup;
    MutePage mute;
    ValuePage value;

    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        main(manager, context),
        track(manager, context),
        sequenceSetup(manager, context),
        mute(manager, context),
        value(manager, context),
    {}
};

