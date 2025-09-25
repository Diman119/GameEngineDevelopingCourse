#include <InputManager.h>
#include <windows.h>

namespace GameEngine::Core
{
	InputManager* g_InputManager = nullptr;

	InputManager::InputManager()
	{}

	void InputManager::ReadConfig()
	{
	}

	bool InputManager::IsActionKeyDown()
	{
		return m_ActionKeyDown;
	}

	void InputManager::ProcessKeyEvent()
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 1)
		{
			PostQuitMessage(0);
		}

		m_ActionKeyDown = GetAsyncKeyState(VK_SPACE) & 1;
	}
}