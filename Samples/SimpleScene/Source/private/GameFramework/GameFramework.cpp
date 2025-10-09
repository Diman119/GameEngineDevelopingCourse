#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
	RegisterEcsPhysSystems(m_World);

	flecs::entity cubeControl = m_World.entity()
		.set(Position{ Math::Vector3f(-2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 0.f, 0.f) })
		.set(Speed{ 10.f })
		.set(FrictionAmount{ 0.9f })
		.set(JumpSpeed{ 10.f })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 0.3f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMoving = m_World.entity()
		.set(Position{ Math::Vector3f(2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 3.f, 0.f) })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 1.f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() });

	flecs::entity camera = m_World.entity()
		.set(Position{ Math::Vector3f(0.0f, 12.0f, -10.0f) })
		.set(Speed{ 10.f })
		.set(CameraPtr{ Core::g_MainCamera })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) })
		.set(Shooter{ 0.1f, 2.f, 20.f, 6, 6, 0.f, Core::Timer() });

	flecs::query<const Position, Projectile> q = m_World.query<const Position, Projectile>();

	// destructible without bonus
	for (int i = 0; i < 10; ++i)
	{
		m_World.entity()
			.set(Position{ Math::Vector3f(-8.f, i * 3 + 8.f, 0.f) })
			.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
			.set(RenderObjectPtr{ new Render::RenderObject() })
			.set(Destructible{ 0, q });
	}

	// destructible with bonus
	for (int i = 0; i < 10; ++i)
	{
		m_World.entity()
			.set(Position{ Math::Vector3f(i * 3 + 8.f, 8.f, 0.f) })
			.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
			.set(RenderObjectPtr{ new Render::RenderObject() })
			.set(Destructible{ 10, q });
	}
}

void GameFramework::Update(float dt)
{

}