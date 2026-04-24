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

    // Owned boon strings — pointers into this vector remain stable after load()
    // because the vector is never modified again at runtime.
    std::vector<Boon> boons;

    // Load JSON from dataDir/{enemies,waves,boons}.json.
    // Logs to stderr on parse failure and keeps defaults for that section.
    void load(const std::string& dataDir);
};

} // namespace hollow
