#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
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
		position.x = position.x + currentMoveDir.Normalized().x * speed.value * world.delta_time();
		position.y = position.y + currentMoveDir.Normalized().y * speed.value * world.delta_time();
		position.z = position.z + currentMoveDir.Normalized().z * speed.value * world.delta_time();
		camera.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z));
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.y = jump.value;
			}
		}
	});

	world.system<const Position, const CameraPtr, Shooter, const ControllerPtr>()
		.each([&](const Position& position, const CameraPtr& camera, Shooter& shooter, const ControllerPtr& controller)
	{
		shooter.timeToShot -= world.delta_time();
		if (controller.ptr->IsPressed("Shoot") && shooter.timeToShot <= 0.f)
		{
			flecs::entity projectile;
			if (shooter.projectileCache.empty())
			{
				projectile = world.entity()
					.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
					.set(Bounciness{ 0.5f })
					.set(ProjectileCollider{ 0.f })
					.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::SmallOctahedron() })
					.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });
			}
			else
			{
				projectile = shooter.projectileCache.back();
				shooter.projectileCache.pop_back();
			}

			Math::Vector3f vel = camera.ptr->GetViewDir() * shooter.projectileSpeed;

			projectile
				.set(position)
				.set(Velocity{ vel.x, vel.y, vel.z })
				.set(Gravity{ 0.f, -9.8065f, 0.f })
				.set(Projectile(&shooter, 5.f));

			if (--shooter.currentAmmo <= 0)
			{
				shooter.timeToShot = shooter.reloadInterval;
				shooter.currentAmmo = shooter.defaultAmmo;
			}
			else
			{
				shooter.timeToShot = shooter.shotInterval;
			}
		}
	});

	world.system<Position, Velocity, Gravity, const BouncePlane, Projectile>()
		.each([&](flecs::entity e, Position& pos, Velocity& vel, Gravity& gravity, const BouncePlane& plane, Projectile& projectile)
	{
		if (projectile.timeToRecycle < 0.f)
		{
			return;
		}

		projectile.timeToRecycle -= world.delta_time();

		if (projectile.timeToRecycle < 0.f)
		{
			gravity.x = gravity.y = gravity.z = 0.f;
			vel.x = vel.y = vel.z = 0.f;
			pos.x = pos.y = pos.z = 1e5f;

			projectile.shooterPtr->projectileCache.push_back(e);
		}
	});
}

