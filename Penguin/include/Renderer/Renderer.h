#pragma once

#include "Core/Window.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Camera.h"
#include "Texture.h"
#include "Math/Matrix4.h"
#include "Renderer/Color.h"
#include "Math/Rect.h"
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace pgn 
{
    enum class RenderPass 
    { 
        Diffuse, 
        Light 
    };

    struct DrawCommand 
    {
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;
        SDL_Texture* texture = nullptr;
        int zIndex = 0;
        uint32_t stateGroupID = 0;

        // Scissor Command Payload
        bool isScissor = false;
        FloatRect scissorRect;
        bool scissorEnabled = false;
    };

    class Renderer 
    {
    public:
        explicit Renderer(Ref<Window> window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        
        // Scene Lifecycle
        void BeginScene();
        void BeginScene(const Camera& camera);
        void EndScene();

        // Geometry & Light Submission
        void Submit(
            const VertexArray& va, 
            Ref<Texture> texture, 
            const Matrix4& transform = Matrix4(), 
            int zIndex = 0, 
            RenderPass pass = RenderPass::Diffuse
        );

        // Transform Hierarchy Stack
        void PushTransform(const Matrix4& transform);
        void PopTransform();
        Matrix4 GetCurrentTransform() const;

        // Lighting Controls
        void SetAmbientLight(Color color) { m_AmbientLight = color; }
        Color GetAmbientLight() const { return m_AmbientLight; }

        // Scissor / Clipping Controls
        void PushScissor(const FloatRect& rect, int zIndex = 0);
        void PopScissor(int zIndex = 0);

        // Frame Statistics (Static)
        static size_t GetDrawCallCount() { return s_FrameDrawCalls; }

        // Native Handle Access
        SDL_Renderer* GetSDLRenderer() const { return m_SDLRenderer; }

    private:
        void Clear(Color color = Color::Black);
        void Present();
        void Flush();

        void PrepareScene(const FloatRect& bounds, const Matrix4& vpm);
        void InitLightmapTarget(int width, int height);

    private:
        SDL_Renderer* m_SDLRenderer = nullptr;
        SDL_Texture* m_LightmapTexture = nullptr;

        Color m_AmbientLight = Color(0.2f, 0.2f, 0.2f, 1.0f);

        std::vector<DrawCommand> m_DiffuseQueue;
        std::vector<DrawCommand> m_LightQueue;
        std::vector<FloatRect> m_ScissorStack;
        std::vector<Matrix4> m_TransformStack;

        Matrix4 m_ViewProjectionMatrix;
        Vector2 m_ViewportSize = { 1280.0f, 720.0f };
        FloatRect m_CameraBounds;

        uint32_t m_NextStateGroupID = 0;
        inline static size_t s_FrameDrawCalls = 0;

        friend class Application;
    };
}