#pragma once

#include <memory>
#include <vector>

namespace sf {
class RenderTarget;
class Event;
} // namespace sf

namespace hollow {

class Scene;

// Scene transitions requested inside update() are deferred until after the
// current update tick completes, so no scene ever destroys itself mid-frame.
class SceneStack {
public:
    SceneStack();
    ~SceneStack();

    void push(std::unique_ptr<Scene> scene);
    void pop();

    bool empty() const { return m_stack.empty(); }

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderTarget& target);

private:
    void flushPending();

    struct PendingPush { std::unique_ptr<Scene> scene; };
    struct PendingPop  {};
    struct Command {
        bool isPush;
        std::unique_ptr<Scene> scene; // valid only when isPush
    };

    std::vector<std::unique_ptr<Scene>> m_stack;
    std::vector<Command>                m_pending;
    bool                                m_flushing = false;
};

} // namespace hollow
