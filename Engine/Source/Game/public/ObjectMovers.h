#pragma once
#include <Vector.h>

namespace GameEngine
{
	class IObjectMover
	{
	public:
		virtual ~IObjectMover() = default;

		virtual Math::Vector3f Move(Math::Vector3f currentPosition, float dt) = 0;
	};

	class BounceObjectMover : public IObjectMover
	{
	public:
		BounceObjectMover(Math::Vector3f gravity, float floorY) :
			IObjectMover(),
			m_Gravity(gravity),
			m_Velocity(Math::Vector3f::Zero()),
			m_FloorY(floorY) {
		}

		Math::Vector3f Move(Math::Vector3f currentPosition, float dt) override
		{
			m_Velocity = m_Velocity + m_Gravity * dt;
			currentPosition = currentPosition + m_Velocity * dt;
			if (currentPosition.y < m_FloorY)
			{
				m_Velocity.y *= -1.f;
				currentPosition.y += (m_FloorY - currentPosition.y) * 2.f;
			}
			return currentPosition;
		}

	private:
		Math::Vector3f m_Gravity;
		Math::Vector3f m_Velocity;
		float m_FloorY;
	};

	class CyclicObjectMover : public IObjectMover
	{
	public:
		CyclicObjectMover(Math::Vector3f velocity, float reverseInterval) :
			IObjectMover(),
			m_Velocity(velocity),
			m_ReverseInterval(reverseInterval) {
		}

		Math::Vector3f Move(Math::Vector3f currentPosition, float dt) override
		{
			m_TimeSinceReverse += dt;
			if (m_TimeSinceReverse > m_ReverseInterval)
			{
				m_TimeSinceReverse -= m_ReverseInterval;
				m_Velocity = -m_Velocity;
			}
			return currentPosition + m_Velocity * dt;
		}

	private:
		Math::Vector3f m_Velocity;
		float m_ReverseInterval;
		float m_TimeSinceReverse = 0.f;
	};

	class ControlledObjectMover : public IObjectMover
	{
	public:
		ControlledObjectMover(float speed) :
			IObjectMover(),
			m_Speed(speed) {
		}

		void Move(Math::Vector3f dir)
		{
			m_CurrentMoveDir = m_CurrentMoveDir + dir;
		}

		Math::Vector3f Move(Math::Vector3f currentPosition, float dt)
		{
			currentPosition = currentPosition + m_CurrentMoveDir.Normalized() * m_Speed * dt;
			m_CurrentMoveDir = Math::Vector3f::Zero();
			return currentPosition;
		}

	private:
		Math::Vector3f m_CurrentMoveDir = Math::Vector3f::Zero();
		float m_Speed;
	};
}