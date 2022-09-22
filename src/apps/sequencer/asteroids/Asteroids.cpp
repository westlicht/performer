#include "Asteroids.h"

#include "core/Debug.h"
#include "core/math/Mat3.h"
#include "core/utils/Random.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cmath>

namespace asteroids {

static const float Pi = 3.1415926536f;
static const float TwoPi = 2.f * Pi;

static const int ScreenWidth = 256;
static const int ScreenHeight = 64;

static Vec2 directionFromAngle(float angle) {
    return Vec2(std::sin(angle), -std::cos(angle));
}

static Random rng;

static float randomFloat() {
    union {
        float f;
        uint32_t i;
    };
    uint32_t x = rng.next();
    i = (((x * 16807) >> 9) | 0x3f800000);
    return f - 1.f;
}

static Vec2 randomVec2() {
    return Vec2(randomFloat(), randomFloat());
}

static Vec2 randomDirection() {
    return directionFromAngle(randomFloat() * TwoPi);
}

static const std::array<Vec2, 4> playerShape = {{
    { 0.f, -5.f },
    {  5.f, 5.f },
    {  0.f, 3.f },
    { -5.f, 5.f },
}};

typedef std::array<Vec2, 16> AsteroidShape;
static std::array<AsteroidShape, 4> asteroidShapes;

static void drawShape(Canvas &canvas, Mat3 &transform, const Vec2 *vertices, size_t count) {
    Vec2 a, b;
    a = transform * vertices[0];
    for (size_t i = 1; i <= count; ++i) {
        b = transform * vertices[i % count];
        canvas.line(a.x, a.y, b.x, b.y);
        a = b;
    }
}

template<size_t N>
static void drawShape(Canvas &canvas, Mat3 &transform, const std::array<Vec2, N> &vertices) {
    drawShape(canvas, transform, vertices.data(), vertices.size());
}

static float shapeRadius(const Vec2 *vertices, size_t count) {
    float radiusSqr = 0.f;
    for (size_t i = 0; i < count; ++i) {
        radiusSqr = std::max(radiusSqr, vertices[i].lengthSqr());
    }
    return std::sqrt(radiusSqr);
}

template<size_t N>
static float shapeRadius(const std::array<Vec2, N> &vertices) {
    return shapeRadius(vertices.data(), vertices.size());
}

static void wrapPosition(Vec2 &position, float margin) {
    const float out = margin + 1.f;
    const float in = margin;
    if (position.x < -out) { position.x = ScreenWidth + in; }
    if (position.x > ScreenWidth + out) { position.x = -in; }
    if (position.y < -out) { position.y = ScreenHeight + in; }
    if (position.y > ScreenHeight + out) { position.y = -in; }
}

static bool isOutside(Vec2 &position, float margin = 10.f) {
    return position.x < -margin || position.x > ScreenWidth + margin || position.y < -margin || position.y > ScreenHeight + margin;
}

//-----------------------------------------------------------------------------
// Particle parameters
//-----------------------------------------------------------------------------

static Particle::Params thrustParticleParams = {
    .lifeTime = 0.5f,
    .size = 2.f,
    .drag = 0.f,
};

static Particle::Params explosionParticleParams = {
    .lifeTime = 1.f,
    .size = 8.f,
    .drag = 0.1f,
};

//-----------------------------------------------------------------------------
// Body
//-----------------------------------------------------------------------------

void Body::initBody(float mass, float radius, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity) {
    _mass = mass;
    _radius = radius;
    _radiusSqr = radius * radius;
    _position = position;
    _velocity = velocity;
    _angle = angle;
    _angleVelocity = angleVelocity;
}

bool Body::handleCollision(Body &body1, Body &body2) {
    if ((body1.position() - body2.position()).length() > body1.radius() + body2.radius()) {
        return false;
    }

    const auto &x1 = body1.position();
    const auto &x2 = body2.position();
    const auto &v1 = body1.velocity();
    const auto &v2 = body2.velocity();

    auto dx = x2 - x1;
    if (dx.dot(v1) < dx.dot(v2)) {
        return false;
    }

    float m1 = 1.f;
    float m2 = 1.f;
    float invm = 1.f / (m1 + m2);

    auto dv = v2 - v1;
    float invd2 = 1.f / dx.lengthSqr();

    body1.setVelocity(v1 + 2.f * m2 * invm * dv.dot(dx) * invd2 * dx);
    body2.setVelocity(v2 - 2.f * m1 * invm * dv.dot(dx) * invd2 * dx);

    auto ofs = 0.5f * dx.normalized() * (dx.length() - body1.radius() - body2.radius());
    body1.setPosition(x1 + ofs);
    body2.setPosition(x2 - ofs);

    return true;
}

//-----------------------------------------------------------------------------
// Player
//-----------------------------------------------------------------------------

Player::Player(Game &game) :
    _game(game)
{
    reset();
}

void Player::reset() {
    initBody(1.f, 0.9f * shapeRadius(playerShape), Vec2(128.f, 32.f), Vec2(0.f), 0.f, 0.f);
    _killed = false;
    _shootTime = 0.f;
    _thrustParticleTime = 0.f;
}

void Player::update(float dt, Inputs &inputs, bool handleInputs) {
    if (_killed) {
        return;
    }

    if (handleInputs && inputs.left) {
        _angle -= dt * AngleVelocity;
    }

    if (handleInputs && inputs.right) {
        _angle += dt * AngleVelocity;
    }

    _direction = directionFromAngle(_angle);

    if (handleInputs && inputs.thrust) {
        _velocity += dt * Acceleration * _direction;
    }

    _shootTime -= dt;

    if (handleInputs && inputs.shoot && _shootTime <= 0.f) {
        _shootTime = ShootInterval;
        _game.spawnProjectile(_position, _direction, _velocity + _direction * ShootVelocity);
    }

    _thrustParticleTime -= dt;

    if (handleInputs && inputs.thrust && _thrustParticleTime <= 0.f) {
        _thrustParticleTime = ThrustParticleInterval;
        Vec2 particleDirection = -_direction + _direction.perp() * 0.5f * (randomFloat() - 0.5f);
        _game.spawnParticle(_position - 3.f * _direction, particleDirection, _velocity + particleDirection * 20.f, thrustParticleParams);
    }

    _velocity -= dt * Drag * _velocity;
    _position += dt * _velocity;
    wrapPosition(_position, _radius);
}

void Player::draw(Canvas &canvas) {
    if (_killed) {
        return;
    }
    canvas.setColor(Color::Bright);
    Mat3 transform = Mat3::transform2D(_position, _angle, 1.f);
    drawShape(canvas, transform, playerShape);
}

//-----------------------------------------------------------------------------
// Asteroid
//-----------------------------------------------------------------------------

void Asteroid::spawn(uint8_t category, uint8_t shape, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity) {
    _category = category;
    _shape = shape;
    _scale = std::pow(1.414f, (_category - 1.f));
    initBody(_scale * _scale, 0.9f * _scale * shapeRadius(asteroidShapes[_shape]), position, velocity, angle, angleVelocity);
}

void Asteroid::update(float dt) {
    _position += dt * _velocity;
    _angle += dt * _angleVelocity;
    wrapPosition(_position, _radius);
}

void Asteroid::draw(Canvas &canvas) {
    canvas.setColor(Color::Medium);
    Mat3 transform = Mat3::transform2D(_position, _angle, _scale);
    drawShape(canvas, transform, asteroidShapes[_shape]);
}

//-----------------------------------------------------------------------------
// Projectile
//-----------------------------------------------------------------------------

void Projectile::spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity) {
    _position = position;
    _direction = direction;
    _velocity = velocity;
    _killed = false;
}

