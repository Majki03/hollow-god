#pragma once

#include "boon/Boon.h"

#include <string>
#include <vector>

namespace hollow {

// Enemy stat defaults mirror the original hardcoded constants.
// DataStore::load() overwrites them from JSON; if the file is missing or
// malformed the game still runs with these values.

struct GruntStats {
    float radius    = 18.f;
    int   maxHp     = 20;
    float moveSpeed = 110.f;
};

struct ChargerStats {
    float radius      = 12.f;
    int   maxHp       = 10;
    float stalkSpeed  = 75.f;
    float chargeSpeed = 310.f;
    float windupDur   = 0.35f;
    float restDur     = 0.55f;
    float chargeDist  = 280.f;
};

struct BruteStats {
    float radius    = 28.f;
    int   maxHp     = 50;
    float moveSpeed = 60.f;
};

struct ArcherStats {
    float radius        = 13.f;
    int   maxHp         = 18;
    float moveSpeed     = 65.f;
    float preferredDist = 290.f;
    float fireInterval  = 2.2f;
};

// ── Weapon stats ─────────────────────────────────────────────────────────────
// Defaults mirror weapons.json; DataStore::load() overwrites from file.

struct SwordStats {
    float swingDuration   = 0.30f;
    float hitboxRadius    = 55.f;
    float hitboxReach     = 40.f;
    float heavyRadiusMult = 1.8f;
    float heavyDamageMult = 2.0f;
    float heavyChargeTime = 0.40f;
    int   baseDamage      = 20;
    float knockback       = 280.f;
};

struct BowStats {
    float projectileSpeed  = 620.f;
    float cooldown         = 0.60f;
    float chargeTime       = 1.20f;
    float chargeDamageMult = 1.5f;
    int   baseDamage       = 15;
};

struct SpearStats {
    float hitboxLength    = 80.f;
    float hitboxWidth     = 30.f;
    float thrustDuration  = 0.25f;
    float throwSpeed      = 550.f;
    float unarmedDuration = 1.5f;
    int   baseDamage      = 18;
    float knockback       = 340.f;
};

struct WaveLayout {
    int grunts   = 0;
    int chargers = 0;
    int brutes   = 0;
    int archers  = 0;
};

struct DataStore {
    struct {
        GruntStats   grunt;
        ChargerStats charger;
        BruteStats   brute;
        ArcherStats  archer;
    } enemies;

    struct {
        std::vector<WaveLayout> layouts;
        int   victoryWave        = 10;
        float hpScalePerTwoWaves = 0.25f;
    } waves;

    struct {
        SwordStats sword;
        BowStats   bow;
        SpearStats spear;
    } weapons;

    // Owned boon strings — pointers into this vector remain stable after load()
    // because the vector is never modified again at runtime.
    std::vector<Boon> boons;

    // Load JSON from dataDir/{enemies,waves,boons}.json.
    // Logs to stderr on parse failure and keeps defaults for that section.
    void load(const std::string& dataDir);
};

} // namespace hollow
