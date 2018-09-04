#pragma once

#include "ListPage.h"

#include "ui/model/NoteTrackListModel.h"
#include "ui/model/CurveTrackListModel.h"
#include "ui/model/MidiCvTrackListModel.h"

class TrackPage : public ListPage {
public:
    TrackPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void setTrack(Track &track);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initTrackSetup();
    void copyTrackSetup();
    void pasteTrackSetup();
    void initRoute();

    RoutableListModel *_listModel;
    NoteTrackListModel _noteTrackListModel;
    CurveTrackListModel _curveTrackListModel;
    MidiCvTrackListModel _midiCvTrackListModel;
};
