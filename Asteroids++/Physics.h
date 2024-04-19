#pragma once
#include <SFML/Graphics.hpp>
#include "Asteroid.h"

namespace physics {
	bool intersects(const Vector2f& point, const float& size, const Vector2f& circlePosition, const float& size2);
	float length(const Vector2f& v);
	Vector2f normalize(const Vector2f& v);
	float dotProduct(const Vector2f& v1, const Vector2f& v2);
	float distance(const Vector2f& p1, const Vector2f& p2);
}
