#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>
#include <random>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist3(0, 2);

		std::vector<ControlledObjectMover*> controlledMovers;

		// How many objects do we want to create
		for (int i = -5; i < 5; ++i)
		{
			for (int j = -5; j < 5; ++j)
			{
				Math::Vector3f pos = Math::Vector3f(i * 4.f, 0.f, j * 4.f);
				std::unique_ptr<IObjectMover> mover;

				switch (dist3(rng))
				{
				case 0:
					mover = std::make_unique<BounceObjectMover>(Math::Vector3f(0.f, -10.f, 0.f), -5.f);
					break;
				case 1:
					mover = std::make_unique<CyclicObjectMover>(Math::Vector3f(0.f, 0.f, 4.f), 0.5f);
					break;
				case 2:
					std::unique_ptr<ControlledObjectMover> controlledMover = std::make_unique<ControlledObjectMover>(4.f);
					controlledMovers.push_back(controlledMover.get());
					mover = std::move(controlledMover);
				}

				GameObject* obj = new GameObject(std::move(mover));

				m_Objects.push_back(obj);

				Render::RenderObject** renderObject = obj->GetRenderObjectRef();
				m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);

				obj->SetPosition(pos, m_renderThread->GetMainFrame());
			}
		}

		Core::g_InputHandler->RegisterCallback("GoForward", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", [&]() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });

		Core::g_InputHandler->RegisterCallback("ObjectsGoLeft", [movers = controlledMovers]() {
			for (ControlledObjectMover* mover : movers)
			{
				mover->Move(Math::Vector3f(-1.f, 0.f, 0.f));
			}
		});
		Core::g_InputHandler->RegisterCallback("ObjectsGoRight", [movers = std::move(controlledMovers)]() {
			for (ControlledObjectMover* mover : movers)
			{
				mover->Move(Math::Vector3f(1.f, 0.f, 0.f));
			}
		});
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

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Move(dt, m_renderThread->GetMainFrame());
		}
	}
}