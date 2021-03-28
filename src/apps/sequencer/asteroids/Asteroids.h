#pragma once

#include "core/math/Vec2.h"
#include "core/gfx/Canvas.h"

#include <array>

namespace asteroids {

class Game;

struct Inputs {
    union {
        struct {
            uint8_t left : 1;
            uint8_t right : 1;
            uint8_t thrust : 1;
            uint8_t shoot : 1;
        };
        uint8_t keys = 0;
    };
};

struct Outputs {
    union {
        struct {
            uint8_t thurst : 1;
            uint8_t shoot : 1;
            uint8_t explosion: 1;
        };
        uint8_t gates = 0;
    };
};

//-----------------------------------------------------------------------------
// Pool
//-----------------------------------------------------------------------------

template<typename T, size_t N>
class Pool {
public:
    size_t capacity() const {
        return N;
    }

    size_t size() const {
        size_t size = 0;
        for (const auto &item : _items) {
            if (item.used) {
                ++size;
            }
        }
        return size;
    }

    void reset() {
        for (auto &item : _items) {
            item.used = false;
        }
    }

    T *allocate() {
        for (auto &item : _items) {
            if (!item.used) {
                item.used = true;
                return &item.object;
            }
        }
        return nullptr;
    }

    template<typename Func>
    void forEach(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                func(item.object);
            }
        }
    }

    template<typename Func>
    void forEachRemove(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                if (func(item.object)) {
                    item.used = false;
                }
            }
        }
    }

private:
    struct Item {
        bool used = false;
        T object;
    };

    std::array<Item, N> _items;
};

//-----------------------------------------------------------------------------
// Body
//-----------------------------------------------------------------------------

class Body {
public:
    void initBody(float mass, float radius, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);

    float mass() const { return _mass; }

    float radius() const { return _radius; }
    float radiusSqr() const { return _radiusSqr; }

    const Vec2 &position() const { return _position; }
    void setPosition(const Vec2 &position) { _position = position; }

    const Vec2 &velocity() const { return _velocity; }
    void setVelocity(const Vec2 &velocity) { _velocity = velocity; }

    float angle() const { return _angle; }
    void setAngle(float angle) { _angle = angle; }

    float angleVelocity() const { return _angleVelocity; }
    void setAngleVelocity(float angleVelocity) { _angleVelocity = angleVelocity; };

    static bool handleCollision(Body &body1, Body &body2);

protected:
    float _mass;
    float _radius;
    float _radiusSqr;
    Vec2 _position;
    Vec2 _velocity;
    float _angle;
    float _angleVelocity;
};

//-----------------------------------------------------------------------------
// Player
//-----------------------------------------------------------------------------

class Player : public Body {
public:
    Player(Game &game);

    void reset();

    void kill() { _killed = true; }
    bool killed() const { return _killed; }

    int score() const { return _score; }
    void setScore(int score) { _score = score; }
    void addScore(int score) { _score += score; }

    bool shooting() const { return _shootTime > ShootInterval - ShootGate; }

    void update(float dt, Inputs &inputs, bool handleInputs);
    void draw(Canvas &canvas);

private:
    const float AngleVelocity = 5.f;
    const float Acceleration = 100.f;
    const float Drag = 0.5f;
    const float ShootInterval = 0.25f;
    const float ShootGate = 0.05f;
    const float ShootVelocity = 100.f;
    const float ThrustParticleInterval = 0.05f;

    Game &_game;
    bool _killed;
    int _score;
    Vec2 _direction;
    float _shootTime;
    float _thrustParticleTime;
};

//-----------------------------------------------------------------------------
// Asteroid
//-----------------------------------------------------------------------------

class Asteroid : public Body {
public:
    uint8_t category() const { return _category; }

    void spawn(uint8_t category, uint8_t shape, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);

    void update(float dt);
    void draw(Canvas &canvas);

private:
    uint8_t _category;
    uint8_t _shape;
    float _scale;
};

//-----------------------------------------------------------------------------
// Projectile
//-----------------------------------------------------------------------------

class Projectile {
public:
    const Vec2 &position() const { return _position; }

    void kill() { _killed = true; }
    bool killed() const { return _killed; }

    void spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity);

    void update(float dt);
    void draw(Canvas &canvas);

private:
    Vec2 _position;
    Vec2 _direction;
    Vec2 _velocity;
    bool _killed;
};

//-----------------------------------------------------------------------------
// Particle
//-----------------------------------------------------------------------------

class Particle {
public:
    struct Params {
        float lifeTime;
        float size;
        float drag;
    };

    bool killed() const { return _killed; }

    void spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Params &params);

    void update(float dt);
    void draw(Canvas &canvas);

private:
    const Params *_params;
    float _time;
    Vec2 _position;
    Vec2 _direction;
    Vec2 _velocity;
    bool _killed;
};

//-----------------------------------------------------------------------------
// Game
//-----------------------------------------------------------------------------

class Game {
public:
    enum State {
        Intro,
        Start,
        Play,
        Win,
        Lose,
    };

    Game();

    void init();

    void reset();

    void levelStart(int level);
    bool levelFinished();

    void setState(State state);

    void update(float dt, Inputs &inputs, Outputs &outputs);
    void draw(Canvas &canvas);

    void spawnAsteroid(uint8_t category, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);
    void spawnRandomAsteroid(uint8_t category);

    void spawnProjectile(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity);

    void spawnParticle(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Particle::Params &params);
    void spawnExplosion(const Vec2 &position, const Vec2 &velocity);

    void divideAsteroid(Asteroid &asteroid);

private:
    void drawTexts(Canvas &canvas, const char *title, const char *msg);
    void drawHUD(Canvas &canvas);
    void drawShadowText(Canvas &canvas, int x, int y, Color color, const char *str);

    static void initAsteroidShapes();

    State _state;
    int _level;
    uint8_t _lastKeys = 0;

    float _time;

    Player _player;
    Pool<Asteroid, 16> _asteroids;
    Pool<Projectile, 16> _projectiles;
    Pool<Particle, 32> _particles;

    float _explosionTime = 0.f;
};

} // namespace asteroids
