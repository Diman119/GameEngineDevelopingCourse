#include <LevelObject.h>

namespace GameEngine::World
{
	void LevelObject::AddComponent(const ComponentName& name, const ComponentDesc& desc)
	{
		m_ComponentList.emplace_back(name, std::make_shared<ComponentDesc>(desc));
	}
}