#include "Asteroid.h"
#include "Bullet.h"
#include "EntitiesList.h"
#include <iostream>
#include <random>
#include "Physics.h"

constexpr double M_PI = 3.14159265358979323846;

Asteroid::Asteroid() : Entity(getRandomPosition(), getRandomAngle(), 0, getRandomValue<int>(FileMenager::enemiesData.asteroid_size), sf::Color::Red)
{
	direction = getRandomDirection();
	speed = getRandomValue<float>(FileMenager::enemiesData.asteroid_speed);

	drawHitboxes();

	drawSprite(Sprites::ASTEROID, 0);
}

void Asteroid::render(sf::RenderWindow& window)
{
	sf::Transform transform;
	window.draw(sprite, transform.translate(position).rotate(angle));
	if(WindowBox::hitboxesVisibility) window.draw(shape, transform);
}

void Asteroid::update(float deltaTime) {
	angle += FileMenager::enemiesData.asteroid_spin * deltaTime;
	position += sf::Vector2f(direction.x * speed * deltaTime, direction.y * speed * deltaTime);
	spriteLifeTime -= deltaTime;

	const int radius = size >> 1;

	if (position.x < radius) {
		direction.x = abs(direction.x);
	}
	else if (position.x > FileMenager::screenData.size_width - radius) {
		direction.x = -abs(direction.x);
	}

	if (position.y < radius) {
		direction.y = abs(direction.y);
	}
	else if (position.y > FileMenager::screenData.size_height - radius) {
		direction.y = -abs(direction.y);
	}

	if (spriteLifeTime <= 0) {
		spriteLifeTime = FileMenager::playerData.sprite_cycle_time;
		spriteState = (spriteState + 1) % 2;
		setSprite(spriteState);
	}

	collisionDetection();
}

const sf::CircleShape& Asteroid::getVertexShape() const
{
	return shape;
}

const EntityType Asteroid::getEntityType()
{
	return EntityType::TYPE_ASTEROID;
}

void Asteroid::collisionDetection()
{
	for (const auto& entity : EntitiesList::entities) {
		if (entity->getEntityType() == EntityType::TYPE_ASTEROID && entity != this) {
			Asteroid* otherAsteroid = dynamic_cast<Asteroid*>(entity);
			if (otherAsteroid) {
				if (physics::intersects(this->position, size >> 1, otherAsteroid->position, otherAsteroid->size >> 1)) {
					direction = -direction;
					otherAsteroid->direction = -otherAsteroid->direction;
				}
			}
		}
	}
}

const sf::Vector2f Asteroid::getRandomPosition()
{
	const int radius = size >> 1;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> xAxis(radius, FileMenager::screenData.size_width - radius);
	std::uniform_real_distribution<float> yAxis(radius, FileMenager::screenData.size_height - radius);

	return sf::Vector2f(xAxis(gen), yAxis(gen));
}

const float Asteroid::getRandomAngle()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0, 360.0);

	return dist(gen);
}

const sf::Vector2f Asteroid::getRandomDirection()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0, 2.0f * M_PI);

	float angle = dist(gen);
	return sf::Vector2f(cos(angle), sin(angle));
}

template<typename T>
inline const T Asteroid::getRandomValue(const T& base)
{
	const double based = base;
		
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dist(0.75 * based, 1.25 * based);

	return static_cast<T>(dist(gen));
}
