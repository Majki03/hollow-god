#include "scene/SceneStack.h"

#include "scene/Scene.h"

namespace hollow {

// Ctor/dtor out-of-line so std::unique_ptr<Scene>'s deleter only needs the
// complete Scene type here, not in every TU that includes SceneStack.h.
SceneStack::SceneStack()  = default;
SceneStack::~SceneStack() = default;

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
