#include "RoutePage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Change  = 0,
    Learn   = 1,
    Back    = 4,
};

RoutePage::RoutePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _routeListModel),
    _routeListModel(_editRoute)
{
}

void RoutePage::show(int routeIndex) {
    show(routeIndex, _project.routing().route(routeIndex));
}

void RoutePage::show(int routeIndex, const Routing::Route &editRoute) {
    _route = &_project.routing().route(routeIndex);
    _routeIndex = routeIndex;
    _editRoute = editRoute;

    setSelectedRow(0);
    setEdit(false);

    ListPage::show();
}

void RoutePage::enter() {
    ListPage::enter();
}

void RoutePage::exit() {
    _engine.midiLearn().stop();

    ListPage::exit();
}

void RoutePage::draw(Canvas &canvas) {
    bool showChange = *_route != _editRoute;
    const char *functionNames[] = { showChange ? "CHANGE" : nullptr, "LEARN", nullptr, nullptr, "BACK" };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ROUTING");
    WindowPainter::drawActiveFunction(canvas, FixedStringBuilder<16>("ROUTE %d", _routeIndex + 1));
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void RoutePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Change:
            *_route = _editRoute;
            break;
        case Function::Learn:
            _engine.midiLearn().start([this] (const MidiLearn::Result &result) {
                assignMidiLearn(result);
                _engine.midiLearn().stop();
            });
            break;
        case Function::Back:
            close();
            break;
        }
        event.consume();
    }

    ListPage::keyPress(event);
}

void RoutePage::assignMidiLearn(const MidiLearn::Result &result) {
    auto &midiSource = _editRoute.midiSource();

    _editRoute.setSource(Routing::Source::Midi);

    midiSource.setPort(Types::MidiPort(result.port));
    midiSource.setChannel(result.channel);

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

    setSelectedRow(int(RouteListModel::MidiPort));
    setTopRow(int(RouteListModel::MidiPort));
    setEdit(false);
}
