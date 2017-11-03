#pragma once

template<
    int ContainerSize,
    int CellCount,
    int CellSize
>
struct PartitionLayout {
    constexpr int containerSize() const { return ContainerSize; }
    constexpr int cellCount() const { return CellCount; }
    constexpr int cellSize() const { return CellSize; }

    constexpr int cellSpan() const { return ContainerSize / CellCount; }
    constexpr int cellPadding() const { return (cellSpan() - cellSize()) / 2; }
    constexpr int cellOffset(int index) const { return index * cellSpan() + cellPadding(); }
};
