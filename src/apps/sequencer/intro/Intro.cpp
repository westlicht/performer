#include "Intro.h"

#include "core/math/Math.h"
#include "core/math/Vec3.h"
#include "core/math/Mat4.h"

const Vec3 boxVertices[] = {
    { -1.f, -1.f, -1.f },
    { -1.f, -1.f,  1.f },
    { -1.f,  1.f, -1.f },
    { -1.f,  1.f,  1.f },
    {  1.f, -1.f, -1.f },
    {  1.f, -1.f,  1.f },
    {  1.f,  1.f, -1.f },
    {  1.f,  1.f,  1.f },
};

const uint8_t boxLineIndices[] = {
    0, 1,
    1, 3,
    3, 2,
    2, 0,

    4, 5,
    5, 7,
    7, 6,
    6, 4,

    0, 4,
    1, 5,
    2, 6,
    3, 7
};

static void transformVertices(const Mat4 &transform, const Vec3 *src, Vec3 *dst, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = transform * src[i];
    }
}

Intro::Intro() {
}

void Intro::init() {
    _time = 0.f;
}

void Intro::update(float dt) {
    _time += dt;
}

void Intro::draw(Canvas &canvas) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::None);
    canvas.fill();
    canvas.setColor(Color::Bright);

    Vec3 eye(0.f, 0.f, -15.f);
    Vec3 target(0.f);
    Vec3 up(0.f, 1.f, 0.f);
    Mat4 viewMatrix = Mat4::lookAt(eye, target - eye, up);

    Mat4 projMatrix = Mat4::perspective(deg2rad(20.f), 1.f, 0.1f, 40.f);

    Vec2 positions[8];

    canvas.setBlendMode(BlendMode::Add);
    canvas.setColor(Color::MediumBright);

    for (int instance = -2; instance <= 2; ++instance) {
        Mat4 modelMatrix = Mat4::translate(Vec3(instance * 6.f, 0.f, 0.f)) * Mat4::rotXYZ(Vec3((_time + instance) * 0.3, (_time + instance) * 0.7, (_time + instance) * 1.3));
        Mat4 modelViewProjMatrix = projMatrix * viewMatrix * modelMatrix;

        for (int i = 0; i < 8; ++i) {
            const auto &src = boxVertices[i];
            Vec4 srch(src);
            Vec4 dsth = modelViewProjMatrix * srch;
            Vec2 pos(dsth.x / dsth.w, dsth.y / dsth.w);
            positions[i] = pos * 20.f + Vec2(128.f, 32.f);
        }

        for (int i = 0; i < 8; ++i) {
            const auto &pos = positions[i];
            int x = std::round(pos.x);
            int y = std::round(pos.y);
            canvas.point(x, y);
        }

        for (int i = 0; i < 12; ++i) {
            const auto &p0 = positions[boxLineIndices[i * 2]];
            const auto &p1 = positions[boxLineIndices[i * 2 + 1]];
            canvas.line(p0.x, p0.y, p1.x, p1.y);
        }
    }


}
