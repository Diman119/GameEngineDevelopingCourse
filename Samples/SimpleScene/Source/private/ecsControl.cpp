#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <ecsMesh.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>
#include <RenderObject.h>
#include <DefaultGeometry.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
	{
		Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
		if (controller.ptr->IsPressed("GoLeft"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoRight"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoBack"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
		}
		if (controller.ptr->IsPressed("GoForward"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
		}
		position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
		camera.ptr->SetPosition(position.value);
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.value.y = jump.value;
			}
		}
	});


	// shooter
	world.system<Position, const CameraPtr, Shooter, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, const CameraPtr& camera, Shooter& shooter, const ControllerPtr& controller)
	{
		shooter.timer.Tick();
		if (controller.ptr->IsPressed("Shoot") && shooter.timer.GetTotalTime() > shooter.nextShotInterval)
		{
			shooter.timer.Reset();

			flecs::entity projectile;
			if (shooter.projectileCache.empty())
			{
				Core::Timer timer;
				timer.Stop();

				projectile = world.entity()
					.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
					.set(Bounciness{ 0.5f })
					.set(GeometryPtr{ RenderCore::DefaultGeometry::SmallOctahedron() })
					.set(RenderObjectPtr{ new Render::RenderObject() })
					.set(Projectile(&shooter, 5.f, timer));
			}
			else
			{
				projectile = shooter.projectileCache.back();
				shooter.projectileCache.pop_back();
			}

			projectile
				.set(Position{ position.value })
				.set(Velocity{ camera.ptr->GetViewDir() * shooter.projectileSpeed })
				.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) });

			if (--shooter.currentAmmo <= 0)
			{
				shooter.nextShotInterval = shooter.reloadInterval;
				shooter.currentAmmo = shooter.defaultAmmo;
			}
			else
			{
				shooter.nextShotInterval = shooter.shotInterval;
			}
		}
	});


	// projectiles
	world.system<Position, Velocity, Gravity, const BouncePlane, Projectile>()
		.each([&](flecs::entity e, Position& pos, Velocity& vel, Gravity& gravity, const BouncePlane& plane, Projectile& projectile)
	{
		if (projectile.timer.IsStopped())
		{
			float dotPos = plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z;
			if (dotPos < plane.value.w)
			{
				projectile.timer.Reset();
				projectile.timer.Start();
			}
		}
		else
		{
			projectile.timer.Tick();
			if (projectile.timer.GetTotalTime() > projectile.destroyInterval)
			{
				gravity.value.x = gravity.value.y = gravity.value.z = 0.f;
				vel.value.x = vel.value.y = vel.value.z = 0.f;
				pos.value.x = pos.value.y = pos.value.z = 1e5f;

				projectile.timer.Stop();

				projectile.shooterPtr->projectileCache.push_back(e);
			}
		}
	});
}

