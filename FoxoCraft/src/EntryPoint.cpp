#include "Sandbox.h"

static FoxoCraft::Application* s_App;

namespace FoxoCraft
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