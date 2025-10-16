#pragma once

#include <array.h>
#include <Geometry.h>
#include <Vector.h>
#include <export.h>

namespace GameEngine
{
	namespace RenderCore
	{
		namespace DefaultGeometry
		{
			Geometry::Ptr RENDER_CORE_API Cube();
			Geometry::Ptr RENDER_CORE_API SmallOctahedron();
		}
	}
}