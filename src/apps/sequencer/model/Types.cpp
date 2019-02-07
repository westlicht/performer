#include "Types.h"

const Types::ConditionInfo Types::conditionInfos[] = {
    [int(Types::Condition::Off)]        = { "Off",      "-",    ""  },
    [int(Types::Condition::Fill)]       = { "Fill",     "F",    ""  },
    [int(Types::Condition::NotFill)]    = { "!Fill",    "!F",   ""  },
    [int(Types::Condition::Pre)]        = { "Pre",      "P",    ""  },
    [int(Types::Condition::NotPre)]     = { "!Pre",     "!P",   ""  },
    [int(Types::Condition::First)]      = { "First",    "1",    ""  },
    [int(Types::Condition::NotFirst)]   = { "!First",   "!1",   ""  },
};

const Types::VoltageRangeInfo Types::voltageRangeInfos[] = {
    [int(Types::VoltageRange::Unipolar1V)]  = { 0.f, 1.f },
    [int(Types::VoltageRange::Unipolar2V)]  = { 0.f, 2.f },
    [int(Types::VoltageRange::Unipolar3V)]  = { 0.f, 3.f },
    [int(Types::VoltageRange::Unipolar4V)]  = { 0.f, 4.f },
    [int(Types::VoltageRange::Unipolar5V)]  = { 0.f, 5.f },
    [int(Types::VoltageRange::Bipolar1V)]   = { -1.f, 1.f },
    [int(Types::VoltageRange::Bipolar2V)]   = { -2.f, 2.f },
    [int(Types::VoltageRange::Bipolar3V)]   = { -3.f, 3.f },
    [int(Types::VoltageRange::Bipolar4V)]   = { -4.f, 4.f },
    [int(Types::VoltageRange::Bipolar5V)]   = { -5.f, 5.f },
};
