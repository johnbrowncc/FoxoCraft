#pragma once

#include <deque>
#include <functional>

namespace FoxoCraft
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		inline void Start()
		{
			if (m_Running) return;
			m_Running = true;

			Init();

			double lastTime = GetTime();
			double currentTime;

			while (m_Running)
			{
				currentTime = GetTime();
				m_DeltaTime = currentTime - lastTime;
				lastTime = currentTime;

				while (!m_InvokesNextFrame.empty())
				{
					m_InvokesNextFrame.front()();
					m_InvokesNextFrame.pop_front();
				}

				Update();
			}

			Destroy();
		}

		inline void Stop()
		{
			m_Running = false;
		}

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Destroy() = 0;
		virtual double GetTime() = 0;

		inline double GetDeltaTime() const
		{
			return m_DeltaTime;
		}

		inline void InvokeNextFrame(const std::function<void()>& function)
		{
			m_InvokesNextFrame.push_back(function);
		}
	private:
		bool m_Running = false;
		double m_DeltaTime = 1.;

		std::deque<std::function<void()>> m_InvokesNextFrame;
	};

	// To be implmented by the client
	Application* GetApplication();
}