#pragma once

template<
    int ContainerWidth,
    int ContainerHeight,
    int Width,
    int Height
>
struct CenterLayout {
    constexpr int containerWidth() const { return ContainerWidth; }
    constexpr int containerHeight() const { return ContainerHeight; }
    constexpr int width() const { return Width; }
    constexpr int height() const { return Height; }
    constexpr int originX() const { return (containerWidth() - width()) / 2; }
    constexpr int originY() const { return (containerHeight() - height()) / 2; }
};
