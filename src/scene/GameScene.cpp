#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "entity/Brute.h"
#include "entity/Charger.h"
#include "entity/EnemyBase.h"
#include "entity/Grunt.h"
#include "entity/Player.h"
#include "hud/Hud.h"
#include "input/ActionMap.h"
#include "physics/Collision.h"
#include "scene/BoonSelectionScene.h"
#include "scene/DeathScene.h"
#include "scene/PauseScene.h"
#include "scene/SceneStack.h"
#include "scene/VictoryScene.h"
#include "world/Room.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

namespace hollow {

namespace {
    constexpr float kPlayerR = 14.f;

    const sf::Vector2f kRoomOrigin = { 120.f, 80.f };
    const sf::Vector2f kRoomSize   = { 1040.f, 560.f };
}

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
    , m_room(kRoomOrigin, kRoomSize)
    , m_hud(ctx)
    , m_rng(std::random_device{}())
{
    auto player = std::make_unique<Player>(
        sf::Vector2f(640.f, 360.f), ctx.input, ctx.actions);
    m_player = player.get();
    m_world.add(std::move(player));

    spawnWave();
}

template<typename T>
void GameScene::spawnEnemy(sf::Vector2f position)
{
    auto e = std::make_unique<T>(position);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

// Explicit instantiations so the template body doesn't need to live in the header.
template void GameScene::spawnEnemy<Grunt>(sf::Vector2f);
template void GameScene::spawnEnemy<Charger>(sf::Vector2f);
template void GameScene::spawnEnemy<Brute>(sf::Vector2f);

void GameScene::spawnWave()
{
    ++m_wave;

    // Wave composition: each entry is {grunts, chargers, brutes}.
    // Waves beyond the table repeat the last row.
    struct WaveLayout { int grunts; int chargers; int brutes; };
    static constexpr WaveLayout kLayouts[] = {
        { 1, 0, 0 }, // wave 1
        { 1, 1, 0 }, // wave 2
        { 2, 1, 0 }, // wave 3
        { 2, 1, 1 }, // wave 4
        { 2, 2, 1 }, // wave 5+
    };
    const auto& layout = kLayouts[std::min(m_wave - 1,
        static_cast<int>(std::size(kLayouts)) - 1)];

    constexpr float kEdge    = 55.f;
    constexpr float kMinDist = 220.f;
    std::uniform_real_distribution<float> rX(
        m_room.topLeft().x + kEdge, m_room.topLeft().x + m_room.size().x - kEdge);
    std::uniform_real_distribution<float> rY(
        m_room.topLeft().y + kEdge, m_room.topLeft().y + m_room.size().y - kEdge);

    const sf::Vector2f playerPos = m_player->position();

    auto pickPos = [&]() {
        sf::Vector2f pos;
        for (int attempt = 0; attempt < 20; ++attempt) {
            pos = { rX(m_rng), rY(m_rng) };
            const sf::Vector2f d = pos - playerPos;
            if (d.x * d.x + d.y * d.y >= kMinDist * kMinDist) break;
        }
        return pos;
    };

    for (int i = 0; i < layout.grunts;   ++i) spawnEnemy<Grunt>  (pickPos());
    for (int i = 0; i < layout.chargers; ++i) spawnEnemy<Charger>(pickPos());
    for (int i = 0; i < layout.brutes;   ++i) spawnEnemy<Brute>  (pickPos());
}

void GameScene::onEnter()
{
    // Called when GameScene becomes the top of the stack again — i.e. right
    // after the BoonSelectionScene popped. Spawn the next wave here so the
    // game is never left in an empty-but-waiting state.
    if (m_boonPending) {
        m_boonPending = false;
        spawnWave();
    }
}

void GameScene::handleEvent(const sf::Event& /*event*/)
{
}

void GameScene::update(float dt)
{
    if (m_ctx.actions.justPressed(Action::Pause) ||
        m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.push(std::make_unique<PauseScene>(m_ctx));
        return;
    }

    const sf::Vector2f playerPos = m_player->position();
    for (EnemyBase* e : m_enemies) {
        if (e->alive()) e->seek(playerPos);
    }
    separateEnemies();

    m_world.update(dt);

    // Keep entities inside the walkable area (each enemy type may have a
    // different radius so we read it from the object, not a hardcoded constant).
    const sf::Vector2f pMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    const sf::Vector2f pMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    m_player->confine(pMin, pMax);

    for (EnemyBase* e : m_enemies) {
        if (!e->alive()) continue;
        const float r = e->radius();
        const sf::Vector2f eMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + r, Room::kWallThick + r);
        const sf::Vector2f eMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + r, Room::kWallThick + r);
        e->confine(eMin, eMax);
    }

    // Tick particles.
    for (Particle& p : m_particles) {
        p.pos  += p.vel * dt;
        p.life -= dt;
    }
    std::erase_if(m_particles, [](const Particle& p) { return p.life <= 0.f; });

    resolveCombat();
    resolveEnemyContact();

    // Trigger shake if the player just took a hit.
    if (!m_player->alive() || m_player->hp() < m_prevHp) {
        m_shakeTrauma = std::min(m_shakeTrauma + 0.7f, 1.f);
    }
    m_prevHp = m_player->hp();
    m_shakeTrauma = std::max(0.f, m_shakeTrauma - 2.2f * dt);

    m_hud.update(*m_player, m_wave, m_kills);

