#pragma once

namespace hollow {

struct PlayerStats {
    // Movement
    float moveSpeed    = 260.f;
    float dashDist     = 140.f;
    float dashCooldown = 0.80f;

    // Durability
    int   maxHp       = 100;
    float iframeDur   = 0.75f;
    int   dmgReduce   = 0;

    // Combat — read by GameScene when resolving swings, knockback, and
    // projectile damage for melee weapons.
    int   swingDamage = 10;
    float knockback   = 420.f;

    // On-kill effect
    int   onKillHeal  = 0;

    // Rare boon flags — each is set once when the boon is applied;
    // the active logic lives in Player::update() or GameScene.
    bool  hasVoidRush       = false;
    int   voidRushDamage    = 0;    // AoE damage dealt on dash (40px radius)
    bool  hasDeathMark      = false;
    bool  hasSpectralVolley = false;
    bool  hasEchoStep       = false;
    bool  hasSoulDrain      = false;
};

} // namespace hollow
