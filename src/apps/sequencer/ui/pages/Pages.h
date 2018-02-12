#pragma once

#include "Config.h"

#include "BusyPage.h"
#include "ClockSetupPage.h"
#include "ConfirmationPage.h"
#include "CurveSequencePage.h"
#include "MainPage.h"
#include "MutePage.h"
#include "NoteSequencePage.h"
#include "PatternPage.h"
#include "PerformerPage.h"
#include "ProjectPage.h"
#include "SequenceSetupPage.h"
#include "TextInputPage.h"
#include "TopPage.h"
#include "TrackPage.h"
#include "TrackSetupPage.h"
#include "ValuePage.h"

#ifdef CONFIG_ENABLE_ASTEROIDS
#include "AsteroidsPage.h"
#endif

struct Pages {
    TopPage top;
    MainPage main;
    TrackSetupPage trackSetup;
    TrackPage track;
    NoteSequencePage noteSequence;
    CurveSequencePage curveSequence;
    SequenceSetupPage sequenceSetup;
    MutePage mute;
    ValuePage value;
    ClockSetupPage clockSetup;
    PatternPage pattern;
    ProjectPage project;
    PerformerPage performer;

    // helper pages
    TextInputPage textInput;
    ConfirmationPage confirmation;
    BusyPage busy;

#ifdef CONFIG_ENABLE_ASTEROIDS
    AsteroidsPage asteroids;
#endif

    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        main(manager, context),
        trackSetup(manager, context),
        track(manager, context),
        noteSequence(manager, context),
        curveSequence(manager, context),
        sequenceSetup(manager, context),
        mute(manager, context),
        value(manager, context),
        clockSetup(manager, context),
        pattern(manager, context),
        project(manager, context),
        performer(manager, context),
        textInput(manager, context),
        confirmation(manager, context),
        busy(manager, context)
#ifdef CONFIG_ENABLE_ASTEROIDS
        ,asteroids(manager, context)
#endif
    {}
};

