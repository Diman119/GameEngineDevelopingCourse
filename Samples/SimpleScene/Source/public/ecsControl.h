#pragma once

#include <flecs.h>
#include <Timer.h>

struct Position;

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
	float nextShotInterval;
	GameEngine::Core::Timer timer;
	std::vector<flecs::entity> projectileCache;
};

struct Projectile
{
	Shooter* shooterPtr;
	float destroyInterval;
	GameEngine::Core::Timer timer;
};

struct Destructible
{
	int ammoBonus;
	flecs::query<const Position, Projectile> q;
};

void RegisterEcsControlSystems(flecs::world& world);