void Projectile::update(float dt) {
    _position += dt * _velocity;
    if (isOutside(_position)) {
        _killed = true;
    }
}

void Projectile::draw(Canvas &canvas) {
    const Vec2 &a = _position;
    Vec2 b = a + _direction * 3.f;
    canvas.setColor(Color::Bright);
    canvas.line(a.x, a.y, b.x, b.y);
}

//-----------------------------------------------------------------------------
// Particle
//-----------------------------------------------------------------------------

void Particle::spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Params &params) {
    _params = &params;
    _time = 0.f;
    _position = position;
    _direction = direction;
    _velocity = velocity;
    _killed = false;
}

void Particle::update(float dt) {
    _time += dt;
    _velocity -= dt * _params->drag * _velocity;
    _position += dt * _velocity;
    if (isOutside(_position) || _time > _params->lifeTime) {
        _killed = true;
    }
}

void Particle::draw(Canvas &canvas) {
    const Vec2 &a = _position;
    Vec2 b = a + _direction * 3.f;
    uint8_t color = static_cast<uint8_t>((1.f - _time / _params->lifeTime) * 0xf);
    canvas.setColorValue(color);
    canvas.line(a.x, a.y, b.x, b.y);
}

//-----------------------------------------------------------------------------
// Game
//-----------------------------------------------------------------------------

