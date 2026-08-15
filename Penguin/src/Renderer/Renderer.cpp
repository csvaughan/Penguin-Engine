#include "Renderer/Renderer.h"
#include "Log/Log.h"
#include <SDL3/SDL_render.h>
#include <algorithm>

namespace pgn 
{
    static inline Vector2 transformPoint(const Vector2& v, const Matrix4& m) 
    {
        return {
            v.x * m.m[0] + v.y * m.m[4] + m.m[12],
            v.x * m.m[1] + v.y * m.m[5] + m.m[13]
        };
    }

    // Transform a local bounding box into world space AABB
    static inline FloatRect transformRect(const FloatRect& rect, const Matrix4& m)
    {
        Vector2 p1 = transformPoint({ rect.x, rect.y }, m);
        Vector2 p2 = transformPoint({ rect.x + rect.w, rect.y }, m);
        Vector2 p3 = transformPoint({ rect.x + rect.w, rect.y + rect.h }, m);
        Vector2 p4 = transformPoint({ rect.x, rect.y + rect.h }, m);

        float minX = std::min({ p1.x, p2.x, p3.x, p4.x });
        float maxX = std::max({ p1.x, p2.x, p3.x, p4.x });
        float minY = std::min({ p1.y, p2.y, p3.y, p4.y });
        float maxY = std::max({ p1.y, p2.y, p3.y, p4.y });

        return { minX, minY, maxX - minX, maxY - minY };
    }

    Renderer::Renderer(Ref<Window> window)
    {
        PGN_ASSERT(window, "Window handle provided to Renderer was null!");
        m_SDLRenderer = window->GetRenderer();

        Vector2 size = window->GetFramebufferSize();
        m_ViewportSize = size;
        InitLightmapTarget(static_cast<int>(size.x), static_cast<int>(size.y));
    }

    Renderer::~Renderer()
    {
        if (m_LightmapTexture)
            SDL_DestroyTexture(m_LightmapTexture);
    }

    void Renderer::InitLightmapTarget(int width, int height)
    {
        if (m_LightmapTexture)
            SDL_DestroyTexture(m_LightmapTexture);

        m_LightmapTexture = SDL_CreateTexture(
            m_SDLRenderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width, height
        );

        SDL_SetTextureBlendMode(m_LightmapTexture, SDL_BLENDMODE_MUL);
    }

