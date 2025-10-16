#pragma once

#include <flecs.h>

namespace GameEngine::Core
{
	class Camera;
	class Controller;
}

struct ControllerPtr
{
	GameEngine::Core::Controller* ptr;
};

struct JumpSpeed
{
	float value;
};

struct CameraPtr
{
	GameEngine::Core::Camera* ptr;
};

struct Shooter
{
	float shotInterval;
	float reloadInterval;
	float projectileSpeed;
	int currentAmmo;
	int defaultAmmo;
	float timeToShot;
	std::vector<flecs::entity> projectileCache;
};

struct Projectile
{
	Shooter* shooterPtr;
	float timeToRecycle;
};


void RegisterEcsControlSystems(flecs::world& world);

