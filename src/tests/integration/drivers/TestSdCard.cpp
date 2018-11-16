#include "IntegrationTest.h"

#include "core/utils/Random.h"

#include "drivers/SdCard.h"

class TestSdCard : public IntegrationTest {
public:
    void init() override {
        sdCard.init();
    }

    void once() override {
        while (!sdCard.available()) {
            DBG("Waiting for SdCard ...");
        }

        if (sdCard.writeProtected()) {
            DBG("SdCard is write protected -> aborting");
            return;
        }

        DBG("sectorCount = %zd", sdCard.sectorCount());
        DBG("sectorSize = %zd", sdCard.sectorSize());

        if (sdCard.sectorSize() != 512) {
            DBG("SdCard has unsupported sector size -> aborting");
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
            if (!sdCard.write(data, 0, SectorCount)) {
                DBG("write failed");
            }
            writeTime += timer.elapsed();

            sdCard.sync();

            // read data
            timer.reset();
            if (!sdCard.read(buf, 0, SectorCount)) {
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
            EXPECT(success, "invalid data");
        }

        // report throughput
        DBG("Write throughput: %.1f kB/s", (DataLength / 1024.0) * 1000000.0 / writeTime);
        DBG("Read throughput: %.1f kB/s", (DataLength / 1024.0) * 1000000.0 / readTime);
    }

private:
    SdCard sdCard;
};

INTEGRATION_TEST(TestSdCard, "SdCard", false)
