#include "IntegrationTest.h"

#include "core/utils/Random.h"

#include "drivers/SDCard.h"

class TestSDCard : public IntegrationTest {
public:
    TestSDCard() :
        IntegrationTest("SDCard", false)
    {}

    void init() override {
        sdcard.init();
    }

    void once() override {
        while (!sdcard.available()) {
            DBG("Waiting for SDCard ...");
        }

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

        Timer timer;

        for (size_t run = 0; run < Runs; ++run) {
            // create random data
            for (size_t i = 0; i < DataLength; ++i) {
                data[i] = (rng.next() >> (i % 24)) & 0xff;
            }

            // write data
            timer.reset();
            if (!sdcard.write(data, 0, SectorCount)) {
                DBG("write failed");
            }
            writeTime += timer.elapsed();

            sdcard.sync();

            // read data
            timer.reset();
            if (!sdcard.read(buf, 0, SectorCount)) {
                DBG("read failed");
            }
            readTime += timer.elapsed();

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
        DBG("Write throughput: %.1f kB/s", (DataLength / 1024.0) * 1000000.0 / writeTime);
        DBG("Read throughput: %.1f kB/s", (DataLength / 1024.0) * 1000000.0 / readTime);
    }

private:
    SDCard sdcard;
};

INTEGRATION_TEST(TestSDCard)
