#pragma once

#include "TopPage.h"
#include "MainPage.h"
#include "TrackPage.h"
#include "SequenceSetupPage.h"
#include "MutePage.h"
#include "ValuePage.h"

struct Pages {
    TopPage topPage;
    MainPage mainPage;
    TrackPage trackPage;
    SequenceSetupPage sequenceSetupPage;
    MutePage mutePage;
    ValuePage valuePage;

    Pages(PageManager &manager, PageContext &context) :
        topPage(manager, context),
        mainPage(manager, context),
        trackPage(manager, context),
        sequenceSetupPage(manager, context),
        mutePage(manager, context),
        valuePage(manager, context)
    {}
};

