#include "Player.h"

constexpr double M_PI = 3.14159265358979323846;
float Player::dashTimer = 0;

Player::Player() :
	Entity(Vector2f(FileMenager::playerData.start_position_x, FileMenager::playerData.start_position_y), FileMenager::playerData.start_position_angle, 0, FileMenager::playerData.size, Color::Blue), 
    shootTimer(), 
    speed(FileMenager::playerData.speed)
{
    spriteInfo = getSprite(Sprites::SHIP);
	drawHitboxes();
}

void Player::render(RenderWindow& window)
{
	Transform transform;
	window.draw(spriteInfo.sprite, transform.translate(position).rotate(angle));
	if(Game::hitboxesVisibility) window.draw(shape, transform);
}

void Player::update(float deltaTime) {
    const auto turnSpeed = FileMenager::playerData.turn_speed;
    shootTimer -= deltaTime;
    dashTimer -= deltaTime;
    spriteInfo.spriteLifeTime -= deltaTime;

    if (Keyboard::isKeyPressed(Keyboard::A) && !isDashing) {
        angle -= turnSpeed * deltaTime;
    }
    if (Keyboard::isKeyPressed(Keyboard::D) && !isDashing) {
        angle += turnSpeed * deltaTime;
    }
    if (Keyboard::isKeyPressed(Keyboard::W)) {
        float radians = angle * (M_PI / 180.0f);

        position.x += cos(radians) * speed * deltaTime;
        position.y += sin(radians) * speed * deltaTime;
    }

    dashAbility(deltaTime);

    if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer <= 0) {
        shootTimer = FileMenager::playerData.bullet_shoot_delay;
        float radians = angle * (M_PI / 180.0f);

        Game::addToEntities(new Bullet(position, Vector2f(cos(radians), sin(radians)), angle));
        SoundData::play(Sounds::LASER_SHOOT);
    }

    if (!Keyboard::isKeyPressed(Keyboard::Space) && spriteInfo.spriteLifeTime <= 0) {
        spriteInfo.spriteLifeTime = FileMenager::playerData.sprite_cycle_time;
        spriteState = (spriteState + 1) % spriteInfo.frames.size();
        updateSprite(spriteInfo.sprite, spriteInfo.frames, spriteState);
    }

    collisionDetection();
}

const EntityType Player::getEntityType()
{
	return EntityType::TYPE_PLAYER;
}

void Player::collisionDetection()
{

	position.x = min(max((double)position.x, radius), FileMenager::screenData.size_width - radius);
	position.y = min(max((double)position.y, radius), FileMenager::screenData.size_height - radius);

	for (size_t i = 0; i < Game::entities.size(); i++)
	{
		if (typeid(*Game::entities[i]) == typeid(Asteroid)) {
			Asteroid* asteroid = dynamic_cast<Asteroid*>(Game::entities[i]);

			if (physics::intersects(position, radius, asteroid->position, asteroid->radius)) {
				SoundData::play(Sounds::EXPLOSION);
				Game::gameOver();
			}
		}
	}
}

void Player::dashAbility(const float& deltaTime)
{
    const auto animationDuration = FileMenager::playerData.dash_duration;

    if (Keyboard::isKeyPressed(Keyboard::R) && !isDashing && dashTimer <= 0 && !Keyboard::isKeyPressed(Keyboard::Space)) {
        isDashing = true;
        dashTimer = FileMenager::playerData.dash_time_delay;

        float radians = angle * (M_PI / 180.0f);

        Vector2f endPoint(position.x + cos(radians) * size * FileMenager::playerData.dash_length, position.y + sin(radians) * FileMenager::playerData.dash_length);

        thread animationThread([this, endPoint, animationDuration]() {
            Clock clock;

            while (true) {
                float elapsedTime = clock.getElapsedTime().asSeconds();
                float t = elapsedTime / animationDuration;
                if (t > 1.0f) t = animationDuration;

                Vector2f interpolatedPosition = position + (endPoint - position) * t;
                position = interpolatedPosition;


                if (t >= animationDuration) {
                    isDashing = false;
                    break;
                }

                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });

        animationThread.detach();
    }
}