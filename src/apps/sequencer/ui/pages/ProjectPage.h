#pragma once

#include "ListPage.h"

#include "ui/model/ProjectListModel.h"

class ProjectPage : public ListPage {
public:
    ProjectPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initProject();
    void loadProject();
    void saveProject();
    void saveAsProject();
    void initRoute();

    void saveProjectToSlot(int slot);
    void loadProjectFromSlot(int slot);

    ProjectListModel _listModel;
};
