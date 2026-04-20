#pragma once
#include "Math/Vector.h"
#include "pgpch.h"
#include "Memory/Ref.h"

    struct SDL_Renderer;
    union SDL_Event;
    struct SDL_Window;

namespace pgn {

    class Event;

	struct WindowSpecification
	{
		std::string Title;
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool IsResizeable = true;
		bool VSync = true;

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
	};

	class Window : public RefCounted
    {
    public:
        Window(const WindowSpecification& specification = WindowSpecification());
        ~Window();

        Vector2 GetFramebufferSize() const;
        Vector2 GetMousePos() const;
        uint32_t GetWindowWidth()   const    { return m_Specification.Width; }
        uint32_t GetWindowHeight()  const    { return m_Specification.Height; }
        Vector2 GetWindowSize()   const    { return {(float)GetWindowWidth(), (float)GetWindowHeight()}; }
        bool ShouldClose() const;

        SDL_Window* GetHandle() const { return m_Handle; }
        SDL_Renderer* GetRenderer() const { return m_Renderer; } 

        void SetFullscreen(bool enabled);
        void SetWindowedFullscreen(bool enabled);
        bool IsFullscreen() const;
        bool IsMinimized() const;

    private:
        void Create();
        void Destroy();
        
        void ProcessEvent(const SDL_Event& event);
        void RaiseEvent(Event& event);

    private:
        WindowSpecification m_Specification;
        SDL_Window* m_Handle = nullptr;
        SDL_Renderer* m_Renderer = nullptr; 
        bool m_ShouldClose = false;

        friend class Application;
    };
}