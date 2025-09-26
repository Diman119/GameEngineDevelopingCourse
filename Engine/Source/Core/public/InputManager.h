#pragma once

#include <windows.h>
#include <Core/export.h>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API InputManager final
		{
		public:
			InputManager() = default;
			~InputManager() = default;

		public:
			void ReadConfig();
			bool IsActionKeyDown();
			void ProcessKeyEvent();

		private:
			bool m_ActionKeyDown = false;

			int m_ActionKeyCode = VK_SPACE;
			int m_ExitKeyCode = VK_ESCAPE;
		};

		extern CORE_API InputManager* g_InputManager;
	}
}