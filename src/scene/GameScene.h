#pragma once

#include "entity/weapon/Weapon.h"
#include "hud/Hud.h"
#include "scene/Scene.h"
#include "world/Room.h"
#include "world/World.h"

#include <optional>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <random>
#include <vector>

namespace hollow {

class Archer;
class EnemyBase;
class Player;
class Projectile;

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx, WeaponType weaponType = WeaponType::Sword);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    struct Particle {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float        life;
        float        maxLife;
        sf::Color    color;
    };

    void spawnGrunt  (sf::Vector2f position, float hpScale);
    void spawnCharger(sf::Vector2f position, float hpScale);
    void spawnBrute  (sf::Vector2f position, float hpScale);
    void spawnArcher (sf::Vector2f position, float hpScale);

    void spawnWave();
    void resolveCombat();
    void resolveEnemyContact();
    void resolveProjectiles();
    void resolvePlayerProjectiles();
    void separateEnemies();
    void applyVoidRush();          // Rare boon: AoE on dash origin
    void emitDeathParticles(sf::Vector2f pos, sf::Color color);
    void handleKill(EnemyBase* e); // shared on-kill logic (heal, soul drain, etc.)

    void onEnter() override;

    Room                    m_room;
    World                   m_world;
    Hud                     m_hud;
    Player*                 m_player = nullptr;
    std::vector<EnemyBase*>  m_enemies;
    std::vector<Archer*>     m_archers;
    std::vector<Projectile*> m_projectiles;        // enemy shots
    std::vector<Projectile*> m_playerProjectiles;  // player weapon shots
    std::vector<Particle>    m_particles;
    WeaponType              m_weaponType;
    int                     m_wave          = 0;
    int                     m_kills         = 0;
    bool                    m_boonPending   = false;
    bool                    m_cursePending  = false;
    bool                    m_rareGuaranteed = false;
    float                   m_shakeTrauma   = 0.f;
    float                   m_hitStop       = 0.f;
    int                     m_prevHp        = 0;
    int                     m_prevSwingId   = 0;
    // Rare boon active state
    int                     m_spectralCount = 0;  // swings since last Spectral Volley
    EnemyBase*              m_deathMarkTarget  = nullptr;
    int                     m_deathMarkCount   = 0;
    std::mt19937            m_rng;
};

} // namespace hollow
