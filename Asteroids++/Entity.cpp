#include "Entity.h"
#include "FileMenager.h"

Entity::Entity(Vector2f position, double angle, int size, Color hitboxColor, SpriteInfo spriteInfo)
	: position(position), angle(angle), size(size), hitboxColor(hitboxColor), radius(size >> 1), spriteInfo(spriteInfo) 
{
}

void Entity::drawHitboxes()
{
	shape.setRadius(radius);
	Vector2f center(radius, radius);

	shape.setOrigin(center);

	shape.setFillColor(Color::Transparent);
	shape.setOutlineColor(hitboxColor);
	shape.setOutlineThickness(1.5f);
}

void Entity::drawHitboxes(const int radius)
{
	shape.setRadius(radius);
	Vector2f center(radius, radius);

	shape.setOrigin(center);

	shape.setFillColor(Color::Transparent);
	shape.setOutlineColor(hitboxColor);
	shape.setOutlineThickness(1.5f);
}

void Entity::setSpriteFullCycle(double deltaTime){
	spriteInfo.currentSpriteLifeTime -= deltaTime;

	if (spriteInfo.currentSpriteLifeTime <= 0) {
		spriteInfo.currentSpriteLifeTime = spriteInfo.defaultSpriteLifeTime;
		spriteInfo.spriteState = (spriteInfo.spriteState + 1) % spriteInfo.frames.size();
		updateSprite(spriteInfo.sprite, spriteInfo.frames, spriteInfo.spriteState);
	}
}

void Entity::setSpriteFullCycle(SpriteInfo& spriteInfo) {

	if (spriteInfo.currentSpriteLifeTime <= 0) {
		spriteInfo.currentSpriteLifeTime = spriteInfo.defaultSpriteLifeTime;
		spriteInfo.spriteState = (spriteInfo.spriteState + 1) % spriteInfo.frames.size();
		updateSprite(spriteInfo.sprite, spriteInfo.frames, spriteInfo.spriteState);
	}
}