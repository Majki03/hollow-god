#pragma once

#include <string>

namespace hollow {

enum class BoonSchool {
    Vigor,      // survivability — HP, healing, damage reduction
    Wrath,      // offense — damage output and knockback
    Swiftness,  // mobility — movement speed and i-frames
    Dominion,   // special — on-kill effects and unique modifiers
};

enum class BoonTier { Common, Rare };

enum class BoonEffect {
    // Common effects — modify a single PlayerStats field.
    MaxHpUp,           // +value to PlayerStats::maxHp (also heals that amount)
    HealNow,           // instantly restore value HP
    DmgReduceUp,       // +value to PlayerStats::dmgReduce
    SwingDmgUp,        // +value to PlayerStats::swingDamage
    KnockbackUp,       // +value to PlayerStats::knockback
    MoveSpeedUp,       // +value to PlayerStats::moveSpeed
    IFrameUp,          // +value to PlayerStats::iframeDur
    OnKillHealUp,      // +value to PlayerStats::onKillHeal
    DashCooldownDown,  // -value to PlayerStats::dashCooldown (faster recharge)
    DashDistUp,        // +value to PlayerStats::dashDist

    // Rare effects — set a flag in PlayerStats; active logic lives in GameScene/Player.
    VoidRush,          // dash deals value AoE damage in 40px radius at origin
    DeathMark,         // 3rd consecutive hit on same enemy deals 3× damage
    SpectralVolley,    // every 5th swing auto-fires a spirit projectile
    BloodPact,         // −5 max HP; +value swing damage
    EchoStep,          // each dash grants 2 s of +50% move speed
    SoulDrain,         // kills restore 2 s of i-frames
};

struct Boon {
    std::string name;
    std::string tagline;
    BoonSchool  school;
    BoonEffect  effect;
    float       value;
    BoonTier    tier = BoonTier::Common;
};

} // namespace hollow
