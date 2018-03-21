#include "UnitTest.h"

#include "core/utils/ObjectPool.h"

UNIT_TEST("ObjectPool") {

    CASE("allocate/release all") {
        ObjectPool<int, 4> pool;

        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(4));

        int *items[4];

        for (size_t i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            expect(items[i] != nullptr);
            expectEqual(pool.capacity(), size_t(4));
            expectEqual(pool.size(), size_t(3 - i));
            *items[i] = i;
        }

        for (int i = 0; i < 4; ++i) {
            expectEqual(*items[i], i);
        }

        for (size_t i = 0; i < 4; ++i) {
            pool.release(items[i]);
            expectEqual(pool.capacity(), size_t(4));
            expectEqual(pool.size(), size_t(i + 1));
        }
    }

    CASE("allocate/release/reuse") {
        ObjectPool<int, 4> pool;

        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(4));

        int *items[4];

        for (size_t i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            expect(items[i] != nullptr);
            expectEqual(pool.capacity(), size_t(4));
            expectEqual(pool.size(), size_t(3 - i));
            *items[i] = i;
        }

        for (int i = 0; i < 4; ++i) {
            expectEqual(*items[i], i);
        }

        pool.release(items[0]);
        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(1));

        pool.release(items[3]);
        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(2));

        items[0] = pool.allocate();
        expect(items[0] != nullptr && items[0] != items[1] && items[0] != items[2]);
        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(1));

        items[3] = pool.allocate();
        expect(items[3] != nullptr && items[3] != items[0] && items[3] != items[1] && items[3] != items[2]);
        expectEqual(pool.capacity(), size_t(4));
        expectEqual(pool.size(), size_t(0));

        for (size_t i = 0; i < 4; ++i) {
            pool.release(items[i]);
            expectEqual(pool.capacity(), size_t(4));
            expectEqual(pool.size(), size_t(i + 1));
        }
    }

    CASE("forEach") {
        ObjectPool<int, 4> pool;
        int *items[4];

        for (int i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            *items[i] = 1 << i;
        }

        pool.release(items[0]);
        pool.release(items[3]);

        int sum = 0;
        pool.forEach([&sum] (int i) {
            sum += i;
        });
        expectEqual(sum, 6);
    }

}
