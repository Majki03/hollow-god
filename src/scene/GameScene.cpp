#include "scene/GameScene.h"

#include "audio/AudioSystem.h"
#include "core/SceneContext.h"
#include "data/DataStore.h"
#include "entity/weapon/BowWeapon.h"
#include "entity/weapon/SpearWeapon.h"
#include "entity/weapon/SwordWeapon.h"
#include "render/PostProcess.h"
#include "entity/Archer.h"
#include "entity/Brute.h"
#include "entity/Charger.h"
#include "entity/EnemyBase.h"
#include "entity/Grunt.h"
#include "entity/Player.h"
#include "entity/Projectile.h"
#include "hud/Hud.h"
#include "input/ActionMap.h"
#include "physics/Collision.h"
#include "scene/BoonSelectionScene.h"
#include "scene/CurseSelectionScene.h"
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

GameScene::GameScene(SceneContext& ctx, WeaponType weaponType)
    : Scene(ctx)
    , m_room(kRoomOrigin, kRoomSize)
    , m_hud(ctx)
    , m_rng(std::random_device{}())
{
    m_weaponType = weaponType;

    auto player = std::make_unique<Player>(
        sf::Vector2f(640.f, 360.f), ctx.input, ctx.actions);
    m_player = player.get();

    // Set the chosen weapon and seed PlayerStats from its base values so that
    // boon accumulation works on top of whatever the weapon brings.
    switch (m_weaponType) {
    case WeaponType::Sword: {
        const auto& s = ctx.data.weapons.sword;
        player->setWeapon(std::make_unique<SwordWeapon>(s));
        player->mutableStats().swingDamage = s.baseDamage;
        player->mutableStats().knockback   = s.knockback;
        break;
    }
    case WeaponType::Bow: {
        const auto& b = ctx.data.weapons.bow;
        player->setWeapon(std::make_unique<BowWeapon>(b));
        // Bow owns its own damage per-shot; swingDamage boons won't apply.
        player->mutableStats().swingDamage = 0;
        player->mutableStats().knockback   = 0.f;
        break;
    }
    case WeaponType::Spear: {
        const auto& sp = ctx.data.weapons.spear;
        player->setWeapon(std::make_unique<SpearWeapon>(sp));
        player->mutableStats().swingDamage = sp.baseDamage;
        player->mutableStats().knockback   = sp.knockback;
        break;
    }
    }

    m_world.add(std::move(player));
    spawnWave();
}

