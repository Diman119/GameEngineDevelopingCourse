#include <Camera.h>
#include <InputManager.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_InputManager = new Core::InputManager();
		Core::g_InputManager->ReadConfig();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		// How many objects do we want to create
		for (int i = 0; i < 3; ++i)
		{
			m_Objects.push_back(new GameObject());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Update(dt);
			
			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();

			m_renderThread->OnEndFrame();
		}
	}

	void Game::Update(float dt)
	{
		float displacement = 0.5f * dt;
		if (Core::g_InputManager->IsActionKeyDown())
		{
			displacement = -displacement;
		}

		for (int i = 0; i < m_Objects.size(); ++i)
		{
			Math::Vector3f pos = m_Objects[i]->GetPosition();

			// Showcase
			if (i == 0)
			{
				pos.x += displacement;
			}
			else if (i == 1)
			{
				pos.y -= displacement;
			}
			else if (i == 2)
			{
				pos.x += displacement;
				pos.y -= displacement;
			}
			m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
		}
	}
}