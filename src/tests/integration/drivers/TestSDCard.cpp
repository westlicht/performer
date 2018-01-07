#include "IntegrationTest.h"

#include "core/utils/Random.h"

#include "drivers/SDCard.h"

class TestSDCard : public IntegrationTest {
public:
    TestSDCard() :
        IntegrationTest("SDCard", false)
    {}

    void init() {
        sdcard.init();

        while (!sdcard.available()) {
            DBG("Waiting for SDCard ...");
        }

        // DBG("SDCard init");
        // auto result = sdcard.available();
        // DBG("SDCard %s", result ? "found" : "not found");
        // return;


        if (sdcard.writeProtected()) {
            DBG("SDCard is write protected -> aborting");
            return;
        }

        DBG("sectorCount = %zd", sdcard.sectorCount());
        DBG("sectorSize = %zd", sdcard.sectorSize());

        if (sdcard.sectorSize() != 512) {
            DBG("SDCard has unsupported sector size -> aborting");
            return;
        }

        static constexpr uint32_t Runs = 32;
        static constexpr uint32_t SectorCount = 1;
        static constexpr uint32_t DataLength = SectorCount * 512;

        static uint8_t data[DataLength];
        static uint8_t buf[DataLength];

        Random rng;

        uint32_t writeTime = 0;
        uint32_t readTime = 0;

        for (size_t run = 0; run < Runs; ++run) {
            // create random data
            for (size_t i = 0; i < DataLength; ++i) {
                data[i] = (rng.next() >> (i % 24)) & 0xff;
            }

            // write data
            uint32_t start = os::ticks();
            if (!sdcard.write(data, 0, SectorCount)) {
                DBG("write failed");
            }
            sdcard.sync();
            writeTime += (os::ticks() - start) / os::time::ms(1);

            // read data
            start = os::ticks();
            if (!sdcard.read(buf, 0, SectorCount)) {
                DBG("read failed");
            }
            readTime += (os::ticks() - start) / os::time::ms(1);

            // verify data
            bool success = true;
            for (size_t i = 0; i < DataLength; ++i) {
                if (buf[i] != data[i]) {
                    DBG("Verify failed: buf[%zd] = %02x, data[%zd] = %02x", i, buf[i], i, data[i]);
                    success = false;
                    break;
                }
            }
        }

        // report throughput
        DBG("Write throughput: %.1f kB/s", (DataLength / 1024.f) / writeTime);
        DBG("Read throughput: %.1f kB/s", (DataLength / 1024.f) / readTime);
    }

private:
    SDCard sdcard;
};

INTEGRATION_TEST(TestSDCard)
