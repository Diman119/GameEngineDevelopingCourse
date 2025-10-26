#pragma once

#include <Level.h>
#include <flecs.h>

#include <EditorECS/ecsEditor.h>

namespace GameEngine::EntitySystem::LevelEditorECS
{
	struct PositionDesc
	{
		// using cstyle pointers to values stored directly in a vector was bold
		std::shared_ptr<World::LevelObject::ComponentDesc> value;
	};

	void RegisterLevelEditorEcsSystems(flecs::world& world);
}