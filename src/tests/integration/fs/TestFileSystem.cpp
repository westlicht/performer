#include "IntegrationTest.h"

#include "core/fs/FileSystem.h"
#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

#include "core/utils/Random.h"
#include "core/utils/StringBuilder.h"

#include "drivers/SdCard.h"

#include <cstring>

Random rng;

class FileSystemTest : public IntegrationTest {
public:
    FileSystemTest() :
        volume(sdCard)
    {}

    void init() override {
        sdCard.init();
    }

    void once() override {
        // testFileWriteRead();
        // testDirectoryList();
        testFileWriterReader();
    }

    void fsAssert(fs::Error actual, fs::Error expected, const char *msg) {
        EXPECT(actual == expected, "%s (actual: %s, expected: %s)", msg, fs::errorToString(actual), fs::errorToString(expected));
    }

    template<typename Func>
    void test(const char *name, Func func) {
        DBG("Test: %s", name);
        DBG("format");
        fsAssert(volume.format(), fs::OK, "failed to format volume");
        DBG("mount");
        fsAssert(volume.mount(), fs::OK, "failed to mount volume");
        dumpStats();
        func();
        dumpStats();
        DBG("unmount");
        fsAssert(volume.unmount(), fs::OK, "failed to unmount volume");
    }

    void dumpStats() {
        DBG("Total size = %zd kB, Free size = %zd kB", volume.sizeTotal(), volume.sizeFree());
    }

    void testFileWriteRead() {
        test("File write/read/stat/delete", [this] () {
            static constexpr size_t DataLength = 16*1024;
            static uint8_t data[DataLength];
            static uint8_t buf[DataLength];

            const char *filename = "test.dat";

            // create random data
            for (size_t i = 0; i < DataLength; ++i) {
                data[i] = rng.next() & 0xff;
            }

            EXPECT(fs::exists(filename) == false, "file must not exist");

            // write data
            DBG("Writing file ...");
            fs::File file;
            fsAssert(file.open(filename, fs::File::Write), fs::OK, "failed to open file for writing");
            fsAssert(file.write(data, DataLength), fs::OK, "failed to write to file");
            fsAssert(file.close(), fs::OK, "failed to close file after writing");

            EXPECT(fs::exists(filename) == true, "file must exist");

            // read data
            DBG("Reading file ...");
            fsAssert(file.open(filename, fs::File::Read), fs::OK, "failed to open file for reading");
            fsAssert(file.read(buf, DataLength), fs::OK, "failed to read from file");
            fsAssert(file.close(), fs::OK, "failed to close file after reading");

            // remove file
            fsAssert(fs::remove(filename), fs::OK, "failed to remove file");

            EXPECT(fs::exists(filename) == false, "file must not exist");

            // verify data
            bool success = true;
            for (size_t i = 0; i < DataLength; ++i) {
                if (buf[i] != data[i]) {
                    DBG("Verify failed: buf[%zd] = %02x, data[%zd] = %02x", i, buf[i], i, data[i]);
                    success = false;
                    break;
                }
            }
            EXPECT(success, "invalid data")
        });
    }

    void testDirectoryList() {
        test("Directory list", [this] () {
            // create some files
            fs::File file;
            for (int i = 0; i < 10; ++i) {
                fsAssert(file.open(FixedStringBuilder<8>("%d.dat", i), fs::File::Write), fs::OK, "failed to open file for writing");
                uint8_t data = i;
                fsAssert(file.write(&data, 1), fs::OK, "failed to write to file");
                fsAssert(file.close(), fs::OK, "failed to close file after writing");
            }

            fs::Directory dir("");
            while (dir.next()) {
                DBG("%s (%zd bytes)", dir.info().name(), dir.info().size());
            }
        });
    }

    void testFileWriterReader() {
        test("FileWriter/FileReader", [this] () {
            for (int i = 0; i < 10; ++i) {
                DBG("writing file %d ...", i);
                fs::FileWriter writer(FixedStringBuilder<8>("%d.dat", i));
                const char text[8] = { 'T', 'E', 'S', 'T', char('0' + i), 0, 0, 0 };
                fsAssert(writer.write(text, 8), fs::OK, "failed to write");
                for (int j = 0; j < 500; ++j) {
                    uint32_t data = i * 512 + j;
                    fsAssert(writer.write(&data, sizeof(data)), fs::OK, "failed to write");
                }
                fsAssert(writer.finish(), fs::OK, "failed to finish writing");
            }

            for (int i = 0; i < 10; ++i) {
                DBG("reading file %d ...", i);
                fs::FileReader reader(FixedStringBuilder<8>("%d.dat", i));
                char text[8];
                const char expected_text[8] = { 'T', 'E', 'S', 'T', char('0' + i), 0, 0, 0 };
                fsAssert(reader.read(text, 8), fs::OK, "failed to read");
                for (int j = 0; j < 8; ++j) {
                    EXPECT(text[j] == expected_text[j], "read invalid data (text)");
                }
                for (int j = 0; j < 500; ++j) {
                    uint32_t data;
                    uint32_t expected = i * 512 + j;
                    fsAssert(reader.read(&data, sizeof(data)), fs::OK, "failed to read");
                    EXPECT(data == expected, "read invalid data");
                }
                fsAssert(reader.finish(), fs::OK, "failed to finish writing");
            }
        });
    }


private:
    SdCard sdCard;
    fs::Volume volume;
};

INTEGRATION_TEST(FileSystemTest, "FileSystem", false)
