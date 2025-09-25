#pragma once

#include <Core/export.h>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API InputManager final
		{
		public:
			InputManager();
			~InputManager() = default;

		public:
			void ReadConfig();
			bool IsActionKeyDown();
			void ProcessKeyEvent();

		private:
			bool m_ActionKeyDown = false;
		};

		extern CORE_API InputManager* g_InputManager;
	}
}