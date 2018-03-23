#pragma once

#include "Config.h"

#include "BpmPage.h"
#include "BusyPage.h"
#include "ClockSetupPage.h"
#include "ConfirmationPage.h"
#include "ContextMenuPage.h"
#include "CurveSequencePage.h"
#include "CurveSequenceSetupPage.h"
#include "MonitorPage.h"
#include "NoteSequencePage.h"
#include "NoteSequenceSetupPage.h"
#include "PatternPage.h"
#include "PerformerPage.h"
#include "ProjectPage.h"
#include "ProjectSelectPage.h"
#include "TextInputPage.h"
#include "TopPage.h"
#include "LayoutPage.h"
#include "TrackPage.h"

#include "EuclideanPage.h"

#ifdef CONFIG_ENABLE_ASTEROIDS
#include "AsteroidsPage.h"
#endif

struct Pages {
    TopPage top;
    // main pages
    ProjectPage project;
    LayoutPage layout;
    TrackPage track;
    NoteSequencePage noteSequence;
    CurveSequencePage curveSequence;
    NoteSequenceSetupPage noteSequenceSetup;
    CurveSequenceSetupPage curveSequenceSetup;
    PatternPage pattern;
    PerformerPage performer;
    // modal pages
    BpmPage bpm;
    ClockSetupPage clockSetup;
    MonitorPage monitor;
    ProjectSelectPage projectSelect;
    ContextMenuPage contextMenu;
    // helper pages
    TextInputPage textInput;
    ConfirmationPage confirmation;
    BusyPage busy;
    // misc
    EuclideanPage euclidean;

#ifdef CONFIG_ENABLE_ASTEROIDS
    AsteroidsPage asteroids;
#endif

    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        // main pages
        project(manager, context),
        layout(manager, context),
        track(manager, context),
        noteSequence(manager, context),
        curveSequence(manager, context),
        noteSequenceSetup(manager, context),
        curveSequenceSetup(manager, context),
        pattern(manager, context),
        performer(manager, context),
        // modal pages
        bpm(manager, context),
        clockSetup(manager, context),
        monitor(manager, context),
        projectSelect(manager, context),
        contextMenu(manager, context),
        // helper pages
        textInput(manager, context),
        confirmation(manager, context),
        busy(manager, context),
        // misc
        euclidean(manager, context)
#ifdef CONFIG_ENABLE_ASTEROIDS
        ,asteroids(manager, context)
#endif
    {}
};

