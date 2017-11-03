#pragma once

template<
    int CellCount,
    int CellWidth,
    int CellHeight,
    int CellSpacing,
    int Padding
>
struct GridLayout {
    constexpr int cellCount() const { return CellCount; }
    constexpr int cellWidth() const { return CellWidth; }
    constexpr int cellHeight() const { return CellHeight; }
    constexpr int cellSpacing() const { return CellSpacing; }
    constexpr int padding() const { return Padding; }

    constexpr int width() const { return  cellCount() * cellWidth() + (cellCount() - 1) * cellSpacing() + 2 * padding(); };
    constexpr int height() const { return cellHeight() + 2 * padding(); }
    constexpr int cellX(int index) const { return padding() + index * (cellWidth() + cellSpacing()); }
    constexpr int cellY(int index) const { return padding(); }
};