void GameScene::spawnGrunt(sf::Vector2f pos, float hpScale)
{
    auto e = std::make_unique<Grunt>(pos, m_ctx.data.enemies.grunt);
    if (hpScale != 1.f) e->scaleHp(hpScale);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

void GameScene::spawnCharger(sf::Vector2f pos, float hpScale)
{
    auto e = std::make_unique<Charger>(pos, m_ctx.data.enemies.charger);
    if (hpScale != 1.f) e->scaleHp(hpScale);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

void GameScene::spawnBrute(sf::Vector2f pos, float hpScale)
{
    auto e = std::make_unique<Brute>(pos, m_ctx.data.enemies.brute);
    if (hpScale != 1.f) e->scaleHp(hpScale);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

void GameScene::spawnArcher(sf::Vector2f pos, float hpScale)
{
    auto e = std::make_unique<Archer>(pos, m_ctx.data.enemies.archer);
    if (hpScale != 1.f) e->scaleHp(hpScale);
    m_archers.push_back(e.get());
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

void GameScene::spawnWave()
{
    ++m_wave;

    // Wave composition read from data/waves.json; waves beyond the table repeat
    // the last row, same as before — the data just lives in a file now.
    const auto& layouts = m_ctx.data.waves.layouts;
    const auto& layout  = layouts[std::min(
        m_wave - 1, static_cast<int>(layouts.size()) - 1)];

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

    const float scale = m_ctx.data.waves.hpScalePerTwoWaves;
    const float hpScale = 1.f + scale * static_cast<float>((m_wave - 1) / 2);

    for (int i = 0; i < layout.grunts;   ++i) spawnGrunt  (pickPos(), hpScale);
    for (int i = 0; i < layout.chargers; ++i) spawnCharger(pickPos(), hpScale);
    for (int i = 0; i < layout.brutes;   ++i) spawnBrute  (pickPos(), hpScale);
    for (int i = 0; i < layout.archers;  ++i) spawnArcher (pickPos(), hpScale);
}

void GameScene::onEnter()
{
    // Called whenever GameScene resurfaces (a pushed scene just popped).
    // Two-stage sequence: CurseSelectionScene → BoonSelectionScene → spawnWave.
    if (m_cursePending) {
        // Curse scene just closed; now push the boon scene (Rare guaranteed
        // if the player accepted a curse, i.e. m_rareGuaranteed == true).
        m_cursePending = false;
        m_boonPending  = true;
        m_ctx.scenes.push(
            std::make_unique<BoonSelectionScene>(m_ctx, *m_player, m_wave, m_rareGuaranteed));
    } else if (m_boonPending) {
        // Boon scene closed; reset flags and start the next wave.
        m_boonPending    = false;
        m_rareGuaranteed = false;
        spawnWave();
        m_ctx.audio.play(Sfx::WaveStart);
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

    // Hit-stop: briefly freeze the simulation on a kill for impact weight.
    if (m_hitStop > 0.f) {
        m_hitStop -= dt;
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

    // New swing this frame — sound plays at animation start, before hitbox opens.
    if (m_player->swingId() != m_prevSwingId) {
        m_prevSwingId = m_player->swingId();
        m_ctx.audio.play(Sfx::Swing);

        // Spectral Volley: every 5th swing auto-fires a spirit projectile.
        if (m_player->stats().hasSpectralVolley) {
            ++m_spectralCount;
            if (m_spectralCount >= 5) {
                m_spectralCount = 0;
                const float angle = m_player->aimAngle();
                const sf::Vector2f dir{ std::cos(angle), std::sin(angle) };
                auto proj = std::make_unique<Projectile>(
                    m_player->position(), dir, 500.f,
                    m_player->stats().swingDamage, false,
                    sf::Color(160, 80, 220), 6.f);
                m_playerProjectiles.push_back(proj.get());
                m_world.add(std::move(proj));
            }
        }
    }

    // Dash trail particles + Void Rush AoE.
    if (m_player->consumeDashEvent()) {
        m_ctx.audio.play(Sfx::Dash);
        if (m_player->stats().hasVoidRush) applyVoidRush();
        const sf::Color trailColor(200, 195, 230);
        constexpr int kTrailCount = 8;
        std::uniform_real_distribution<float> rAngle(0.f, 6.2832f);
        std::uniform_real_distribution<float> rSpeed(30.f, 110.f);
        std::uniform_real_distribution<float> rLife(0.12f, 0.28f);
        for (int i = 0; i < kTrailCount; ++i) {
            const float a = rAngle(m_rng), s = rSpeed(m_rng), l = rLife(m_rng);
            Particle p;
            p.pos     = m_player->dashOrigin();
            p.vel     = { std::cos(a) * s, std::sin(a) * s };
            p.life    = l;
            p.maxLife = l;
            p.color   = trailColor;
            m_particles.push_back(p);
        }
    }

    // Poll archers for pending shots and spawn projectiles.
    for (Archer* a : m_archers) {
        if (!a->alive()) continue;
        if (auto dir = a->pendingShot()) {
            auto proj = std::make_unique<Projectile>(a->position(), *dir);
            m_projectiles.push_back(proj.get());
            m_world.add(std::move(proj));
        }
    }

    // Poll weapon for player-fired projectiles (bow shots, thrown spear).
    if (auto shot = m_player->pendingWeaponProjectile()) {
        auto proj = std::make_unique<Projectile>(
            m_player->position(), shot->direction, shot->speed,
            shot->damage, shot->pierces);
        m_playerProjectiles.push_back(proj.get());
        m_world.add(std::move(proj));
    }

    resolveCombat();
    resolveEnemyContact();
    resolveProjectiles();
    resolvePlayerProjectiles();

    // Trigger shake if the player just took a hit.
    if (!m_player->alive() || m_player->hp() < m_prevHp) {
        m_shakeTrauma = std::min(m_shakeTrauma + 0.7f, 1.f);
    }
    m_prevHp = m_player->hp();
    m_shakeTrauma = std::max(0.f, m_shakeTrauma - 2.2f * dt);
    m_ctx.post.setTrauma(m_shakeTrauma);

    m_hud.update(*m_player, m_wave, m_kills);

    if (!m_player->alive()) {
        m_ctx.audio.play(Sfx::PlayerDeath);
        m_ctx.scenes.push(std::make_unique<DeathScene>(m_ctx, m_wave, m_kills));
        return;
    }

    // Null Death Mark target while entities are still alive in memory (before
    // pruneDead destroys them — after that the raw pointer would be dangling).
    if (m_deathMarkTarget && !m_deathMarkTarget->alive()) {
        m_deathMarkTarget = nullptr;
        m_deathMarkCount  = 0;
    }

    std::erase_if(m_enemies,          [](const EnemyBase*  e) { return !e->alive(); });
    std::erase_if(m_archers,          [](const Archer*     a) { return !a->alive(); });
    std::erase_if(m_projectiles,      [](const Projectile* p) { return !p->alive(); });
    std::erase_if(m_playerProjectiles,[](const Projectile* p) { return !p->alive(); });
    m_world.pruneDead();

    if (m_enemies.empty() && !m_boonPending && !m_cursePending) {
        if (m_wave >= m_ctx.data.waves.victoryWave) {
            m_ctx.scenes.push(
                std::make_unique<VictoryScene>(m_ctx, m_wave, m_player->hp(), m_kills));
        } else if (m_wave % 3 == 0) {
            // Every third wave: curse offer first, then boon (Rare guaranteed on accept).
            m_cursePending = true;
            m_ctx.scenes.push(
                std::make_unique<CurseSelectionScene>(m_ctx, *m_player, m_rareGuaranteed));
        } else {
            // Every other wave: boon immediately.
            m_boonPending = true;
            m_ctx.scenes.push(
                std::make_unique<BoonSelectionScene>(m_ctx, *m_player, m_wave, m_rareGuaranteed));
        }
    }
}

void GameScene::handleKill(EnemyBase* e)
{
    ++m_kills;
    m_hitStop = std::max(m_hitStop, 0.055f);
    emitDeathParticles(e->position(), e->normalColor());
    m_ctx.audio.play(Sfx::EnemyDeath);
    if (m_player->stats().onKillHeal > 0)
        m_player->healBy(m_player->stats().onKillHeal);
    if (m_player->stats().hasSoulDrain)
        m_player->extendIframes(2.0f);
}

void GameScene::applyVoidRush()
{
    constexpr float kRadius = 40.f;
    const sf::Vector2f origin = m_player->dashOrigin();
    const int dmg = m_player->stats().voidRushDamage;

    for (EnemyBase* e : m_enemies) {
        if (!e->alive()) continue;
        const sf::Vector2f d = e->position() - origin;
        if (d.x * d.x + d.y * d.y <= kRadius * kRadius) {
            e->damage(dmg);
            if (!e->alive()) handleKill(e);
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
            int dmg = static_cast<int>(
                m_player->stats().swingDamage * m_player->weaponDamageMult());
            e->damage(dmg);
            e->setLastHitSwing(swing);

            sf::Vector2f dir = e->position() - center;
            const float  d2  = dir.x * dir.x + dir.y * dir.y;
            if (d2 > 0.f) {
                dir *= 1.f / std::sqrt(d2);
                e->applyImpulse(dir * m_player->stats().knockback);
            }

            // Death Mark: 3rd consecutive hit on the same target deals 3× total.
            if (m_player->stats().hasDeathMark && e->alive()) {
                if (e == m_deathMarkTarget) {
                    ++m_deathMarkCount;
                } else {
                    m_deathMarkTarget = e;
                    m_deathMarkCount  = 1;
                }
                if (m_deathMarkCount >= 3) {
                    e->damage(dmg * 2); // +2× extra so total = 3×
                    m_deathMarkCount = 0;
                }
            }

            if (!e->alive()) handleKill(e);
            else             m_ctx.audio.play(Sfx::Hit);
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

void GameScene::resolveProjectiles()
{
    for (Projectile* p : m_projectiles) {
        if (!p->alive()) continue;
        if (physics::circlesOverlap(m_player->position(), kPlayerR,
                                    p->position(),        p->radius())) {
            if (m_player->damage(Player::kContactDamage))
                m_ctx.audio.play(Sfx::PlayerHit);
            p->kill();
        }
    }
}

void GameScene::resolvePlayerProjectiles()
{
    for (Projectile* p : m_playerProjectiles) {
        if (!p->alive()) continue;
        for (EnemyBase* e : m_enemies) {
            if (!e->alive()) continue;
            if (physics::circlesOverlap(p->position(), p->radius(),
                                        e->position(),  e->radius())) {
                e->damage(p->damage());

                sf::Vector2f dir = e->position() - p->position();
                const float  d2  = dir.x * dir.x + dir.y * dir.y;
                if (d2 > 0.f) {
                    dir *= 1.f / std::sqrt(d2);
                    e->applyImpulse(dir * m_player->stats().knockback);
                }

                if (!e->alive()) handleKill(e);
                else             m_ctx.audio.play(Sfx::Hit);

                if (!p->pierces()) {
                    p->kill();
                    break;
                }
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
            if (m_player->damage(Player::kContactDamage))
                m_ctx.audio.play(Sfx::PlayerHit);
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
