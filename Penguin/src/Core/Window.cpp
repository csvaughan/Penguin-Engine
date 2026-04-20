#include "Core/Window.h"
#include "Core/Input.h"
#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Events/InputEvents.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <imgui_impl_sdl3.h>

namespace pgn 
{

	Window::Window(const WindowSpecification& specification): m_Specification(specification) {}

    Window::~Window() { Destroy(); }

	bool Window::IsFullscreen() const {return (SDL_GetWindowFlags(m_Handle) & SDL_WINDOW_FULLSCREEN);}
    bool Window::IsMinimized() const { return (SDL_GetWindowFlags(m_Handle) & SDL_WINDOW_MINIMIZED); }

    void Window::Create()
    {
        if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
        {
            if (!SDL_Init(SDL_INIT_VIDEO)) // SDL3 returns bool
            {
                std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
                assert(false);
            }
        }

        uint32_t windowFlags = SDL_WINDOW_HIDDEN;
        if (m_Specification.IsResizeable)
            windowFlags |= SDL_WINDOW_RESIZABLE;

        m_Handle = SDL_CreateWindow(m_Specification.Title.c_str(), 
                                    m_Specification.Width, 
                                    m_Specification.Height, 
                                    windowFlags);

        if (!m_Handle) {assert(false);}

        // Create the Renderer
        // NULL lets SDL3 choose the best driver (Vulkan, Metal, D3D11, etc.)
        m_Renderer = SDL_CreateRenderer(m_Handle, NULL);
        
        if (!m_Renderer) {
            std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
            assert(false);
        }

        // 3. Handle VSync via Renderer properties
        SDL_SetRenderVSync(m_Renderer, m_Specification.VSync ? 1 : 0);
        
        SDL_ShowWindow(m_Handle);
    }

    void Window::Destroy()
    {
        if (m_Renderer) SDL_DestroyRenderer(m_Renderer);
        if (m_Handle) SDL_DestroyWindow(m_Handle);
        m_Handle = nullptr;
        m_Renderer = nullptr;
    }

	void Window::ProcessEvent(const SDL_Event& e)
	{
		#ifdef PGN_DEBUG
			ImGui_ImplSDL3_ProcessEvent(&e);
			ImGuiIO& io = ImGui::GetIO();
			
			// Check for Mouse Capture
			bool isMouseEvent = (e.type >= SDL_EVENT_MOUSE_MOTION && e.type <= SDL_EVENT_MOUSE_WHEEL);
			if (isMouseEvent && io.WantCaptureMouse) 
				return;

			// Check for Keyboard Capture
			bool isKeyEvent = (e.type >= SDL_EVENT_KEY_DOWN && e.type <= SDL_EVENT_KEY_UP);
			if (isKeyEvent && io.WantCaptureKeyboard)
				return;
		#endif

		switch (e.type)
		{
			case SDL_EVENT_QUIT:
			{
				m_ShouldClose = true;
				WindowClosedEvent event;
				RaiseEvent(event);
				break;
			}
			case SDL_EVENT_WINDOW_MINIMIZED:
			{
				WindowMinimizedEvent event;
				RaiseEvent(event);
				break;
			}
			case SDL_EVENT_WINDOW_RESTORED:
			{
				WindowRestoredEvent event;
				RaiseEvent(event);
				break;
			}
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					uint32_t newWidth = (uint32_t)e.window.data1;
					uint32_t newHeight = (uint32_t)e.window.data2;

					// Update internal spec
					m_Specification.Width = newWidth;
					m_Specification.Height = newHeight;

					SDL_SetRenderViewport(m_Renderer, NULL); 

					WindowResizeEvent event(newWidth, newHeight);
					RaiseEvent(event);
				}
				break;
			}
			
			case SDL_EVENT_KEY_DOWN:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					KeyPressedEvent event(e.key.key, e.key.repeat);
					RaiseEvent(event);
				}
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					KeyReleasedEvent event(e.key.key);
					RaiseEvent(event);
				}
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					MouseButtonPressedEvent event(e.button.button, { (float)e.button.x, (float)e.button.y });
					RaiseEvent(event);
				}
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					MouseButtonReleasedEvent event(e.button.button, { (float)e.button.x, (float)e.button.y });
					RaiseEvent(event);
				}
				break;
			}
			case SDL_EVENT_MOUSE_WHEEL:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					MouseScrolledEvent event(GetMousePos(), e.wheel.x, e.wheel.y);
					RaiseEvent(event);
				}
				break;
			}
			case SDL_EVENT_MOUSE_MOTION:
			{
				if (e.window.windowID == SDL_GetWindowID(m_Handle))
				{
					MouseMovedEvent event({e.motion.x, e.motion.y});
					RaiseEvent(event);
				}
				break;
			}
		}
	}

	void Window::RaiseEvent(Event& event)
	{
		if (m_Specification.EventCallback) {m_Specification.EventCallback(event);}
	}

	Vector2 Window::GetFramebufferSize() const
	{
		int width, height;
		SDL_GetWindowSizeInPixels(m_Handle, &width, &height);
		return { (float)width, (float)height };
	}

	Vector2 Window::GetMousePos() const
	{
		float x, y;
		SDL_GetMouseState(&x, &y);
		return { x, y };
	}

	bool Window::ShouldClose() const
	{
		return m_ShouldClose;
	}

	void Window::SetFullscreen(bool enabled) 
	{
		SDL_SetWindowFullscreen(m_Handle, enabled);
	}

	void Window::SetWindowedFullscreen(bool enabled)
	{
		SDL_SetWindowFullscreenMode(m_Handle, NULL); // Use current desktop mode
		SDL_SetWindowFullscreen(m_Handle, enabled); 
	}
}