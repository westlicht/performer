#pragma once

#include "TopPage.h"
#include "MainPage.h"
#include "TrackPage.h"
#include "MutePage.h"
#include "ValuePage.h"

#include "PageContext.h"

#include "model/Model.h"

#include "engine/Engine.h"


struct Pages {
    TopPage topPage;
    MainPage mainPage;
    TrackPage trackPage;
    MutePage mutePage;
    ValuePage valuePage;

    Pages(PageManager &pageManager, PageContext &context) :
        topPage(pageManager, context),
        mainPage(pageManager, context),
        trackPage(pageManager, context),
        mutePage(pageManager, context),
        valuePage(pageManager, context)
    {}
};

