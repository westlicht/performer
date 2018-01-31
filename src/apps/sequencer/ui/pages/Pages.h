#pragma once

#include "Config.h"

#include "TopPage.h"
#include "MainPage.h"
#include "TrackPage.h"
#include "SequenceSetupPage.h"
#include "MutePage.h"
#include "ValuePage.h"
#include "ClockSetupPage.h"

#ifdef CONFIG_ENABLE_ASTEROIDS
#include "AsteroidsPage.h"
#endif

struct Pages {
    TopPage top;
    MainPage main;
    TrackPage track;
    SequenceSetupPage sequenceSetup;
    MutePage mute;
    ValuePage value;
    ClockSetupPage clockSetup;

#ifdef CONFIG_ENABLE_ASTEROIDS
    AsteroidsPage asteroids;
#endif

    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        main(manager, context),
        track(manager, context),
        sequenceSetup(manager, context),
        mute(manager, context),
        value(manager, context),
        clockSetup(manager, context)
#ifdef CONFIG_ENABLE_ASTEROIDS
        ,asteroids(manager, context)
#endif
    {}
};

