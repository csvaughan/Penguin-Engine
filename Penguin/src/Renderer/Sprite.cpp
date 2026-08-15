#include "Renderer/Sprite.h"
#include "Renderer/Renderer.h"

namespace pgn
{
    void Sprite::render(Renderer& renderer) const
    {
        auto texPtr = getTexture().lock();
        if (!texPtr) return;

        const auto& t = getTransform();
        const auto& src = getTextureRect();
        const Color color = getColor();

        float tw = texPtr->getWidth();
        float th = texPtr->getHeight();
        float u0 = src.x / tw,         v0 = src.y / th;
        float u1 = (src.x + src.w)/tw, v1 = (src.y + src.h)/th;

        VertexArray va;
        va.addQuad(
            Vertex{ {-t.origin.x,         -t.origin.y},         color, {u0, v0} },
            Vertex{ {-t.origin.x + src.w, -t.origin.y},         color, {u1, v0} },
            Vertex{ {-t.origin.x + src.w, -t.origin.y + src.h}, color, {u1, v1} },
            Vertex{ {-t.origin.x,         -t.origin.y + src.h}, color, {u0, v1} }
        );

        renderer.Submit(va, texPtr, t.GetModelMatrix(), getZIndex(), RenderPass::Diffuse);
    }
} // namespace pgn