Game::Game() :
    _player(*this)
{}

void Game::init() {
    initAsteroidShapes();
    setState(Intro);
}

void Game::reset() {
    _player.reset();
    _asteroids.reset();
    _projectiles.reset();
    _particles.reset();
}

void Game::levelStart(int level) {
    reset();

    for (int i = 0; i < ((level - 1) % 3 + 1); ++i) {
        spawnRandomAsteroid((level - 1) / 3);
    }

}

bool Game::levelFinished() {
    return _asteroids.size() == 0;
}

void Game::setState(Game::State state) {

    _time = 0.f;

    switch (state) {
    case Intro:
        reset();
        for (int i = 0; i < 10; ++i) {
            spawnRandomAsteroid(i % 4);
        }
        _player.kill();
        _player.setScore(0);
        _level = 0;
        break;
    case Start:
        ++_level;
        break;
    case Play:
        levelStart(_level);
        break;
    default:
        break;
    }
    _state = state;
}

void Game::update(float dt, Inputs &inputs, Outputs &outputs) {

    _time += dt;

    bool keyPressed = inputs.keys != 0 && _lastKeys == 0;
    bool nextState = _time > 1.f && keyPressed;
    _lastKeys = inputs.keys;

    bool handlePlayerInputs = false;

    switch (_state) {
    case Intro:
        if (nextState) {
            setState(Start);
        }
        break;
    case Start:
        if (nextState) {
            setState(Play);
        }
        break;
    case Play:
        handlePlayerInputs = true;
        if (levelFinished()) {
            setState(Win);
        }
        if (_player.killed()) {
            setState(Lose);
        }
        break;
    case Win:
        if (nextState) {
            setState(Start);
        }
        break;
    case Lose:
        if (nextState) {
            setState(Intro);
        }
        break;
    default:
        break;
    }

    _player.update(dt, inputs, handlePlayerInputs);

    // update asteroids and asteroid <-> player collisions
    _asteroids.forEachRemove([&] (Asteroid &asteroid) {
        asteroid.update(dt);
        if (!_player.killed() && Body::handleCollision(_player, asteroid)) {
            _player.kill();
            spawnExplosion(_player.position(), _player.velocity());
            return false;
        }
        return false;
    });

    // handle asteroid collisions
    _asteroids.forEach([&] (Asteroid &asteroid1) {
        _asteroids.forEach([&] (Asteroid &asteroid2) {
            if (&asteroid1 > &asteroid2) {
                Body::handleCollision(asteroid1, asteroid2);
            }
        });
    });

    // update projectiles and projectile <-> asteroid collisions
    _projectiles.forEachRemove([&] (Projectile &projectile) {
        projectile.update(dt);
        _asteroids.forEachRemove([&] (Asteroid &asteroid) {
            if (!projectile.killed() && (asteroid.position() - projectile.position()).lengthSqr() < asteroid.radiusSqr()) {
                spawnExplosion(projectile.position(), asteroid.velocity());
                projectile.kill();
                if (asteroid.category() > 0) {
                    divideAsteroid(asteroid);
                }
                _player.addScore((4 << asteroid.category()) * 10);
                return true;
            }
            return false;
        });
        return projectile.killed();
    });

    // update particles
    _particles.forEachRemove([&] (Particle &particle) {
        particle.update(dt);
        return particle.killed();
    });

    // outputs
    outputs.thurst = inputs.thrust;
    outputs.shoot = _player.shooting();
    outputs.explosion = _explosionTime > 0.f;

    _explosionTime -= dt;
}

void Game::draw(Canvas &canvas) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::None);
    canvas.fill();

    canvas.setBlendMode(BlendMode::Add);

    _player.draw(canvas);
    _asteroids.forEach([&] (Asteroid &asteroid) { asteroid.draw(canvas); });
    _projectiles.forEach([&] (Projectile &projectile) { projectile.draw(canvas); });
    _particles.forEach([&] (Particle &particle) { particle.draw(canvas); });

    switch (_state) {
    case Intro:
        drawTexts(canvas, "ASTEROIDS", "Press any f-key to continue");
        break;
    case Start: {
        FixedStringBuilder<16> str("LEVEL %d", _level);
        drawTexts(canvas, str, "Press any f-key to continue");
        break;
    }
    case Play:
        drawHUD(canvas);
        break;
    case Win: {
        FixedStringBuilder<16> str("Score %d", _player.score());
        drawTexts(canvas, "YOU WIN!", str);
        break;
    }
    case Lose: {
        FixedStringBuilder<16> str("Score %d", _player.score());
        drawTexts(canvas, "YOU LOSE!", str);
        break;
    }
    }
}

