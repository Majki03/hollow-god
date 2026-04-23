#pragma once

#include <string_view>

namespace hollow {

// The four Divine Echo schools. Each shapes a different aspect of combat.
enum class BoonSchool {
    Vigor,      // survivability — HP, healing, damage reduction
    Wrath,      // offense — damage output and knockback
    Swiftness,  // mobility — movement speed and i-frames
    Dominion,   // special — on-kill effects and unique modifiers
};

enum class BoonEffect {
    MaxHpUp,        // +value to PlayerStats::maxHp (also heals that amount)
    HealNow,        // instantly restore value HP
    DmgReduceUp,    // +value to PlayerStats::dmgReduce
    SwingDmgUp,     // +value to PlayerStats::swingDamage
    KnockbackUp,    // +value to PlayerStats::knockback
    MoveSpeedUp,    // +value to PlayerStats::moveSpeed
    IFrameUp,       // +value to PlayerStats::iframeDur
    OnKillHealUp,   // +value to PlayerStats::onKillHeal
};

struct Boon {
    std::string_view name;
    std::string_view tagline;   // short description for the selection card
    BoonSchool       school;
    BoonEffect       effect;
    float            value;     // magnitude; cast to int where the stat is int
};

} // namespace hollow
