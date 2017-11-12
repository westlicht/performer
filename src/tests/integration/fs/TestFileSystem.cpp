#include "test/TestRunner.h"

#include "core/fs/FileSystem.h"

#include "core/utils/Random.h"

#include "drivers/SDCard.h"

#include <cstring>

class TestFileSystem : public Test {
public:
    TestFileSystem() :
        volume(sdcard)
    {}

    void init() {
        sdcard.init();

        DBG("Formatting volume ...");
        volume.format();

        DBG("Mounting volume ...");
        volume.mount();
        DBG("Total size = %zd kB, Free size = %zd kB", volume.sizeTotal(), volume.sizeFree());

        static constexpr size_t DataLength = 4096;

        static uint8_t data[DataLength];
        static uint8_t buf[DataLength];

        Random rng;

        // create random data
        for (size_t i = 0; i < DataLength; ++i) {
            data[i] = rng.next() >> (i % 24) & 0xff;
        }

        // write data
        DBG("Writing file ...");
        fs::File file("test.dat", fs::File::Write);
        file.write(data, DataLength);
        file.close();

        // read data
        DBG("Reading file ...");
        file.open("test.dat", fs::File::Read);
        file.read(buf, DataLength);
        file.close();

        // verify data
        bool success = true;
        for (size_t i = 0; i < DataLength; ++i) {
            if (buf[i] != data[i]) {
                DBG("Verify failed: buf[%zd] = %02x, data[%zd] = %02x", i, buf[i], i, data[i]);
                success = false;
                break;
            }
        }

        DBG("Total size = %zd kB, Free size = %zd kB", volume.sizeTotal(), volume.sizeFree());

        DBG("Unmounting volume ...");
        volume.unmount();
    }

private:
    SDCard sdcard;
    fs::Volume volume;
};

TEST(TestFileSystem)
