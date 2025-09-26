#include <InputManager.h>
#include <INIReader.h>

namespace GameEngine::Core
{
	InputManager* g_InputManager = nullptr;

	void InputManager::ReadConfig()
	{
		static const std::string SECTION = "Input";
		auto reader = INIReader("config.ini");
		m_ActionKeyCode = reader.GetInteger(SECTION, "ActionKeyCode", m_ActionKeyCode);
		m_ExitKeyCode = reader.GetInteger(SECTION, "ExitKeyCode", m_ExitKeyCode);
	}

	bool InputManager::IsActionKeyDown()
	{
		return m_ActionKeyDown;
	}

	void InputManager::ProcessKeyEvent()
	{
		if (GetAsyncKeyState(m_ExitKeyCode) & 1)
		{
			PostQuitMessage(0);
		}

		m_ActionKeyDown = GetAsyncKeyState(m_ActionKeyCode) & 1;
	}
}