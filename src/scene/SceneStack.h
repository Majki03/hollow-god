#pragma once

#include <memory>
#include <vector>

namespace sf {
class RenderTarget;
class Event;
} // namespace sf

namespace hollow {

class Scene;

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
    std::vector<std::unique_ptr<Scene>> m_stack;
};

} // namespace hollow
