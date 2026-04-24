#pragma once

namespace hollow {

// Runtime-mutable player statistics. Starts at baseline values; boons
// accumulate changes here for the duration of a run.
struct PlayerStats {
    // Movement
    float moveSpeed    = 260.f;
    float dashDist     = 140.f;  // teleport distance per dash
    float dashCooldown = 0.80f;  // seconds between dashes

    // Durability
    int   maxHp       = 100;
    float iframeDur   = 0.75f;
    int   dmgReduce   = 0;     // flat damage reduction per hit

    // Combat — read by GameScene when resolving swings and knockback
    int   swingDamage = 10;
    float knockback   = 420.f;

    // On-kill effect
    int   onKillHeal  = 0;     // HP restored per enemy killed
};

} // namespace hollow
