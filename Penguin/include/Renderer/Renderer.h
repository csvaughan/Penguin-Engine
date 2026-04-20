#pragma once
#include "pgpch.h"
#include "Memory/Ref.h"
#include "Math/Rect.h"


namespace pgn {

    class Window;
    class Sprite;
    class Text;
    class CircleShape;
    class RectangleShape;
    class LineShape;
    class Camera;
    class VertexArray;
    class Texture;
    class Transform2D;
    class Matrix4;

    class Renderer 
    {
    public:
        // Scene Management
        static void BeginScene(const Camera& camera);
        static void BeginScene(); // Default view
        static void EndScene();

        //state Management
        static void PushScissor(const FloatRect& rect, int zIndex);
        static void PopScissor(int zIndex);
        static void PushTransform(const Matrix4& transform); 
        static void PopTransform();

        // Drawing API
        static void Submit(const VertexArray& va, Ref<Texture> texture, int zIndex);
        static void Submit(const VertexArray& va, Ref<Texture> texture, const Transform2D& transform, int zIndex);
        static void Submit(const Sprite& sprite);
        static void Submit(const Text& text);
        static void Submit(const RectangleShape& shape);
        static void Submit(const CircleShape& shape);
        static void Submit(const LineShape& shape);

        static const size_t GetDrawCallCount();

    private:
        static void Init(Ref<Window> win);
        static void clear();
        static void present();
        static void Shutdown();
        
        friend class Application;
    };
}