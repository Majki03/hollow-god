#include "data/DataStore.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace hollow {

namespace {

using json = nlohmann::json;

json openJson(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("cannot open: " + path);
    // Throw on parse error so the caller can catch and fall back to defaults.
    return json::parse(f);
}

BoonSchool parseSchool(const std::string& s)
{
    if (s == "Vigor")     return BoonSchool::Vigor;
    if (s == "Wrath")     return BoonSchool::Wrath;
    if (s == "Swiftness") return BoonSchool::Swiftness;
    if (s == "Dominion")  return BoonSchool::Dominion;
    throw std::runtime_error("unknown boon school: " + s);
}

BoonEffect parseEffect(const std::string& s)
{
    if (s == "MaxHpUp")          return BoonEffect::MaxHpUp;
    if (s == "HealNow")          return BoonEffect::HealNow;
    if (s == "DmgReduceUp")      return BoonEffect::DmgReduceUp;
    if (s == "SwingDmgUp")       return BoonEffect::SwingDmgUp;
    if (s == "KnockbackUp")      return BoonEffect::KnockbackUp;
    if (s == "MoveSpeedUp")      return BoonEffect::MoveSpeedUp;
    if (s == "IFrameUp")         return BoonEffect::IFrameUp;
    if (s == "OnKillHealUp")     return BoonEffect::OnKillHealUp;
    if (s == "DashCooldownDown") return BoonEffect::DashCooldownDown;
    if (s == "DashDistUp")       return BoonEffect::DashDistUp;
    throw std::runtime_error("unknown boon effect: " + s);
}

void loadEnemies(DataStore& store, const std::string& dir)
{
    try {
        auto j = openJson(dir + "/enemies.json");

        auto& g = store.enemies.grunt;
        g.radius    = j["grunt"].value("radius",    g.radius);
        g.maxHp     = j["grunt"].value("maxHp",     g.maxHp);
        g.moveSpeed = j["grunt"].value("moveSpeed", g.moveSpeed);

        auto& ch = store.enemies.charger;
        ch.radius      = j["charger"].value("radius",      ch.radius);
        ch.maxHp       = j["charger"].value("maxHp",       ch.maxHp);
        ch.stalkSpeed  = j["charger"].value("stalkSpeed",  ch.stalkSpeed);
        ch.chargeSpeed = j["charger"].value("chargeSpeed", ch.chargeSpeed);
        ch.windupDur   = j["charger"].value("windupDur",   ch.windupDur);
        ch.restDur     = j["charger"].value("restDur",     ch.restDur);
        ch.chargeDist  = j["charger"].value("chargeDist",  ch.chargeDist);

        auto& br = store.enemies.brute;
        br.radius    = j["brute"].value("radius",    br.radius);
        br.maxHp     = j["brute"].value("maxHp",     br.maxHp);
        br.moveSpeed = j["brute"].value("moveSpeed", br.moveSpeed);

        auto& ar = store.enemies.archer;
        ar.radius        = j["archer"].value("radius",        ar.radius);
        ar.maxHp         = j["archer"].value("maxHp",         ar.maxHp);
        ar.moveSpeed     = j["archer"].value("moveSpeed",     ar.moveSpeed);
        ar.preferredDist = j["archer"].value("preferredDist", ar.preferredDist);
        ar.fireInterval  = j["archer"].value("fireInterval",  ar.fireInterval);
    } catch (const std::exception& e) {
        std::cerr << "[DataStore] enemies.json: " << e.what() << " — using defaults\n";
    }
}

void loadWaves(DataStore& store, const std::string& dir)
{
    try {
        auto j = openJson(dir + "/waves.json");

        store.waves.victoryWave        = j.value("victoryWave",        store.waves.victoryWave);
        store.waves.hpScalePerTwoWaves = j.value("hpScalePerTwoWaves", store.waves.hpScalePerTwoWaves);

        if (j.contains("layouts") && j["layouts"].is_array()) {
            store.waves.layouts.clear();
            for (const auto& entry : j["layouts"]) {
                WaveLayout w;
                w.grunts   = entry.value("grunts",   0);
                w.chargers = entry.value("chargers", 0);
                w.brutes   = entry.value("brutes",   0);
                w.archers  = entry.value("archers",  0);
                store.waves.layouts.push_back(w);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataStore] waves.json: " << e.what() << " — using defaults\n";
    }
}

void loadBoons(DataStore& store, const std::string& dir)
{
    try {
        auto j = openJson(dir + "/boons.json");
        if (!j.is_array()) throw std::runtime_error("expected a JSON array");

        store.boons.clear();
        for (const auto& entry : j) {
            Boon b;
            b.name    = entry.at("name").get<std::string>();
            b.tagline = entry.at("tagline").get<std::string>();
            b.school  = parseSchool(entry.at("school").get<std::string>());
            b.effect  = parseEffect(entry.at("effect").get<std::string>());
            b.value   = entry.at("value").get<float>();
            store.boons.push_back(std::move(b));
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataStore] boons.json: " << e.what() << " — using defaults\n";
        store.boons.clear();
    }
}

void loadWeapons(DataStore& store, const std::string& dir)
{
    try {
        auto j = openJson(dir + "/weapons.json");

        auto& sw = store.weapons.sword;
        if (j.contains("sword")) {
            const auto& s = j["sword"];
            sw.swingDuration = s.value("swingDuration", sw.swingDuration);
            sw.hitboxRadius  = s.value("hitboxRadius",  sw.hitboxRadius);
            sw.hitboxReach   = s.value("hitboxReach",   sw.hitboxReach);
            sw.baseDamage    = s.value("damage",         sw.baseDamage);
            sw.knockback     = s.value("knockback",      sw.knockback);
            if (s.contains("heavySlam")) {
                const auto& h = s["heavySlam"];
                sw.heavyRadiusMult = h.value("radiusMult",  sw.heavyRadiusMult);
                sw.heavyDamageMult = h.value("damageMult",  sw.heavyDamageMult);
                sw.heavyChargeTime = h.value("chargeTime",  sw.heavyChargeTime);
            }
        }

        auto& bw = store.weapons.bow;
        if (j.contains("bow")) {
            const auto& b = j["bow"];
            bw.projectileSpeed  = b.value("projectileSpeed",  bw.projectileSpeed);
            bw.cooldown         = b.value("cooldown",         bw.cooldown);
            bw.chargeTime       = b.value("chargeTime",       bw.chargeTime);
            bw.chargeDamageMult = b.value("chargeDamageMult", bw.chargeDamageMult);
            bw.baseDamage       = b.value("damage",            bw.baseDamage);
        }

        auto& sp = store.weapons.spear;
        if (j.contains("spear")) {
            const auto& s = j["spear"];
            sp.hitboxLength    = s.value("hitboxLength",    sp.hitboxLength);
            sp.hitboxWidth     = s.value("hitboxWidth",     sp.hitboxWidth);
            sp.thrustDuration  = s.value("thrustDuration",  sp.thrustDuration);
            sp.throwSpeed      = s.value("throwSpeed",      sp.throwSpeed);
            sp.unarmedDuration = s.value("unarmedDuration", sp.unarmedDuration);
            sp.baseDamage      = s.value("damage",           sp.baseDamage);
            sp.knockback       = s.value("knockback",        sp.knockback);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DataStore] weapons.json: " << e.what() << " — using defaults\n";
    }
}

} // namespace

void DataStore::load(const std::string& dataDir)
{
    loadEnemies(*this, dataDir);
    loadWaves(*this, dataDir);
    loadBoons(*this, dataDir);
    loadWeapons(*this, dataDir);
}

} // namespace hollow
