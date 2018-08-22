#include "RoutingPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Prev    = 0,
    Next    = 1,
    Init    = 2,
    Learn   = 3,
    Commit  = 4,
};

RoutingPage::RoutingPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _routeListModel),
    _routeListModel(_editRoute)
{
    showRoute(0);
}

void RoutingPage::show(int routeIndex, const Routing::Route *initialValue) {
    showRoute(routeIndex, initialValue);
    ListPage::show();
}

void RoutingPage::enter() {
    ListPage::enter();
}

void RoutingPage::exit() {
    _engine.midiLearn().stop();

    ListPage::exit();
}

void RoutingPage::draw(Canvas &canvas) {
    bool showCommit = *_route != _editRoute;
    bool showLearn = _editRoute.param() != Routing::Param::None;
    bool highlightLearn = showLearn && _engine.midiLearn().isActive();
    const char *functionNames[] = { "PREV", "NEXT", "INIT", showLearn ? "LEARN" : nullptr, showCommit ? "COMMIT" : nullptr };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ROUTING");
    WindowPainter::drawActiveFunction(canvas, FixedStringBuilder<16>("ROUTE %d", _routeIndex + 1));
    WindowPainter::drawFooter(canvas, functionNames, keyState(), highlightLearn ? int(Function::Learn) : -1);

    ListPage::draw(canvas);
}

void RoutingPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (edit() && selectedRow() == int(RouteListModel::Item::Tracks) && key.isTrack()) {
        _editRoute.toggleTrack(key.track());
        event.consume();
        return;
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Prev:
            selectRoute(_routeIndex - 1);
            break;
        case Function::Next:
            selectRoute(_routeIndex + 1);
            break;
        case Function::Init:
            _engine.midiLearn().stop();
            _editRoute.clear();
            setSelectedRow(0);
            setEdit(false);
            break;
        case Function::Learn:
            if (_editRoute.param() != Routing::Param::None) {
                _engine.midiLearn().start([this] (const MidiLearn::Result &result) {
                    // TODO this might be unsafe as callback is called from engine thread
                    assignMidiLearn(result);
                    _engine.midiLearn().stop();
                });
            }
            break;
        case Function::Commit:
            _engine.midiLearn().stop();
            *_route = _editRoute;
            showMessage("ROUTE CHANGED");
            break;
        }
        event.consume();
    }

    ListPage::keyPress(event);
}

void RoutingPage::encoder(EncoderEvent &event) {
    if (!edit() && keyState()[Key::Shift]) {
        selectRoute(_routeIndex + event.value());
        event.consume();
        return;
    }

    ListPage::encoder(event);
}

void RoutingPage::showRoute(int routeIndex, const Routing::Route *initialValue) {
    _route = &_project.routing().route(routeIndex);
    _routeIndex = routeIndex;
    _editRoute = *(initialValue ? initialValue : _route);

    setSelectedRow(0);
    setEdit(false);
}

void RoutingPage::selectRoute(int routeIndex) {
    routeIndex = clamp(routeIndex, 0, CONFIG_ROUTE_COUNT - 1);
    if (routeIndex != _routeIndex) {
        _engine.midiLearn().stop();
        showRoute(routeIndex);
    }
}

void RoutingPage::assignMidiLearn(const MidiLearn::Result &result) {
    auto &midiSource = _editRoute.midiSource();

    _editRoute.setSource(Routing::Source::Midi);

    midiSource.source().setPort(Types::MidiPort(result.port));
    midiSource.source().setChannel(result.channel);

    switch (result.event) {
    case MidiLearn::Event::ControlAbsolute:
        midiSource.setEvent(Routing::MidiSource::Event::ControlAbsolute);
        midiSource.setControlNumber(result.controlNumber);
        break;
    case MidiLearn::Event::ControlRelative:
        midiSource.setEvent(Routing::MidiSource::Event::ControlRelative);
        midiSource.setControlNumber(result.controlNumber);
        break;
    case MidiLearn::Event::PitchBend:
        midiSource.setEvent(Routing::MidiSource::Event::PitchBend);
        break;
    case MidiLearn::Event::Note:
        midiSource.setEvent(Routing::MidiSource::Event::NoteMomentary);
        midiSource.setNote(result.note);
        break;
    case MidiLearn::Event::Last:
        break;
    }

    setSelectedRow(int(RouteListModel::MidiSource));
    setTopRow(int(RouteListModel::MidiSource));
    setEdit(false);
}
