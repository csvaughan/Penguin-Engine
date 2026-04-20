#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Sprite.h"
#include "Renderer/Text.h"
#include "Renderer/RenderShape.h"
#include "Renderer/Camera.h"
#include "Renderer/VertexArray.h"
#include "Core/Window.h"
#include "Math/Matrix4.h"
#include "Math/MathUtils.h"
#include "Log/Log.h"
#include <SDL3/SDL_render.h>

namespace pgn {

    struct RendererData 
    {
        SDL_Renderer* sdlRenderer = nullptr;
        std::vector<RenderItem> queue;
        std::vector<FloatRect> scissorStack; 
        std::vector<Matrix4> transformStack;
        uint32_t nextStateGroupID = 0;
        Ref<Window> window;
        size_t frameDrawCalls = 0;
        Matrix4 viewProjectionMatrix;
        FloatRect cameraBounds;
    };

    static RendererData* s_Data = nullptr;

    static inline Vector2 transformPoint(const Vector2& v, const Matrix4& m) 
    {
        return {
            v.x * m.m[0] + v.y * m.m[4] + m.m[12],
            v.x * m.m[1] + v.y * m.m[5] + m.m[13]
        };
    }

    static SDL_Texture* getCommandTexture(const RenderCommand& cmd) 
    {
        return std::visit([](auto&& arg) -> SDL_Texture* {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, SpriteCommand>)   return arg.texture;
            if constexpr (std::is_same_v<T, GeometryCommand>) return arg.texture;
            if constexpr (std::is_same_v<T, TextCommand>)     return arg.fontAtlas;
            return (SDL_Texture*)nullptr; 
        }, cmd);
    }

    static void prepareScene(const FloatRect& bounds, const Matrix4& vpm) 
    {
        s_Data->viewProjectionMatrix = vpm;
        s_Data->cameraBounds = bounds;
        s_Data->queue.clear(); 
        s_Data->frameDrawCalls = 0;
        s_Data->nextStateGroupID = 0;
        s_Data->transformStack.clear();
        s_Data->transformStack.push_back(vpm); 
    }

    static void submitItem(RenderItem&& item) { s_Data->queue.push_back(std::move(item)); }

    // --- Renderer Lifecycle ---

    void Renderer::Init(Ref<Window> win) 
    {
        if (!s_Data) {
            s_Data = new RendererData();
            s_Data->sdlRenderer = win->GetRenderer();
            s_Data->window = win;
            s_Data->transformStack.push_back(Matrix4());
            PGN_CORE_INFO("Renderer initialized.");
        }
    }

    void Renderer::Shutdown() 
    {
        if (s_Data) 
        {
            delete s_Data;
            s_Data = nullptr;
            PGN_CORE_INFO("Renderer Shutdown Successfully...");
        }
    }

    void Renderer::clear() 
    {
        SDL_SetRenderDrawColor(s_Data->sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(s_Data->sdlRenderer);
    }

    void Renderer::present() { SDL_RenderPresent(s_Data->sdlRenderer); }

    const size_t Renderer::GetDrawCallCount() { return s_Data->frameDrawCalls; }

    // --- Scene Logic ---

    void Renderer::BeginScene(const Camera& camera) { prepareScene(camera.getViewportBounds(), camera.getViewProjection()); }
    void Renderer::BeginScene()                     { prepareScene({ Vector2::Zero(), s_Data->window->GetWindowSize() }, Matrix4());}

    void Renderer::EndScene() 
    {
        if (s_Data->queue.empty()) return;

        std::sort(s_Data->queue.begin(), s_Data->queue.end());

        std::vector<SDL_Vertex> bVerts;
        std::vector<int> bIndices;
        SDL_Texture* currTex = nullptr;

        auto renderBatch = [&]() {
            if (bVerts.empty()) return;
            s_Data->frameDrawCalls++;
            SDL_SetRenderDrawBlendMode(s_Data->sdlRenderer, SDL_BLENDMODE_BLEND);
            SDL_RenderGeometry(s_Data->sdlRenderer, currTex, bVerts.data(), (int)bVerts.size(), bIndices.data(), (int)bIndices.size());
            bVerts.clear(); bIndices.clear();
        };

        for (const auto& item : s_Data->queue) 
        {
            std::visit([&](auto&& cmd) {
                using T = std::decay_t<decltype(cmd)>;

                if constexpr (std::is_same_v<T, ScissorCommand>) 
                {
                    renderBatch(); 
                    if (cmd.enabled) {
                        SDL_Rect r = { (int)cmd.rect.x, (int)cmd.rect.y, (int)cmd.rect.w, (int)cmd.rect.h };
                        SDL_SetRenderClipRect(s_Data->sdlRenderer, &r);
                    } else {
                        SDL_SetRenderClipRect(s_Data->sdlRenderer, nullptr);
                    }
                } 
                else 
                {
                    SDL_Texture* tex = getCommandTexture(item.command);
                    if (tex != currTex && !bVerts.empty()) renderBatch();
                    currTex = tex;

                    int base = (int)bVerts.size();
                    if constexpr (std::is_same_v<T, SpriteCommand>) 
                    {
                        const SDL_Vertex* ptr = reinterpret_cast<const SDL_Vertex*>(cmd.vertices.data());
                        bVerts.insert(bVerts.end(), ptr, ptr + 4);
                        for (int idx : cmd.indices) bIndices.push_back(base + idx);
                    } 
                    else 
                    {
                        const auto& va = cmd.vertexArray;
                        const SDL_Vertex* ptr = reinterpret_cast<const SDL_Vertex*>(va.getVertices().data());
                        bVerts.insert(bVerts.end(), ptr, ptr + va.getVertices().size());
                        for (int idx : va.getIndices()) bIndices.push_back(base + idx);
                    }
                }
            }, item.command);
        }

        renderBatch();
        
        SDL_SetRenderClipRect(s_Data->sdlRenderer, nullptr);
        s_Data->queue.clear();
        s_Data->scissorStack.clear();
        s_Data->transformStack.clear();
        s_Data->transformStack.push_back(Matrix4());
    }

    // --- State Operations ---

    void Renderer::PushScissor(const FloatRect& rect, int zIndex) 
    {
        s_Data->nextStateGroupID++;
        FloatRect finalRect = rect;
        if (!s_Data->scissorStack.empty()) { finalRect = s_Data->scissorStack.back().intersection(rect); }
        s_Data->scissorStack.push_back(finalRect);
        submitItem({ zIndex, s_Data->nextStateGroupID, 0, ScissorCommand{finalRect, true} });
        s_Data->nextStateGroupID++; 
    }

    void Renderer::PopScissor(int zIndex) 
    {
        s_Data->nextStateGroupID++;
        if (!s_Data->scissorStack.empty()) s_Data->scissorStack.pop_back();
        ScissorCommand sc = (s_Data->scissorStack.empty()) ? ScissorCommand{{}, false} : ScissorCommand{s_Data->scissorStack.back(), true};
        submitItem({ zIndex, s_Data->nextStateGroupID, 0, sc});
        s_Data->nextStateGroupID++;
    }

    void Renderer::PushTransform(const Matrix4& transform) 
    {
        Matrix4 current = s_Data->transformStack.back();
        s_Data->transformStack.push_back(current * transform);
    }

    void Renderer::PopTransform() 
    {
        if (s_Data->transformStack.size() > 1) 
            s_Data->transformStack.pop_back();
    }

    // --- Drawing Implementations ---

    void Renderer::Submit(const VertexArray& va, Ref<Texture> texture, int zIndex) { Submit(va, texture, Transform2D(), zIndex); }

    void Renderer::Submit(const VertexArray& va, Ref<Texture> texture, const Transform2D& transform, int zIndex) 
    {
        if (!va.getBounds().intersects(s_Data->cameraBounds)) return; 
    
        const auto& localVertices = va.getVertices();
        if (localVertices.empty()) return;

        GeometryCommand cmd;
        cmd.texture = texture ? texture->getSDLTexture() : nullptr;
        const Matrix4 globalModel = s_Data->transformStack.back() * transform.GetModelMatrix();

        cmd.vertexArray.reserve(localVertices.size(), va.getIndices().size());
        cmd.vertexArray.getIndices() = va.getIndices(); 

        for (const auto& v : localVertices) 
            cmd.vertexArray.addVertex({ transformPoint(v.position, globalModel), v.color, v.tex_coord });

        submitItem({ zIndex, s_Data->nextStateGroupID, reinterpret_cast<uintptr_t>(cmd.texture), std::move(cmd) });
    }

    void Renderer::Submit(const Sprite& sprite) 
    {
        if (!sprite.getGlobalBounds().intersects(s_Data->cameraBounds)) return; 
        auto texPtr = sprite.getTexture().lock(); 
        if (!texPtr) return;

        const auto& t = sprite.getTransform();
        const auto& src = sprite.getTextureRect();
        const Color color = sprite.getColor();
        const Matrix4 globalModel = s_Data->transformStack.back() * t.GetModelMatrix();
    
        float tw, th;
        SDL_GetTextureSize(texPtr->getSDLTexture(), &tw, &th);
        float u0 = src.x / tw, v0 = src.y / th;
        float u1 = (src.x + src.w) / tw, v1 = (src.y + src.h) / th;

        SpriteCommand cmd;
        cmd.texture = texPtr->getSDLTexture();
        cmd.vertices = {
            Vertex{ transformPoint({-t.origin.x, -t.origin.y}, globalModel), color, {u0, v0} },
            Vertex{ transformPoint({-t.origin.x + src.w, -t.origin.y}, globalModel), color, {u1, v0} },
            Vertex{ transformPoint({-t.origin.x + src.w, -t.origin.y + src.h}, globalModel), color, {u1, v1} },
            Vertex{ transformPoint({-t.origin.x, -t.origin.y + src.h}, globalModel), color, {u0, v1} }
        };
        cmd.indices = { 0, 1, 2, 2, 3, 0 };

        submitItem({ sprite.getZIndex(), s_Data->nextStateGroupID, reinterpret_cast<uintptr_t>(cmd.texture), std::move(cmd) });
    }

    void Renderer::Submit(const Text& text) 
    {
        if (!text.getGlobalBounds().intersects(s_Data->cameraBounds)) return; 

        const auto& localVertices = text.getVertices(); 
        SDL_Texture* atlas = text.getAtlasTexture();
        if (localVertices.empty() || !atlas) return;

        const auto& t = text.getTransform();
        const Matrix4 globalModel = s_Data->transformStack.back() * t.GetModelMatrix();
        const Color color = text.getColor();

        TextCommand cmd;
        cmd.fontAtlas = atlas;
        cmd.vertexArray.reserve(localVertices.size(), text.getIndices().size());
        cmd.vertexArray.getIndices() = text.getIndices(); 

        for (const auto& v : localVertices) 
        {
            Vector2 localPos = v.position - t.origin;
            cmd.vertexArray.addVertex({ transformPoint(localPos, globalModel), color, v.tex_coord });
        }

        submitItem({ text.getZIndex(), s_Data->nextStateGroupID, reinterpret_cast<uintptr_t>(atlas), std::move(cmd) });
    }

    void Renderer::Submit(const RectangleShape& shape) 
    {
        if (!shape.getGlobalBounds().intersects(s_Data->cameraBounds)) return; 

        const auto& t = shape.getTransform();
        const Vector2 size = shape.getSize();
        const Matrix4 globalModel = s_Data->transformStack.back() * t.GetModelMatrix();
        const Color color = shape.getColor();

        GeometryCommand cmd;
        cmd.texture = nullptr;

        auto addQuad = [&](Vector2 p, Vector2 s) {
            int base = (int)cmd.vertexArray.getVertices().size();
            cmd.vertexArray.addIndex(base);     cmd.vertexArray.addIndex(base + 1); cmd.vertexArray.addIndex(base + 2);
            cmd.vertexArray.addIndex(base + 2); cmd.vertexArray.addIndex(base + 3); cmd.vertexArray.addIndex(base);

            cmd.vertexArray.addVertex({ transformPoint(p - t.origin, globalModel), color, {0,0} });
            cmd.vertexArray.addVertex({ transformPoint({p.x + s.x - t.origin.x, p.y - t.origin.y}, globalModel), color, {0,0} });
            cmd.vertexArray.addVertex({ transformPoint(p + s - t.origin, globalModel), color, {0,0} });
            cmd.vertexArray.addVertex({ transformPoint({p.x - t.origin.x, p.y + s.y - t.origin.y}, globalModel), color, {0,0} });
        };

        if (shape.isFilled()) addQuad({0, 0}, size);
        else {
            float th = shape.getOutlineThickness();
            addQuad({0, 0}, {size.x, th});
            addQuad({0, size.y - th}, {size.x, th});
            addQuad({0, th}, {th, size.y - 2*th});
            addQuad({size.x - th, th}, {th, size.y - 2*th});
        }

        submitItem({ shape.getZIndex(), s_Data->nextStateGroupID, 0, std::move(cmd) });
    }

    void Renderer::Submit(const CircleShape& shape) 
    {
        if (!shape.getGlobalBounds().intersects(s_Data->cameraBounds)) return; 
        
        const auto& t = shape.getTransform();
        const float radius = shape.getRadius();
        const Color color = shape.getColor();
        const Matrix4 globalModel = s_Data->transformStack.back() * t.GetModelMatrix();

        GeometryCommand cmd;
        cmd.texture = nullptr;
        const int segments = 64; 

        if (shape.isFilled()) 
        {
            cmd.vertexArray.addVertex({ transformPoint({0, 0}, globalModel), color, {0.5f, 0.5f} });
            for (int i = 0; i <= segments; i++) 
            {
                float theta = i * 2.0f * Math::PI / segments;
                Vector2 localPos = { radius * Math::Cos(theta), radius * Math::Sin(theta) };
                Vector2 uv = { (Math::Cos(theta) + 1.0f) * 0.5f, (Math::Sin(theta) + 1.0f) * 0.5f };
                cmd.vertexArray.addVertex({ transformPoint(localPos, globalModel), color, uv });
                if (i < segments) {
                    cmd.vertexArray.addIndex(0);
                    cmd.vertexArray.addIndex(i + 1);
                    cmd.vertexArray.addIndex(i + 2);
                }
            }
        } 
        else 
        {
            float thickness = shape.getOutlineThickness();
            for (int i = 0; i <= segments; i++) 
            {
                float theta = i * 2.0f * Math::PI / segments;
                float cosT = Math::Cos(theta); float sinT = Math::Sin(theta);
                Vector2 outerP = { radius * cosT, radius * sinT };
                Vector2 innerP = { (radius - thickness) * cosT, (radius - thickness) * sinT };

                cmd.vertexArray.addVertex({ transformPoint(innerP, globalModel), color, {0,0} }); 
                cmd.vertexArray.addVertex({ transformPoint(outerP, globalModel), color, {0,0} }); 

                if (i < segments) {
                    int base = i * 2;
                    cmd.vertexArray.addIndex(base);     cmd.vertexArray.addIndex(base + 1); cmd.vertexArray.addIndex(base + 2);
                    cmd.vertexArray.addIndex(base + 1); cmd.vertexArray.addIndex(base + 3); cmd.vertexArray.addIndex(base + 2);
                }
            }
        }
        submitItem({ shape.getZIndex(), s_Data->nextStateGroupID, 0, std::move(cmd) });
    }

    void Renderer::Submit(const LineShape& shape) 
    {
        if (!shape.getGlobalBounds().intersects(s_Data->cameraBounds)) return;

        const auto& t = shape.getTransform();
        const Matrix4 globalModel = s_Data->transformStack.back() * t.GetModelMatrix();
        const Color color = shape.getColor();
        Vector2 p1 = shape.getPoint1() - t.origin;
        Vector2 p2 = shape.getPoint2() - t.origin;

        Vector2 dir = p2 - p1;
        if (dir.LengthSquared() < 0.001f) return;

        Vector2 normal = Vector2{-dir.y, dir.x}.Normalize();
        float halfThickness = Math::Max(shape.getOutlineThickness(), 1.0f) * 0.5f;
        Vector2 offset = normal * halfThickness;

        GeometryCommand cmd;
        cmd.texture = nullptr;
        cmd.vertexArray.addIndex(0); cmd.vertexArray.addIndex(1); cmd.vertexArray.addIndex(2);
        cmd.vertexArray.addIndex(2); cmd.vertexArray.addIndex(3); cmd.vertexArray.addIndex(0);
        
        cmd.vertexArray.addVertex({ transformPoint(p1 + offset, globalModel), color, {0,0} });
        cmd.vertexArray.addVertex({ transformPoint(p2 + offset, globalModel), color, {0,0} });
        cmd.vertexArray.addVertex({ transformPoint(p2 - offset, globalModel), color, {0,0} });
        cmd.vertexArray.addVertex({ transformPoint(p1 - offset, globalModel), color, {0,0} });

        submitItem({ shape.getZIndex(), s_Data->nextStateGroupID, 0, std::move(cmd) });
    }
}