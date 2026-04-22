#pragma once

namespace sf {
class RenderTarget;
class Event;
} // namespace sf

namespace hollow {

struct SceneContext;

// A Scene is one screen-level state: title, gameplay, pause overlay, etc.
// The SceneStack owns Scenes and pumps the active one each frame. Scenes get
// their services through the SceneContext reference handed in at construction.
class Scene {
public:
    explicit Scene(SceneContext& ctx) : m_ctx(ctx) {}
    virtual ~Scene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;

protected:
    SceneContext& m_ctx;
};

} // namespace hollow
