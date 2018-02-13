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
    // main pages
    ProjectPage project;
    PatternPage pattern;
    TrackSetupPage trackSetup;
    NoteSequencePage noteSequence;
    CurveSequencePage curveSequence;
    SequenceSetupPage sequenceSetup;
    PerformerPage performer;
    // modal pages
    ClockSetupPage clockSetup;
    // obsolete pages
    MainPage main;
    TrackPage track;
    MutePage mute;
    // helper pages
    ValuePage value;
    TextInputPage textInput;
    ConfirmationPage confirmation;
    BusyPage busy;

#ifdef CONFIG_ENABLE_ASTEROIDS
    AsteroidsPage asteroids;
#endif

    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        // main pages
        project(manager, context),
        pattern(manager, context),
        trackSetup(manager, context),
        noteSequence(manager, context),
        curveSequence(manager, context),
        sequenceSetup(manager, context),
        performer(manager, context),
        // modal pages
        clockSetup(manager, context),
        // obsolete pages
        main(manager, context),
        track(manager, context),
        mute(manager, context),
        // helper pages
        value(manager, context),
        textInput(manager, context),
        confirmation(manager, context),
        busy(manager, context)
#ifdef CONFIG_ENABLE_ASTEROIDS
        ,asteroids(manager, context)
#endif
    {}
};

