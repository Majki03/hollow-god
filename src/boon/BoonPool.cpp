#include "boon/BoonPool.h"

namespace hollow {

// Three boons per school — enough variety without overwhelming the player.
// Values are tuned so each boon is clearly impactful in a short run.
static constexpr Boon kBoons[] = {
    // ── Vigor ────────────────────────────────────────────────────────────
    { "Iron Flesh",     "Max HP +25",               BoonSchool::Vigor,     BoonEffect::MaxHpUp,      25.f },
    { "Mending",        "Restore 20 HP now",         BoonSchool::Vigor,     BoonEffect::HealNow,      20.f },
    { "Stone Veil",     "Reduce damage taken by 3",  BoonSchool::Vigor,     BoonEffect::DmgReduceUp,   3.f },

    // ── Wrath ────────────────────────────────────────────────────────────
    { "Shattered Edge", "Swing damage +6",           BoonSchool::Wrath,     BoonEffect::SwingDmgUp,    6.f },
    { "Heavy Strike",   "Knockback force +120",      BoonSchool::Wrath,     BoonEffect::KnockbackUp, 120.f },
    { "Bloodlust",      "Kill heals 5 HP",           BoonSchool::Wrath,     BoonEffect::OnKillHealUp,  5.f },

    // ── Swiftness ────────────────────────────────────────────────────────
    { "Fleet Step",     "Move speed +40",            BoonSchool::Swiftness, BoonEffect::MoveSpeedUp,  40.f },
    { "Ghost Step",     "I-frames +0.30 s",          BoonSchool::Swiftness, BoonEffect::IFrameUp,      0.30f },
    { "Surge",          "Move speed +25",            BoonSchool::Swiftness, BoonEffect::MoveSpeedUp,  25.f },

    // ── Dominion ─────────────────────────────────────────────────────────
    { "Void Echo",      "Kill heals 8 HP",           BoonSchool::Dominion,  BoonEffect::OnKillHealUp,  8.f },
    { "Fracture",       "Swing damage +4",           BoonSchool::Dominion,  BoonEffect::SwingDmgUp,    4.f },
    { "Spectral Hide",  "Reduce damage taken by 2",  BoonSchool::Dominion,  BoonEffect::DmgReduceUp,   2.f },
};

std::span<const Boon> BoonPool::all()
{
    return kBoons;
}

} // namespace hollow
