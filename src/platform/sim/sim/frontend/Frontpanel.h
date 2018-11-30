#pragma once

#include <cmath>

namespace sim {

namespace Frontpanel {

static double width = 172.5;
static double height = 128.27;

static double scale = 5.0;

static int windowWidth = std::ceil(width * scale);
static int windowHeight = std::ceil(height * scale);
static int controlHeight = 80;

enum class Widget {
    Jack,
    Led,
    CButton,
    RButton,
    Encoder,
    Lcd,
};

enum class Signal {
    Encoder,
    MidiInput,
    MidiOutput,
    DigitalInput,
    DigitalOutput,
    CvInput,
    GateOutput,
    CvOutput,
    Button,
    Led,
    Lcd,
};

struct Info {
    Widget widget;
    Signal signal;
    int index;

    double x;
    double y;
    double w;
    double h;
};

#define INFO(_kind_, _signal_, _index_, _x_, _y_, _w_, _h_) \
    { Widget::_kind_, Signal::_signal_, _index_, _x_, _y_, _w_, _h_ },

static const Info infos[] = {

//   kind       function        index   x           y           w       h

INFO(Lcd,       Lcd,            0,      44.725,     37.823,     82.0,   20.5    )
INFO(Encoder,   Encoder,        0,      31.75,      48.26,      10.0,   10.0    )

// jacks

INFO(Jack,      MidiInput,      0,      25.4,       18.415,     6.3,    6.3     )
INFO(Jack,      MidiOutput,     0,      38.1,       18.415,     6.3,    6.3     )
INFO(Jack,      DigitalInput,   0,      50.8,       18.415,     6.3,    6.3     )
INFO(Jack,      DigitalInput,   1,      63.5,       18.415,     6.3,    6.3     )
INFO(Jack,      DigitalOutput,  0,      76.2,       18.415,     6.3,    6.3     )
INFO(Jack,      DigitalOutput,  1,      88.9,       18.415,     6.3,    6.3     )
INFO(Jack,      CvInput,        0,      101.6,      18.415,     6.3,    6.3     )
INFO(Jack,      CvInput,        1,      114.3,      18.415,     6.3,    6.3     )
INFO(Jack,      CvInput,        2,      127.0,      18.415,     6.3,    6.3     )
INFO(Jack,      CvInput,        3,      139.7,      18.415,     6.3,    6.3     )

INFO(Jack,      GateOutput,     0,      152.4,      18.415,     6.3,    6.3     )
INFO(Jack,      GateOutput,     1,      152.4,      31.115,     6.3,    6.3     )
INFO(Jack,      GateOutput,     2,      152.4,      43.815,     6.3,    6.3     )
INFO(Jack,      GateOutput,     3,      152.4,      56.515,     6.3,    6.3     )
INFO(Jack,      GateOutput,     4,      152.4,      69.215,     6.3,    6.3     )
INFO(Jack,      GateOutput,     5,      152.4,      81.915,     6.3,    6.3     )
INFO(Jack,      GateOutput,     6,      152.4,      94.615,     6.3,    6.3     )
INFO(Jack,      GateOutput,     7,      152.4,      107.315,    6.3,    6.3     )

INFO(Jack,      CvOutput,       0,      165.1,      18.415,     6.3,    6.3     )
INFO(Jack,      CvOutput,       1,      165.1,      31.115,     6.3,    6.3     )
INFO(Jack,      CvOutput,       2,      165.1,      43.815,     6.3,    6.3     )
INFO(Jack,      CvOutput,       3,      165.1,      56.515,     6.3,    6.3     )
INFO(Jack,      CvOutput,       4,      165.1,      69.215,     6.3,    6.3     )
INFO(Jack,      CvOutput,       5,      165.1,      81.915,     6.3,    6.3     )
INFO(Jack,      CvOutput,       6,      165.1,      94.615,     6.3,    6.3     )
INFO(Jack,      CvOutput,       7,      165.1,      107.315,    6.3,    6.3     )

    // function buttons

INFO(RButton,   Button,         32,     55.244,     74.295,     10.0,   5.1     )
INFO(RButton,   Button,         33,     70.484,     74.295,     10.0,   5.1     )
INFO(RButton,   Button,         34,     85.724,     74.295,     10.0,   5.1     )
INFO(RButton,   Button,         35,     100.964,    74.295,     10.0,   5.1     )
INFO(RButton,   Button,         36,     116.204,    74.295,     10.0,   5.1     )

    // global buttons

INFO(CButton,   Button,         24,     12.7,       74.295,     5.3,    5.3     )
INFO(CButton,   Button,         25,     25.4,       74.295,     5.3,    5.3     )
INFO(CButton,   Button,         26,     12.7,       86.995,     5.3,    5.3     )
INFO(CButton,   Button,         27,     25.4,       86.995,     5.3,    5.3     )
INFO(CButton,   Button,         28,     12.7,       99.695,     5.3,    5.3     )
INFO(CButton,   Button,         29,     25.4,       99.695,     5.3,    5.3     )
INFO(CButton,   Button,         30,     12.7,       112.395,    5.3,    5.3     )
INFO(CButton,   Button,         31,     25.4,       112.395,    5.3,    5.3     )

    // track 1-8 buttons

INFO(CButton,   Button,         16,     44.45,      86.995,     5.3,    5.3     )
INFO(CButton,   Button,         17,     57.15,      86.995,     5.3,    5.3     )
INFO(CButton,   Button,         18,     69.85,      86.995,     5.3,    5.3     )
INFO(CButton,   Button,         19,     82.55,      86.995,     5.3,    5.3     )
INFO(CButton,   Button,         20,     95.25,      86.995,     5.3,    5.3     )
INFO(CButton,   Button,         21,     107.95,     86.995,     5.3,    5.3     )
INFO(CButton,   Button,         22,     120.65,     86.995,     5.3,    5.3     )
INFO(CButton,   Button,         23,     133.35,     86.995,     5.3,    5.3     )

    // step 1-8 buttons

INFO(CButton,   Button,         8,      44.45,      99.695,     5.3,    5.3     )
INFO(CButton,   Button,         9,      57.15,      99.695,     5.3,    5.3     )
INFO(CButton,   Button,         10,     69.85,      99.695,     5.3,    5.3     )
INFO(CButton,   Button,         11,     82.55,      99.695,     5.3,    5.3     )
INFO(CButton,   Button,         12,     95.25,      99.695,     5.3,    5.3     )
INFO(CButton,   Button,         13,     107.95,     99.695,     5.3,    5.3     )
INFO(CButton,   Button,         14,     120.65,     99.695,     5.3,    5.3     )
INFO(CButton,   Button,         15,     133.35,     99.695,     5.3,    5.3     )

    // step 9-16 buttons

INFO(CButton,   Button,         0,      44.45,      112.395,    5.3,    5.3     )
INFO(CButton,   Button,         1,      57.15,      112.395,    5.3,    5.3     )
INFO(CButton,   Button,         2,      69.85,      112.395,    5.3,    5.3     )
INFO(CButton,   Button,         3,      82.55,      112.395,    5.3,    5.3     )
INFO(CButton,   Button,         4,      95.25,      112.395,    5.3,    5.3     )
INFO(CButton,   Button,         5,      107.95,     112.395,    5.3,    5.3     )
INFO(CButton,   Button,         6,      120.65,     112.395,    5.3,    5.3     )
INFO(CButton,   Button,         7,      133.35,     112.395,    5.3,    5.3     )

    // global leds

INFO(Led,       Led,            24,     12.7,       67.945,     3.3,    3.3     )
INFO(Led,       Led,            25,     25.4,       67.945,     3.3,    3.3     )
INFO(Led,       Led,            26,     12.7,       80.645,     3.3,    3.3     )
INFO(Led,       Led,            27,     25.4,       80.645,     3.3,    3.3     )
INFO(Led,       Led,            28,     12.7,       93.345,     3.3,    3.3     )
INFO(Led,       Led,            29,     25.4,       93.345,     3.3,    3.3     )
INFO(Led,       Led,            30,     12.7,       106.045,    3.3,    3.3     )
INFO(Led,       Led,            31,     25.4,       106.045,    3.3,    3.3     )

    // track 1-8 leds

INFO(Led,       Led,            16,     44.45,      80.645,     3.3,    3.3     )
INFO(Led,       Led,            17,     57.15,      80.645,     3.3,    3.3     )
INFO(Led,       Led,            18,     69.85,      80.645,     3.3,    3.3     )
INFO(Led,       Led,            19,     82.55,      80.645,     3.3,    3.3     )
INFO(Led,       Led,            20,     95.25,      80.645,     3.3,    3.3     )
INFO(Led,       Led,            21,     107.95,     80.645,     3.3,    3.3     )
INFO(Led,       Led,            22,     120.65,     80.645,     3.3,    3.3     )
INFO(Led,       Led,            23,     133.35,     80.645,     3.3,    3.3     )

    // step 1-8 leds

INFO(Led,       Led,            8,      44.45,      93.345,     3.3,    3.3     )
INFO(Led,       Led,            9,      57.15,      93.345,     3.3,    3.3     )
INFO(Led,       Led,            10,     69.85,      93.345,     3.3,    3.3     )
INFO(Led,       Led,            11,     82.55,      93.345,     3.3,    3.3     )
INFO(Led,       Led,            12,     95.25,      93.345,     3.3,    3.3     )
INFO(Led,       Led,            13,     107.95,     93.345,     3.3,    3.3     )
INFO(Led,       Led,            14,     120.65,     93.345,     3.3,    3.3     )
INFO(Led,       Led,            15,     133.35,     93.345,     3.3,    3.3     )

    // step 9-16 leds

INFO(Led,       Led,            0,      44.45,      106.045,    3.3,    3.3     )
INFO(Led,       Led,            1,      57.15,      106.045,    3.3,    3.3     )
INFO(Led,       Led,            2,      69.85,      106.045,    3.3,    3.3     )
INFO(Led,       Led,            3,      82.55,      106.045,    3.3,    3.3     )
INFO(Led,       Led,            4,      95.25,      106.045,    3.3,    3.3     )
INFO(Led,       Led,            5,      107.95,     106.045,    3.3,    3.3     )
INFO(Led,       Led,            6,      120.65,     106.045,    3.3,    3.3     )
INFO(Led,       Led,            7,      133.35,     106.045,    3.3,    3.3     )

};

#undef INFO

} // namespace Frontpanel

} // namespace sim
