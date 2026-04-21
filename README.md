# The Hollow God

> *Creation has appetite. Your worshippers devoured you. Climb back out.*

A top-down action roguelike in the vein of *Hades*. You are a Shard — a fragment
of a consumed deity — waking in the abyss with no memory of what you were.
Fight through three layers of the Hollow, reclaim your divine fragments, and
face the terrible truth: you may have deserved to be eaten.

## Tech stack

- **Language:** C++20
- **Graphics / windowing / audio:** [SFML 2.6](https://www.sfml-dev.org/)
- **Build:** CMake 3.22+ (SFML is pulled in via `FetchContent`, no system install required)
- **Platforms:** macOS, Linux, Windows

## Build

```sh
cmake -S . -B build
cmake --build build -j
./build/hollow_god
```

The first configure takes a minute or two — CMake clones and builds SFML.
Subsequent builds are incremental.

## Controls

| Action | Key |
| --- | --- |
| Quit  | `Esc` |

More to come as gameplay comes online.

## Roadmap

- [x] **Phase 1 — Foundation.** CMake, SFML, window, fixed-timestep loop.
- [ ] **Phase 2 — Core engine.** Input, resource manager, scene stack, entities.
- [ ] **Phase 3 — Combat prototype.** Player, movement, attacks, a dummy enemy.
- [ ] **Phase 4 — Rooms & floors.** Room templates, procedural layout, transitions.
- [ ] **Phase 5 — AI & enemies.** State machines, archetypes, first boss.
- [ ] **Phase 6 — Boons & meta-progression.** Divine Echoes, memory fragments, hub.
- [ ] **Phase 7 — Content pass.** Three biomes, three bosses, dialogue, lore.
- [ ] **Phase 8 — Polish.** Audio, VFX, HUD, tuning, release pipeline.

## License

TBD.
