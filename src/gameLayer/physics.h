#pragma once
#include <raylib.h>
#include <raymath.h>
#include <cmath>

// Vector2 operator overloads
inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return { a.x + b.x, a.y + b.y };
}

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return { a.x - b.x, a.y - b.y };
}

inline Vector2 operator*(const Vector2& a, float scalar)
{
	return { a.x * scalar, a.y * scalar };
}

inline Vector2 operator/(const Vector2& a, float scalar)
{
	return { a.x / scalar, a.y / scalar };
}


inline Vector2& operator*=(Vector2& a, float scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	return a;
}

inline Vector2& operator/=(Vector2& a, float scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	return a;
}

inline Vector2& operator+=(Vector2& a, float scalar)
{
	a.x += scalar;
	a.y += scalar;
	return a;
}

inline Vector2& operator-=(Vector2& a, float scalar)
{
	a.x -= scalar;
	a.y -= scalar;
	return a;
}


inline bool operator==(const Vector2& a, const Vector2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2& a, const Vector2& b)
{
	return !(a == b);
}

inline Vector2& operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline Vector2& operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline Vector2& operator*=(Vector2& a, const Vector2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}

inline Vector2& operator/=(Vector2& a, const Vector2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}

struct Transform2D
{

	Vector2 pos = {};	// center
	float w = 0;	// width
	float h = 0;	// height

	Vector2 getCenter()			const { return { pos.x, pos.y }; }
	Vector2 getTop()			const { return { pos.x, pos.y - h * 0.5f }; }
	Vector2 getBottom()			const { return { pos.x, pos.y + h * 0.5f }; }
	Vector2 getLeft()			const { return { pos.x - w * 0.5f, pos.y }; }
	Vector2 getRight()			const { return { pos.x + w * 0.5f, pos.y }; }
	Vector2 getTopLeft()		const { return { pos.x - w * 0.5f, pos.y - w * 0.5f }; }
	Vector2 getTopRight()		const { return { pos.x + w * 0.5f, pos.y - w * 0.5f }; }
	Vector2 getBottomLeft()		const { return { pos.x - w * 0.5f, pos.y + w * 0.5f }; }
	Vector2 getBottomRight()	const { return { pos.x + w * 0.5f, pos.y + w * 0.5f }; }

	// using for rendering
	Rectangle getAABB()
	{
		return { pos.x - w * 0.5f, pos.y - h * 0.5f, w, h };
	}

	Rectangle getAABBFromTopLeft()
	{
		return { pos.x, pos.y, w, h };
	}

	// detect collision

	bool intersectPoint(Vector2 point, float delta = 0)		// delta is for scaling
	{
		Rectangle aabb = getAABB();
		aabb.x -= delta;
		aabb.y -= delta;
		aabb.width += 2 * delta;
		aabb.height += 2 * delta;

		return CheckCollisionPointRec(point, aabb);
	}

	bool intersectTransform(Transform2D other, float delta = 0)		// delta is for scaling
	{
		Rectangle a = getAABB();
		Rectangle b = other.getAABB();

		a.x -= delta;
		a.y -= delta;
		a.width += 2 * delta;
		a.height += 2 * delta;

		b.x -= delta;
		b.y -= delta;
		b.width += 2 * delta;
		b.height += 2 * delta;

		return CheckCollisionRecs(a, b);
	}

	bool intersectTransformFromTopLeft(Transform2D other, float delta = 0)		// delta is for scaling
	{
		Rectangle a = getAABB();
		Rectangle b = other.getAABBFromTopLeft();

		a.x -= delta;
		a.y -= delta;
		a.width += 2 * delta;
		a.height += 2 * delta;

		b.x -= delta;
		b.y -= delta;
		b.width += 2 * delta;
		b.height += 2 * delta;

		return CheckCollisionRecs(a, b);
	}
};

struct GameMap;

struct PhysicalEntity
{
	Transform2D transform;
	Vector2 lastPosition = {};

	Vector2 velocity = {};
	Vector2 acceleration = {};

	// learn when and how the player touch the boundary
	bool upTouch = 0;
	bool downTouch = 0;
	bool leftTouch = 0;
	bool rightTouch = 0;

	// When transferring the entity, both the current position 
	// and the position of the previous frame should be change simultaneously.
	void teleport(Vector2 pos)
	{
		transform.pos = pos;
		lastPosition = pos;
	}

	void updateForces(float deltaTime)
	{
		velocity += acceleration * deltaTime;
		transform.pos += velocity * deltaTime;

		// universal drag (air resistence / friction)
		Vector2 dragVector = Vector2{ velocity.x * std::abs(velocity.x),  velocity.y * std::abs(velocity.y) };
		float dragCoefficient = 0.01f;		// tweak this for your needs

		if (Vector2Length(dragVector) * dragCoefficient * deltaTime > Vector2Length(velocity))
		{
			velocity = {};
		}
		else
		{
			velocity -= dragVector * dragCoefficient * deltaTime;
		}
		if (Vector2Length(velocity) < 0.01f)
		{
			velocity = {};
		}
		acceleration = {};
	}

	// called at the end of the frame
	void updateFinal()
	{
		lastPosition = { transform.pos.x, transform.pos.y };
	}

	void applyGravity()
	{
		acceleration += {0.f, 10.0f};
	}

	// function to resolve the collisions
	void resolveConstrains(GameMap& mapData);

	void checkCollisionOnce(Vector2& pos, GameMap& mapData);

	Vector2 performCollisionOnOneAxis(GameMap& mapData, Vector2 pos, Vector2 delta);
};