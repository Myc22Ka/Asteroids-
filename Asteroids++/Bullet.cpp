#include "Bullet.h"
#include "SingleAsteroid.h"
#include "MultiAsteroid.h"
#include "Physics.h"
#include "Score.h"
#include "SoundData.h"
#include "Pickup.h"

constexpr double M_PI = 3.14159265358979323846;

bool Bullet::piercing{ false };

Bullet::Bullet(Vector2f position, Vector2f direction, float& angle) :
    direction(direction), 
    Entity(position, angle, Player::playerStats.bulletSize, Color::Green, getSprite(Player::getPlayerBulletSprite())),
    lifeTime(FileMenager::playerData.bullet_lifetime)
{
}

void Bullet::update(float deltaTime)
{
    lifeTime -= deltaTime;
    position += Vector2f(direction.x * Player::playerStats.bulletSpeed * deltaTime, direction.y * Player::playerStats.bulletSpeed * deltaTime);
    if (lifeTime <= 0) Game::removeEntity(this);

    if (Player::playerStats.bulletType.homing) homeToEnemy(deltaTime);

    collisionDetection();
}

void Bullet::homeToEnemy(float deltaTime) {
    const auto enemy = findNearestEnemy();

    if (enemy) {
        Vector2f directionToEnemy = enemy->position - position;

        Vector2f normalizedDirection = physics::normalize(directionToEnemy);

        float speed = Player::playerStats.bulletSpeed;
        Vector2f movement = Vector2f(normalizedDirection.x * speed * deltaTime, normalizedDirection.y * speed * deltaTime);

        position += movement;

        if (position.x < enemy->position.x) {
            Vector2f homingDirection = physics::calculateDirection(position, enemy->position);
            position += homingDirection * speed * deltaTime;
        }
        else if (position.x > enemy->position.x) {
            Vector2f homingDirection = physics::calculateDirection(position, enemy->position);
            position += homingDirection * speed * deltaTime;
        }
    }
}



Entity* Bullet::findNearestEnemy()
{
    Entity* nearestEnemy = nullptr;
    float minTimeToEnemy = 1;

    Entity* player = nullptr;

    for (Entity* entity : Game::entities) {
        if (entity->getEntityType() == EntityType::TYPE_PLAYER) player = entity;
    }

    if (!player) return nullptr;

    for (Entity* entity : Game::entities)
    {
        if (entity->getEntityType() == EntityType::TYPE_ENEMY_SINGLE_ASTEROID)
        {
            Vector2f directionToAsteroid = entity->position - position;
            float distanceToEnemy = physics::distance(position, entity->position);

            float timeToEnemy = distanceToEnemy / Player::playerStats.bulletSpeed;

            if (timeToEnemy < minTimeToEnemy)
            {
                float targetAngle = atan2(directionToAsteroid.y, directionToAsteroid.x) * 180.0f / M_PI;

                float adjustedAngle = targetAngle - angle;

                setRotation(spriteInfo.sprite, adjustedAngle);

                angle = targetAngle;

                minTimeToEnemy = timeToEnemy;
                nearestEnemy = entity;
            }
        }
    }

    return nearestEnemy;
}



void Bullet::render(RenderWindow& window)
{
    Transform transform;
	window.draw(spriteInfo.sprite, transform.translate(position));
    if (Game::hitboxesVisibility) window.draw(shape, transform);
}

void Bullet::collisionDetection()
{
    for (size_t i = 0; i < Game::entities.size(); i++)
    {
        if (typeid(*Game::entities[i]) == typeid(SingleAsteroid)) asteroidHit<SingleAsteroid>(i);
        
        if (typeid(*Game::entities[i]) == typeid(MultiAsteroid)) asteroidHit<MultiAsteroid>(i);
    }
}

template<typename T>
void Bullet::asteroidHit(const int& i) {
    T* asteroid = dynamic_cast<T*>(Game::entities[i]);

    if (physics::intersects(position, radius, asteroid->position, asteroid->radius) && lifeTime > 0 && hitAsteroids.find(i) == hitAsteroids.end()) {
        if (!Player::playerStats.bulletType.piercing) lifeTime = 0;
        asteroid->health -= Player::playerStats.bulletDamage;

        if (asteroid->health > 0) {
            hitAsteroids.insert(i);
            return;
        }

        if(typeid(*Game::entities[i]) == typeid(SingleAsteroid)) destroySingleAsteroid(i);
        if(typeid(*Game::entities[i]) == typeid(MultiAsteroid)) destroyMultiAsteroid(i);

    }
}

void Bullet::spawnPickup(const Vector2f& position)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    auto propability = dist(gen);

    if (propability > 0.5) return;

    if (propability < 1) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::HEART1UP)));
        return;
    }
    if (propability < 0.1) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::PICKUP_PIERCING)));
        return;
    }
    if (propability < 0.2) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::PICKUP_4)));
        return;
    }
    if (propability < 0.3) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::PICKUP_3)));
        return;
    }
    if (propability < 0.4) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::PICKUP_2)));
        return;
    }
    if (propability < 0.5) {
        Game::addToEntities(new Pickup(position, getSprite(Sprites::PICKUP_1)));
        return;
    }
    
}

void Bullet::destroySingleAsteroid(const int& i)
{
    SingleAsteroid* asteroid = dynamic_cast<SingleAsteroid*>(Game::entities[i]);

    Game::toAddList.push_back(new Explosion(asteroid->position, asteroid->size));

    Game::removeEntity(asteroid);

    spawnPickup(asteroid->position);

    Score::score += 10;
    SoundData::play(Sounds::EXPLOSION);
}

void Bullet::destroyMultiAsteroid(const int& i)
{
    MultiAsteroid* asteroid = dynamic_cast<MultiAsteroid*>(Game::entities[i]);

    Game::toAddList.push_back(new Explosion(asteroid->position, asteroid->size));

    Game::removeEntity(asteroid);

    auto asteroid1 = new SingleAsteroid(asteroid->position, asteroid->getRandomDirection());
    auto asteroid2 = new SingleAsteroid(Vector2f(asteroid->position.x + asteroid1->radius, asteroid->position.y + asteroid1->radius), asteroid->getRandomDirection());

    const auto bounceDirection = physics::bounceDirection(asteroid1, asteroid2, 1.0f);

    asteroid1->position += bounceDirection.second;
    asteroid2->position -= bounceDirection.second;

    asteroid1->direction -= bounceDirection.first;
    asteroid2->direction += bounceDirection.first;

    Game::addToEntities(new SingleAsteroid(asteroid1->position, asteroid1->direction));
    Game::addToEntities(new SingleAsteroid(asteroid2->position, asteroid2->direction));

    Score::score += 20;
    SoundData::play(Sounds::EXPLOSION);
}
