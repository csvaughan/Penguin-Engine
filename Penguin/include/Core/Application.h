#pragma once
#include "Window.h"
#include "Assets/AssetManager.h"
#include "Memory/Scope.h"
#include "Memory/Ref.h"

namespace pgn
{
	class State;

	struct ApplicationSpecification
	{
		std::string Name;
		std::string AssetPath;
		unsigned int CustomRandomSeed; //Used for debugging, leave at 0 for engine random value.
		WindowSpecification WindowSpec;

		ApplicationSpecification(std::string n = "Application", std::string ap = "", unsigned int rs = 0, WindowSpecification ws = WindowSpecification()) 
		: Name(n), AssetPath(ap), CustomRandomSeed(rs), WindowSpec(ws) {}
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
			m_pendingState = CreateScope<TState>(std::forward<Args>(args)...);
		}

		static Application& Get();
		Vector2 GetFramebufferSize() const;
		Ref<Window> GetWindow() const { return m_window; }
		static double GetTime();
		float GetFPS() const { return m_FPS; }
		AssetManager& GetAssetManager() { return m_assetManager; }
	
	private:
		void RaiseEvent(Event& event);
		void ExecuteStateChange();

	private:
		ApplicationSpecification m_Specification;
		Ref<Window> m_window;
		bool m_running = false;
		bool m_minimized = false;

		Scope<State> m_activeState;
		Scope<State> m_pendingState;

		AssetManager m_assetManager;

		uint64_t m_startTime = 0; 
		double m_perfFrequency = 0.0;

		double m_FPS = 0.0f;
    	double m_FPSTimer = 0.0f;
    	int m_FrameCount = 0;
		
		friend class State;
	};
}