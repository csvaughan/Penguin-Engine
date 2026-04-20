#pragma once
#include "Panel.h"
#include "Renderer/Sprite.h"
#include "Animation/Animator.h"
#include "Renderer/Renderer.h"
#include "UICanvas.h"

namespace pgn::GUI
{
    class ImageBox : public GUIElement
    {
    public:
        ImageBox(UICanvas* canvas, Vector2 position, Vector2 size, GUIElement* parent = nullptr, const std::string& name = "Image") 
            : GUIElement(canvas, position, size, name, parent){}
        ~ImageBox(){}

        void SetImage(Ref<Texture> texture) { m_sprite.setTexture(texture); }

    private:

        void OnUpdate(Timestep ts) override
        {
            m_sprite.setScale(GetLocalSize());
            GUIElement::OnUpdate(ts);
        }

        void OnRender(float alpha) override
        {
            if (!m_visible) return;

            float scale = m_canvas->GetScaleFactor();
            Vector2 topLeft = GetScreenPos();
            
            m_sprite.setPosition(topLeft);
            
            // Calculate scale ratio: (Target Size * Canvas Scale) / Original Texture Size
            Vector2 texSize = m_sprite.getTextureRect().getSize();
            m_sprite.setScale({ (m_bounds.w * scale) / texSize.x, (m_bounds.h * scale) / texSize.y });

            Renderer::Submit(m_sprite);
            GUIElement::OnRender(alpha);
        }

    private:
        Sprite m_sprite;
    };

    class AnimatedImageBox : public GUIElement
    {
    public:
        AnimatedImageBox(UICanvas* canvas, Vector2 position, Vector2 size, GUIElement* parent = nullptr, const std::string& name = "AnimatedImage") 
            : GUIElement(canvas, position, size, name, parent){}
        ~AnimatedImageBox(){}

        void SetAnimation(Ref<Animation> ani) 
        {
            m_animation = ani; 
            SetSize(m_animation->getFrameSize());
        }

        void PlayAnimation() { m_animator.resetAnimation(); m_animator.play(m_animation); }

    private:

        void OnUpdate(Timestep ts) override
        {
            if (!m_enabled) return;
            
            m_animator.update(ts, m_sprite);
            
            GUIElement::OnUpdate(ts);
        }

        void OnRender(float alpha) override
        {
            if (!m_visible) return;

            if(m_animation->getLoopMode() == LoopMode::Single && m_animator.isFinished()) { return;}

            float scale = m_canvas->GetScaleFactor();
            Vector2 topLeft = GetScreenPos();
            
            m_sprite.setPosition(topLeft);
            
            // Calculate scale ratio: (Target Size * Canvas Scale) / Original Texture Size
            Vector2 texSize = m_sprite.getTextureRect().getSize();
            m_sprite.setScale({ (m_bounds.w * scale) / texSize.x, (m_bounds.h * scale) / texSize.y });

            Renderer::Submit(m_sprite);
            GUIElement::OnRender(alpha);
        }

    private:
        Sprite m_sprite;
        Ref<Animation> m_animation;
        Animator m_animator;
    };

} // namespace pgn::GUI