#pragma once
#include "Window.h"
#include "Asset/AssetManager.h"
#include "Memory/Scope.h"
#include "Memory/Ref.h"

namespace pgn
{
	class State;
	class Renderer;

	struct ApplicationSpecification
	{
		std::string Name;
		std::string AssetPath;
		std::string WindowIconPath;
		
		unsigned int CustomRandomSeed; //Used for debugging, leave at 0 for engine random value.

		WindowSpecification WindowSpec;

		ApplicationSpecification(
			std::string n = "Application", 
			std::string ap = "", 
			std::string wip = "",
			unsigned int seed = 0, 
			WindowSpecification ws = WindowSpecification()) 
		: Name(n), AssetPath(ap), WindowIconPath(wip), CustomRandomSeed(seed), WindowSpec(ws) {}
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification = ApplicationSpecification());
		~Application();

		void Run();
		void Stop();

		template<typename TState, typename... Args>
		requires(std::is_base_of_v<State, TState>)
		void PushState(Args&&... args)
		{
			m_PendingState = CreateScope<TState>(std::forward<Args>(args)...);
		}

		static Application& Get();
		Vector2 GetFramebufferSize() const;
		Ref<Window> GetWindow() const { return m_Window; }
		static double GetTime();
		float GetFPS() const { return m_FPS; }
		AssetManager& GetAssetManager() { return m_AssetManager; }
	
	private:
		void RaiseEvent(Event& event);
		void ExecuteStateChange();

	private:
		ApplicationSpecification m_Specification;
		Ref<Window> m_Window;
		Scope<Renderer> m_Renderer;
		
		bool m_Running = false;
		bool m_Minimized = false;

		Scope<State> m_ActiveState;
		Scope<State> m_PendingState;

		AssetManager m_AssetManager;

		uint64_t m_StartTime = 0; 
		double m_PerfFrequency = 0.0;

		double m_FPS = 0.0f;
    	double m_FPSTimer = 0.0f;
    	int m_FrameCount = 0;
		
		friend class State;
		friend class AppContext;
	};
}