void Game::spawnAsteroid(uint8_t category, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity) {
    Asteroid *asteroid = _asteroids.allocate();
    if (asteroid) {
        uint8_t shape = rng.nextRange(asteroidShapes.size());
        asteroid->spawn(category, shape, position, velocity, angle, angleVelocity);
    }
}

void Game::spawnRandomAsteroid(uint8_t category) {
    while (true) {
        Vec2 position = randomVec2() * Vec2(ScreenWidth, ScreenHeight);
        if ((position - _player.position()).length() < 30.f) {
            continue;
        }
        Vec2 velocity = randomVec2() * 50.f - 25.f;
        float angle = randomFloat() * TwoPi;
        float angleVelocity = randomFloat() * 10.f - 5.f;

        spawnAsteroid(category, position, velocity, angle, angleVelocity);
        break;
    }
}

void Game::spawnProjectile(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity) {
    Projectile *projectile = _projectiles.allocate();
    if (projectile) {
        projectile->spawn(position, direction, velocity);
    }
}

void Game::spawnParticle(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Particle::Params &params) {
    Particle *particle = _particles.allocate();
    if (particle) {
        particle->spawn(position, direction, velocity, params);
    }
}

void Game::spawnExplosion(const Vec2 &position, const Vec2 &velocity) {
    for (size_t i = 0; i < 10; ++i) {
        Vec2 direction = randomDirection();
        spawnParticle(position, direction, velocity + direction * 50.f, explosionParticleParams);
    }

    _explosionTime = 0.05f;
}

void Game::divideAsteroid(Asteroid &asteroid) {
    float theta = randomFloat() * TwoPi;
    const size_t count = 3;
    for (size_t i = 0; i < count; ++i) {
        Vec2 direction = directionFromAngle(theta + float(i) / count * TwoPi);
        Vec2 velocity = direction * 25.f;
        float angle = randomFloat() * TwoPi;
        float angleVelocity = randomFloat() * 10.f - 5.f;
        spawnAsteroid(asteroid.category() - 1, asteroid.position() + direction * 0.5f * asteroid.radius(), asteroid.velocity() + velocity, angle, angleVelocity);
    }
}

void Game::drawTexts(Canvas &canvas, const char *title, const char *msg) {
    canvas.setFont(Font::Small);
    drawShadowText(canvas, (ScreenWidth - canvas.textWidth(title)) / 2, 30, Color::Bright, title);

    canvas.setFont(Font::Tiny);
    drawShadowText(canvas, (ScreenWidth - canvas.textWidth(msg)) / 2, 44, Color::Medium, msg);
}

void Game::drawHUD(Canvas &canvas) {
    canvas.setFont(Font::Tiny);

    FixedStringBuilder<16> level("Level %d", _level);
    drawShadowText(canvas, 2, 7, Color::Bright, level);

    FixedStringBuilder<16> score("Score %d", _player.score());
    drawShadowText(canvas, ScreenWidth - 2 - canvas.textWidth(score), 7, Color::Bright, score);
}

void Game::drawShadowText(Canvas &canvas, int x, int y, Color color, const char *str) {
    canvas.setBlendMode(BlendMode::Sub);
    canvas.setColor(Color::Medium);

    for (int dx = -1; dx <= 1; dx += 1) {
        for (int dy = -1; dy <= 1; dy += 1) {
            canvas.drawText(x + dx, y + dy, str);
        }
    }

    canvas.setBlendMode(BlendMode::Add);
    canvas.setColor(color);
    canvas.drawText(x, y, str);
}

void Game::initAsteroidShapes() {
    for (auto &shape : asteroidShapes) {
        for (size_t i = 0; i < shape.size(); ++i) {
            float theta = float(i) / shape.size() * TwoPi;
            float d = 3.f + randomFloat() * 5.f;
            shape[i].set(std::sin(theta) * d, std::cos(theta) * d);
        }
    }
}

} // namespace asteroids
