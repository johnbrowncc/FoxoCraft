#pragma once

namespace FoxoCraft
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		void Start();
		void Stop();

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Destroy() = 0;

		virtual double GetTime() = 0;

		inline double GetDeltaTime() const
		{
			return m_DeltaTime;
		}
	private:
		bool m_Running = false;
		double m_DeltaTime = 1.;
	};

	Application* GetApplication();
}