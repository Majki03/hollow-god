#include "scene/SceneStack.h"

#include "scene/Scene.h"

namespace hollow {

void SceneStack::push(std::unique_ptr<Scene> scene)
{
    scene->onEnter();
    m_stack.push_back(std::move(scene));
}

void SceneStack::pop()
{
    if (m_stack.empty()) {
        return;
    }
    m_stack.back()->onExit();
    m_stack.pop_back();
}

void SceneStack::handleEvent(const sf::Event& event)
{
    if (!m_stack.empty()) {
        m_stack.back()->handleEvent(event);
    }
}

void SceneStack::update(float dt)
{
    if (!m_stack.empty()) {
        m_stack.back()->update(dt);
    }
}

void SceneStack::render(sf::RenderTarget& target)
{
    if (!m_stack.empty()) {
        m_stack.back()->render(target);
    }
}

} // namespace hollow
