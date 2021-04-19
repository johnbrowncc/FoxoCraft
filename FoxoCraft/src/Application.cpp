#include "Application.h"

namespace FoxoCraft
{
	void Application::Start()
	{
		if (m_Running) return;
		m_Running = true;

		Init();

		double lastTime, currentTime;

		lastTime = GetTime();
		
		while (m_Running)
		{
			currentTime = GetTime();
			m_DeltaTime = currentTime - lastTime;
			lastTime = currentTime;

			Update();
		}

		Destroy();
	}

	void Application::Stop()
	{
		m_Running = false;
	}
}