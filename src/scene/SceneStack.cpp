#include "scene/SceneStack.h"

#include "scene/Scene.h"

namespace hollow {

// Ctor/dtor out-of-line so std::unique_ptr<Scene>'s deleter only needs the
// complete Scene type here, not in every TU that includes SceneStack.h.
SceneStack::SceneStack()  = default;
SceneStack::~SceneStack() = default;

void SceneStack::push(std::unique_ptr<Scene> scene)
{
    if (m_flushing) {
        scene->onEnter();
        m_stack.push_back(std::move(scene));
    } else {
        m_pending.push_back({ true, std::move(scene) });
    }
}

void SceneStack::pop()
{
    if (m_flushing) {
        if (!m_stack.empty()) {
            m_stack.back()->onExit();
            m_stack.pop_back();
        }
    } else {
        m_pending.push_back({ false, nullptr });
    }
}

void SceneStack::flushPending()
{
    m_flushing = true;
    for (auto& cmd : m_pending) {
        if (cmd.isPush) {
            cmd.scene->onEnter();
            m_stack.push_back(std::move(cmd.scene));
        } else {
            if (!m_stack.empty()) {
                m_stack.back()->onExit();
                m_stack.pop_back();
                // Notify the newly-exposed scene that it's on top again.
                if (!m_stack.empty()) {
                    m_stack.back()->onEnter();
                }
            }
        }
    }
    m_pending.clear();
    m_flushing = false;
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
    flushPending();
}

void SceneStack::render(sf::RenderTarget& target)
{
    if (!m_stack.empty()) {
        m_stack.back()->render(target);
    }
}

} // namespace hollow
