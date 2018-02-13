#pragma once

#include "Config.h"

#include "BpmPage.h"
#include "BusyPage.h"
#include "ClockSetupPage.h"
#include "ConfirmationPage.h"
#include "CurveSequencePage.h"
#include "MutePage.h"
#include "NoteSequencePage.h"
#include "PatternPage.h"
#include "PerformerPage.h"
#include "ProjectPage.h"
#include "SequenceSetupPage.h"
#include "TextInputPage.h"
#include "TopPage.h"
#include "TrackSetupPage.h"

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
    BpmPage bpm;
    ClockSetupPage clockSetup;
    // obsolete pages
    MutePage mute;
    // helper pages
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
        bpm(manager, context),
        clockSetup(manager, context),
        // obsolete pages
        mute(manager, context),
        // helper pages
        textInput(manager, context),
        confirmation(manager, context),
        busy(manager, context)
#ifdef CONFIG_ENABLE_ASTEROIDS
        ,asteroids(manager, context)
#endif
    {}
};

