#include "UnitTest.h"

#include "core/utils/StringUtils.h"

UNIT_TEST("StringUtils") {

    CASE("copy") {
        char buf[4];

        buf[0] = '\1';
        StringUtils::copy(buf, "", 0);
        expectEqual(buf[0], '\1');

        buf[0] = '\1';
        StringUtils::copy(buf, "", 1);
        expectEqual(buf, "");

        buf[0] = '\1';
        StringUtils::copy(buf, "", sizeof(buf));
        expectEqual(buf, "");

        StringUtils::copy(buf, "a", sizeof(buf));
        expectEqual(buf, "a");

        StringUtils::copy(buf, "abc", sizeof(buf));
        expectEqual(buf, "abc");

        StringUtils::copy(buf, "test", sizeof(buf));
        expectEqual(buf, "tes");
    }

}