    void Renderer::Clear(Color color)
    {
        SDL_SetRenderDrawColorFloat(m_SDLRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(m_SDLRenderer);
    }

    void Renderer::Present()
    {
        SDL_RenderPresent(m_SDLRenderer);
    }

    void Renderer::PrepareScene(const FloatRect& bounds, const Matrix4& vpm) 
    {
        m_ViewProjectionMatrix = vpm;
        m_CameraBounds = bounds;
        s_FrameDrawCalls = 0;
        m_NextStateGroupID = 0;

        m_DiffuseQueue.clear();
        m_LightQueue.clear();
        m_ScissorStack.clear();

        m_TransformStack.clear();
        m_TransformStack.push_back(vpm); // Bottom of stack holds the ViewProjection matrix
    }

    void Renderer::BeginScene()
    {
        PrepareScene({ Vector2::Zero(), m_ViewportSize }, Matrix4());
    }

    void Renderer::BeginScene(const Camera& camera)
    {
        PrepareScene(camera.getViewportBounds(), camera.getViewProjection());
    }

    void Renderer::PushTransform(const Matrix4& transform)
    {
        Matrix4 current = GetCurrentTransform();
        m_TransformStack.push_back(current * transform);
    }

    void Renderer::PopTransform()
    {
        if (m_TransformStack.size() > 1)
            m_TransformStack.pop_back();
    }

    Matrix4 Renderer::GetCurrentTransform() const
    {
        if (!m_TransformStack.empty())
            return m_TransformStack.back();
        return Matrix4();
    }

    void Renderer::PushScissor(const FloatRect& rect, int zIndex)
    {
        m_NextStateGroupID++;
        FloatRect finalRect = rect;
        if (!m_ScissorStack.empty()) 
            finalRect = m_ScissorStack.back().intersection(rect);

        m_ScissorStack.push_back(finalRect);

        DrawCommand cmd;
        cmd.isScissor = true;
        cmd.scissorRect = finalRect;
        cmd.scissorEnabled = true;
        cmd.zIndex = zIndex;
        cmd.stateGroupID = m_NextStateGroupID++;

        m_DiffuseQueue.push_back(cmd);
    }

    void Renderer::PopScissor(int zIndex)
    {
        m_NextStateGroupID++;
        if (!m_ScissorStack.empty()) 
            m_ScissorStack.pop_back();

        DrawCommand cmd;
        cmd.isScissor = true;
        cmd.scissorEnabled = !m_ScissorStack.empty();
        if (cmd.scissorEnabled)
            cmd.scissorRect = m_ScissorStack.back();

        cmd.zIndex = zIndex;
        cmd.stateGroupID = m_NextStateGroupID++;

        m_DiffuseQueue.push_back(cmd);
    }

    void Renderer::Submit( const VertexArray& va, Ref<Texture> texture, const Matrix4& transform, int zIndex, RenderPass pass)
    {
        const auto& localVertices = va.getVertices();
        const auto& localIndices = va.getIndices();
        if (localVertices.empty() || localIndices.empty()) return;

        // 1. Calculate World Model Matrix
        Matrix4 worldModel = GetCurrentTransform() * transform;

        // 2. Transform local bounds into World Space for Culling
        FloatRect worldBounds = transformRect(va.getBounds(), worldModel);
        if (!worldBounds.intersects(m_CameraBounds)) 
            return; // Correctly culled only when outside camera bounds

        // 3. Combine with View-Projection matrix for final screen transformation
        Matrix4 mvp = m_ViewProjectionMatrix * worldModel;

        DrawCommand cmd;
        cmd.texture = texture ? texture->getSDLTexture() : nullptr;
        cmd.zIndex = zIndex;
        cmd.stateGroupID = m_NextStateGroupID;
        cmd.indices = localIndices;

        // 4. Project local vertices to screen coordinates
        cmd.vertices.reserve(localVertices.size());
        for (const auto& v : localVertices)
        {
            Vector2 p = transformPoint(v.position, mvp);
            cmd.vertices.push_back(SDL_Vertex{
                { p.x, p.y },
                { v.color.r, v.color.g, v.color.b, v.color.a },
                { v.texCoords.x, v.texCoords.y }
            });
        }

        if (pass == RenderPass::Diffuse)
            m_DiffuseQueue.push_back(std::move(cmd));
        else if (pass == RenderPass::Light)
            m_LightQueue.push_back(std::move(cmd));
    }

    void Renderer::Flush()
    {
        auto processCommandQueue = [this](std::vector<DrawCommand>& queue) {
            if (queue.empty()) return;

            // Sort by Z-Index, then by State Group ID
            std::sort(queue.begin(), queue.end(), [](const DrawCommand& a, const DrawCommand& b) {
                if (a.zIndex != b.zIndex)
                    return a.zIndex < b.zIndex;
                return a.stateGroupID < b.stateGroupID;
            });

            std::vector<SDL_Vertex> bVerts;
            std::vector<int> bIndices;
            SDL_Texture* currTex = nullptr;

            auto renderBatch = [&]() {
                if (bVerts.empty()) return;
                s_FrameDrawCalls++;
                SDL_SetRenderDrawBlendMode(m_SDLRenderer, SDL_BLENDMODE_BLEND);
                SDL_RenderGeometry(
                    m_SDLRenderer,
                    currTex,
                    bVerts.data(), static_cast<int>(bVerts.size()),
                    bIndices.data(), static_cast<int>(bIndices.size())
                );
                bVerts.clear();
                bIndices.clear();
            };

            for (const auto& cmd : queue)
            {
                if (cmd.isScissor)
                {
                    renderBatch();
                    if (cmd.scissorEnabled) {
                        SDL_Rect r = { static_cast<int>(cmd.scissorRect.x), static_cast<int>(cmd.scissorRect.y), static_cast<int>(cmd.scissorRect.w), static_cast<int>(cmd.scissorRect.h) };
                        SDL_SetRenderClipRect(m_SDLRenderer, &r);
                    } else {
                        SDL_SetRenderClipRect(m_SDLRenderer, nullptr);
                    }
                }
                else
                {
                    if (cmd.texture != currTex && !bVerts.empty())
                        renderBatch();

                    currTex = cmd.texture;
                    int base = static_cast<int>(bVerts.size());

                    bVerts.insert(bVerts.end(), cmd.vertices.begin(), cmd.vertices.end());
                    for (int idx : cmd.indices)
                    {
                        bIndices.push_back(base + idx);
                    }
                }
            }

            renderBatch();
            SDL_SetRenderClipRect(m_SDLRenderer, nullptr);
        };

        // -------------------------------------------------------------
        // PASS 1: Render Lightmap Offscreen
        // -------------------------------------------------------------
        if (!m_LightQueue.empty())
        {
            SDL_SetRenderTarget(m_SDLRenderer, m_LightmapTexture);

            SDL_SetRenderDrawColorFloat(
                m_SDLRenderer, 
                m_AmbientLight.r, 
                m_AmbientLight.g, 
                m_AmbientLight.b, 
                1.0f
            );
            SDL_RenderClear(m_SDLRenderer);

            SDL_SetRenderDrawBlendMode(m_SDLRenderer, SDL_BLENDMODE_ADD);

            processCommandQueue(m_LightQueue);

            SDL_SetRenderTarget(m_SDLRenderer, nullptr);
        }

        // -------------------------------------------------------------
        // PASS 2: Render Diffuse Scene Geometry
        // -------------------------------------------------------------
        processCommandQueue(m_DiffuseQueue);

        // -------------------------------------------------------------
        // PASS 3: Composite Multiplicative Lightmap Over Diffuse Scene
        // -------------------------------------------------------------
        if (!m_LightQueue.empty())
        {
            SDL_RenderTexture(m_SDLRenderer, m_LightmapTexture, nullptr, nullptr);
        }

        m_DiffuseQueue.clear();
        m_LightQueue.clear();
    }

    void Renderer::EndScene()
    {
        Flush();
    }
}