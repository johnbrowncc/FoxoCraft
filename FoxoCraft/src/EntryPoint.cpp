#include "Sandbox.h"

static FoxoCommons::Application* s_App;

namespace FoxoCommons
{
	Application* GetApplication()
	{
		return s_App;
	}
}

int main()
{
	s_App = new FoxoCraft::Sandbox();
	s_App->Start();
	delete s_App;
}