    if (!m_player->alive()) {
        m_ctx.scenes.push(std::make_unique<DeathScene>(m_ctx, m_wave, m_kills));
        return;
    }

    std::erase_if(m_enemies, [](const EnemyBase* e) { return !e->alive(); });
    m_world.pruneDead();

    if (m_enemies.empty() && !m_boonPending) {
        constexpr int kVictoryWave = 10;
        if (m_wave >= kVictoryWave) {
            m_ctx.scenes.push(
                std::make_unique<VictoryScene>(m_ctx, m_wave, m_player->hp(), m_kills));
        } else if (m_wave % 2 == 0) {
            // Offer a boon every 2 waves.
            m_boonPending = true;
            m_ctx.scenes.push(
                std::make_unique<BoonSelectionScene>(m_ctx, *m_player));
        } else {
            spawnWave();
        }
    }
}

void GameScene::emitDeathParticles(sf::Vector2f pos, sf::Color color)
{
    constexpr int   kCount    = 10;
    constexpr float kMinSpeed = 60.f;
    constexpr float kMaxSpeed = 220.f;
    constexpr float kLifeMin  = 0.25f;
    constexpr float kLifeMax  = 0.45f;

    std::uniform_real_distribution<float> rAngle(0.f, 6.2832f);
    std::uniform_real_distribution<float> rSpeed(kMinSpeed, kMaxSpeed);
    std::uniform_real_distribution<float> rLife(kLifeMin, kLifeMax);

    for (int i = 0; i < kCount; ++i) {
        const float angle = rAngle(m_rng);
        const float speed = rSpeed(m_rng);
        const float life  = rLife(m_rng);
        Particle p;
        p.pos     = pos;
        p.vel     = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.life    = life;
        p.maxLife = life;
        p.color   = color;
        m_particles.push_back(p);
    }
}

void GameScene::resolveCombat()
{
    if (!m_player->hitboxActive()) return;

    const auto  center = m_player->hitboxPosition();
    const float radius = m_player->hitboxRadius();
    const int   swing  = m_player->swingId();

    for (EnemyBase* e : m_enemies) {
        if (!e->alive() || e->lastHitSwing() == swing) continue;
        if (physics::circlesOverlap(center, radius, e->position(), e->radius())) {
            e->damage(m_player->stats().swingDamage);
            e->setLastHitSwing(swing);

            sf::Vector2f dir = e->position() - center;
            const float  d2  = dir.x * dir.x + dir.y * dir.y;
            if (d2 > 0.f) {
                dir *= 1.f / std::sqrt(d2);
                e->applyImpulse(dir * m_player->stats().knockback);
            }

            if (!e->alive()) {
                ++m_kills;
                emitDeathParticles(e->position(), e->normalColor());
                if (m_player->stats().onKillHeal > 0)
                    m_player->healBy(m_player->stats().onKillHeal);
            }
        }
    }
}

void GameScene::separateEnemies()
{
    // Push pairs of enemies apart when they overlap. Applied as a direct
    // impulse on m_knockback so the existing decay smooths it out naturally.
    constexpr float kPushStrength = 300.f;

    for (std::size_t i = 0; i < m_enemies.size(); ++i) {
        for (std::size_t j = i + 1; j < m_enemies.size(); ++j) {
            EnemyBase* a = m_enemies[i];
            EnemyBase* b = m_enemies[j];
            if (!a->alive() || !b->alive()) continue;

            sf::Vector2f delta = a->position() - b->position();
            const float  d2    = delta.x * delta.x + delta.y * delta.y;
            const float  minD  = a->radius() + b->radius();

            if (d2 < minD * minD && d2 > 0.f) {
                const float  d   = std::sqrt(d2);
                const float  pen = (minD - d) / minD; // 0→touching, 1→fully overlapping
                sf::Vector2f push = (delta / d) * (pen * kPushStrength);
                a->applyImpulse( push);
                b->applyImpulse(-push);
            }
        }
    }
}

void GameScene::resolveEnemyContact()
{
    for (EnemyBase* e : m_enemies) {
        if (!e->alive()) continue;
        if (physics::circlesOverlap(m_player->position(), kPlayerR,
                                    e->position(),         e->radius())) {
            m_player->damage(Player::kContactDamage);
        }
    }
}

void GameScene::render(sf::RenderTarget& target)
{
    // Apply screen shake by temporarily offsetting the view.
    const sf::View original = target.getView();
    if (m_shakeTrauma > 0.f) {
        constexpr float kMaxOffset = 14.f;
        std::uniform_real_distribution<float> jitter(-1.f, 1.f);
        const float mag = m_shakeTrauma * m_shakeTrauma * kMaxOffset;
        sf::View shaken = original;
        shaken.move(jitter(m_rng) * mag, jitter(m_rng) * mag);
        target.setView(shaken);
    }

    m_room.render(target);
    m_world.render(target);

    // Draw death particles (world-space, same shaken view).
    {
        sf::RectangleShape chip({ 4.f, 4.f });
        for (const Particle& p : m_particles) {
            const float alpha = p.life / p.maxLife;
            sf::Color c = p.color;
            c.a = static_cast<sf::Uint8>(alpha * 220.f);
            chip.setFillColor(c);
            chip.setPosition(p.pos - sf::Vector2f(2.f, 2.f));
            target.draw(chip);
        }
    }

    // HUD is always in screen-space — reset view before drawing it.
    target.setView(original);
    m_hud.render(target);
}

} // namespace hollow
