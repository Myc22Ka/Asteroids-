#include "Entity.h"
#include "FileMenager.h"

Entity::Entity(Vector2f position, float angle, int spriteState, int size, Color hitboxColor)
	: position(position), angle(angle), size(size), hitboxColor(hitboxColor), radius(size >> 1) {}

void Entity::drawHitboxes()
{
	shape.setRadius(radius);
	Vector2f center(radius, radius);

	shape.setOrigin(center);

	shape.setFillColor(Color::Transparent);
	shape.setOutlineColor(hitboxColor);
	shape.setOutlineThickness(1.5f);
}