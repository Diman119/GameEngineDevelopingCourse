#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <ObjectMovers.h>

namespace GameEngine
{
	class GameObject final
	{
	public:
		GameObject() = default;
		GameObject(std::unique_ptr<IObjectMover> moverPtr): m_Mover(std::move(moverPtr)) { }

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		void Move(float dt, size_t frame)
		{
			if (m_Mover)
			{
				SetPosition(m_Mover->Move(GetPosition(), dt), frame);
			}
		}

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();

		std::unique_ptr<IObjectMover> m_Mover;
	};
}