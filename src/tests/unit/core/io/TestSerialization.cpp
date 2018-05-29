#include "UnitTest.h"

#include "MemoryReaderWriter.h"

#include "core/io/SerializedWriter.h"
#include "core/io/SerializedReader.h"

static uint8_t buf[512];

UNIT_TEST("Serialization") {

    CASE("write/read") {
        MemoryWriter memoryWriter(buf, sizeof(buf));
        SerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); });

        for (int i = 0; i < 10; ++i) {
            int8_t i8 = -i;
            uint8_t u8 = i;
            int16_t i16 = -i * 2;
            uint16_t u16 = i * 2;
            int32_t i32 = -i * 4;
            uint32_t u32 = i * 4;
            writer.write(i8);
            writer.write(u8);
            writer.write(i16);
            writer.write(u16);
            writer.write(i32);
            writer.write(u32);
        }

        MemoryReader memoryReader(buf, sizeof(buf));
        SerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); });

        for (int i = 0; i < 10; ++i) {
            int8_t i8;
            uint8_t u8;
            int16_t i16;
            uint16_t u16;
            int32_t i32;
            uint32_t u32;
            reader.read(i8);
            reader.read(u8);
            reader.read(i16);
            reader.read(u16);
            reader.read(i32);
            reader.read(u32);
            expectEqual(i8, int8_t(-i));
            expectEqual(u8, uint8_t(i));
            expectEqual(i16, int16_t(-i * 2));
            expectEqual(u16, uint16_t(i * 2));
            expectEqual(i32, int32_t(-i * 4));
            expectEqual(u32, uint32_t(i * 4));
        }

        expectEqual(memoryWriter.bytesWritten(), memoryReader.bytesRead());
    }

}
