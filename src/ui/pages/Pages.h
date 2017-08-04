#pragma once

#include "TopPage.h"
#include "MainPage.h"
#include "MutePage.h"
#include "ValuePage.h"

#include "model/Model.h"

#include "engine/Engine.h"

struct Pages {
    TopPage topPage;
    MainPage mainPage;
    MutePage mutePage;
    ValuePage valuePage;

    Pages(PageManager &pageManager, Model &model, Engine &engine) :
        topPage(pageManager, model, engine),
        mainPage(pageManager, model, engine),
        mutePage(pageManager, model, engine),
        valuePage(pageManager, model, engine)
    {}
};

