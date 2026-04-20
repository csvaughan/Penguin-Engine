#include "Core/Application.h"
#include "Core/Input.h"
#include "Log/Log.h"
#include "Core/State.h"
#include "Core/Timestep.h"
#include "Events/EngineEvents.h"
#include "Events/WindowEvents.h"
#include "Events/EventBus.h"
#include "Renderer/Renderer.h"
#include "Math/Random.h"
#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace pgn
{
	static Application* s_Application = nullptr;

	Application::Application(const ApplicationSpecification& specification) : m_Specification(specification)
	{
		s_Application = this;
		m_startTime = SDL_GetPerformanceCounter();
		m_perfFrequency = (double)SDL_GetPerformanceFrequency();
		
		Log::Init();
		AudioSystem::Init();

		if (m_Specification.CustomRandomSeed != 0) 
        	pgn::Random::SetGlobalSeed(m_Specification.CustomRandomSeed);
		else 
			pgn::Random::SetGlobalSeed(std::random_device{}()); // Use a random device seed by default
    
    	PGN_CORE_INFO("Random system initialized with seed: {}", pgn::Random::masterSeed.load());

		// Initialize SDL Events and Video
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
			PGN_ASSERT("Failed to initialize SDL: {}", SDL_GetError()); 

		// Set window title to app name if empty
		if (m_Specification.WindowSpec.Title.empty())
			m_Specification.WindowSpec.Title = m_Specification.Name;

		// The Window triggers this callback, which dispatches to layers
		m_Specification.WindowSpec.EventCallback = [this](Event& event) { RaiseEvent(event); };

		m_window = CreateRef<Window>(m_Specification.WindowSpec);
		m_window->Create();

		Renderer::Init(m_window);

		#ifdef PGN_DEBUG
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Optional: enables window docking

			ImGui_ImplSDL3_InitForSDLRenderer(m_window->GetHandle(), m_window->GetRenderer());
			ImGui_ImplSDLRenderer3_Init(m_window->GetRenderer());
			
			PGN_CORE_INFO("ImGui Initialized (Debug Mode)");
		#endif

		m_assetManager.Init(m_window->GetRenderer(), m_Specification.AssetPath);
		PGN_CORE_INFO("{} created.", m_Specification.Name);
	}

	Application::~Application()
	{

		#ifdef PGN_DEBUG
			ImGui_ImplSDLRenderer3_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
			PGN_CORE_INFO("ImGUI Shutdown");
		#endif

		m_assetManager.Shutdown();
		AudioSystem::Shutdown();
		Renderer::Shutdown();
		m_window->Destroy();
		SDL_Quit();
		s_Application = nullptr;
	}

	void Application::Run()
	{
		m_running = true;
		double lastTime = GetTime();
		double accumulator = 0.0f;
		const float fixedDeltaTime = 1.0f / 60.0f; 

		while (m_running)
		{
			ExecuteStateChange();
			EventBus::ProcessPending();

			double currentTime = GetTime();
			double frameTime = currentTime - lastTime;
			lastTime = currentTime;

			// Cap the frameTime to prevent "Spiral of Death" if the game lags
			if (frameTime > 0.25f) frameTime = 0.25f; 

			m_FPSTimer += (float)frameTime;
			m_FrameCount++;

			if (m_FPSTimer >= 0.5f) // Update FPS every half second
			{
				m_FPS = (float)m_FrameCount / m_FPSTimer;
				m_FrameCount = 0;
				m_FPSTimer = 0.0f;
			}

			SDL_Event event;
			while (SDL_PollEvent(&event)) { m_window->ProcessEvent(event); }

			if (m_window->ShouldClose()) 
			{ 
				AppQuitEvent quitEvent;

				if (m_activeState && !quitEvent.Handled)
					m_activeState->OnAppQuit(quitEvent);

				if (!quitEvent.Handled) 
					Stop(); 
			}

			// Only simulate if we are running and not minimized
			if (m_running && m_activeState && !m_window->IsMinimized())
			{
				accumulator += frameTime;

				while (accumulator >= fixedDeltaTime)
				{
					m_activeState->OnUpdate(Timestep(fixedDeltaTime));
					accumulator -= fixedDeltaTime;
				}

				Renderer::clear();

			#ifdef PGN_DEBUG
				// Start ImGui Frame
				ImGui_ImplSDLRenderer3_NewFrame();
				ImGui_ImplSDL3_NewFrame();
				ImGui::NewFrame();
			#endif

				float alpha = accumulator / fixedDeltaTime;
				m_activeState->OnRender(alpha); 

			#ifdef PGN_DEBUG
				// Render ImGui
				ImGui::Render();
				ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_window->GetRenderer());
			#endif

				Renderer::present();
			}

			Input::OnUpdate();
		}
	}

	void Application::Stop()
	{
		if(m_running)
		{
			m_running = false;
			PGN_CORE_INFO("{} shutting down...", m_Specification.Name);
		}
	}

	void Application::RaiseEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
    
		dispatcher.Dispatch<WindowMinimizedEvent>([this](WindowMinimizedEvent& e) {
			Input::ClearStates();
			m_minimized = true;
			return false; 
		});

		dispatcher.Dispatch<WindowRestoredEvent>([this](WindowRestoredEvent& e) {
			m_minimized = false;
			return false;
		});

		dispatcher.Dispatch<StateChangeEvent>([this](StateChangeEvent& e) {
			if (m_pendingState) return true;
			m_pendingState = e.GetState();
			return true; 
		});

		Input::OnEvent(event); 

		if(m_activeState && !m_minimized) //State shouldn't process events if Window is minimized
			m_activeState->OnEvent(event);
	}

	Vector2 Application::GetFramebufferSize() const
	{
		return m_window->GetFramebufferSize();
	}

	Application& Application::Get()
	{
		assert(s_Application);
		return *s_Application;
	}

	double Application::GetTime()
	{
		uint64_t now = SDL_GetPerformanceCounter();
        uint64_t relativeTicks = now - s_Application->m_startTime;
        
        return (double)(relativeTicks / s_Application->m_perfFrequency);
	}

	void Application::ExecuteStateChange()
	{
		if (!m_pendingState) return;
		PGN_CORE_INFO("New Active State: {}", m_pendingState->GetName());
		if (m_activeState) m_activeState->OnExit();
		m_activeState = std::move(m_pendingState); 
		m_activeState->OnEnter();
	}
}