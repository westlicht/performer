#include "UnitTest.h"

#include "MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#define VERSION(_x_) (_x_)

struct Data1 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    uint32_t field3 = 345;
};

struct Data2 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    int8_t field4 = -123; // new
    uint32_t field3 = 345;
};

struct Data3 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    int8_t field4 = -123;
    int16_t field5 = -234; // new
    uint32_t field3 = 345;
};

struct Data4 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    // int8_t field4 = -123; // removed
    int16_t field5 = -234;
    uint32_t field3 = 345;
};

static void writeVersion1(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 1);
    Data1 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion2(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 2);
    Data2 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field4);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion3(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 3);
    Data3 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field4);
    writer.write(data.field5);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion4(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 4);
    Data4 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field5);
    writer.write(data.field3);
    writer.writeHash();
}

static void readVersion1(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 1);
    Data1 data;
    std::memset(&data, 0, sizeof(data));
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field3);
    expectTrue(reader.checkHash());
    Data1 expected;
    expectEqual(data.field1, expected.field1);
    expectEqual(data.field2, expected.field2);
    expectEqual(data.field3, expected.field3);
}

static void readVersion2(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 2);
    Data2 data;
    std::memset(&data, 0, sizeof(data));
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field4, VERSION(2));
    reader.read(data.field3);
    expectTrue(reader.checkHash());
    Data2 expected;
    expectEqual(data.field1, expected.field1);
    expectEqual(data.field2, expected.field2);
    expectEqual(data.field4, reader.dataVersion() >= 2 ? expected.field4 : int8_t(0));
    expectEqual(data.field3, expected.field3);
}

static void readVersion3(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
    Data3 data;
    std::memset(&data, 0, sizeof(data));
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field4, VERSION(2));
    reader.read(data.field5, VERSION(3));
    reader.read(data.field3);
    expectTrue(reader.checkHash());
    Data3 expected;
    expectEqual(data.field1, expected.field1);
    expectEqual(data.field2, expected.field2);
    expectEqual(data.field4, reader.dataVersion() >= 2 ? expected.field4 : int8_t(0));
    expectEqual(data.field5, reader.dataVersion() >= 3 ? expected.field5 : int16_t(0));
    expectEqual(data.field3, expected.field3);
}

static void readVersion4(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 4);
    Data4 data;
    std::memset(&data, 0, sizeof(data));
    reader.read(data.field1);
    reader.read(data.field2);
    reader.skip<int8_t>(VERSION(2), VERSION(4));
    reader.read(data.field5, VERSION(3));
    reader.read(data.field3);
    expectTrue(reader.checkHash());
    Data4 expected;
    expectEqual(data.field1, expected.field1);
    expectEqual(data.field2, expected.field2);
    expectEqual(data.field5, reader.dataVersion() >= 3 ? expected.field5 : int16_t(0));
    expectEqual(data.field3, expected.field3);
}

static uint8_t buf[512];

static void clear() {
    std::memset(buf, 0, sizeof(buf));
}

UNIT_TEST("VersionedSerialization") {

    CASE("version 1") {
        // version 1 reader
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion1(buf, sizeof(buf));
    }

    CASE("version 2") {
        // version 2 reader
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion2(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion2(buf, sizeof(buf));
    }

    CASE("version 3") {
        // version 3 reader
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));

        clear();
        writeVersion3(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));
    }

    CASE("version 4") {
        // version 4 reader
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion3(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion4(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));
    }

}
