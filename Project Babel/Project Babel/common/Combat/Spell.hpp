#pragma once

#define FIREBALL 0
#define SPIT 1

#define FINISHED 0
#define RUNNING 1

class Spell
{

	


public:
	Sprite * m_sprite;
	float speed;
	int damage;
	int type;
	glm::vec2 position;
	glm::ivec2 rPosition;
	glm::vec2 scale;
	bool active;
	int direction;
	bool state;
	std::string path;
	glm::vec2 offset;
	glm::vec2 finish;

	inline Spell(int spellID, int damage, glm::vec2 position, glm::vec2 offset, glm::vec2 scale, float speed, int direction)
		: damage(damage), position(position), scale(scale), speed(speed), direction(direction), offset(offset)
	{

		if (spellID == FIREBALL)
			path = "data/spells/fireball.png";

		this->m_sprite = new Sprite();
		this->m_sprite->Load(path.c_str());
		this->active = true;
		this->state = true;
	}

	inline glm::vec2 Operate(Controller *ctrl, ScreenUniformData * u_data, GLboolean **tiles)
	{


		rPosition = glm::ivec2((this->position - offset )/ this->scale);

		if (tiles[rPosition.x][rPosition.y] == 0)
		{

			if (direction == UP)
				{
					position.y -= speed;
				}
			if (direction == DOWN && tiles[rPosition.x][rPosition.y + 1] == 0)
				{
					position.y += speed;
				}
			else if (direction == DOWN && tiles[rPosition.x][rPosition.y + 1] == 1)
			{
				this->active = false;

				rPosition.y++;
				return position;
			}
			if (direction == LEFT)
				{
					position.x -= speed;
				}
			if (direction == RIGHT && tiles[rPosition.x + 1][rPosition.y] == 0)
				{
					position.x += speed;
				}
			else if (direction == RIGHT && tiles[rPosition.x + 1][rPosition.y] == 1)
			{
				this->active = false;
				rPosition.x++;
				return position;
			}

			this->Render(u_data);
		}
		else
		{
			this->active = false;
			return position;
		}
		
	}

	inline void Render(ScreenUniformData * u_data)
	{
		u_data->SetAmbientLight();
		u_data->ApplyMatrix(Translation(this->position)*Scale(this->scale));
		this->m_sprite->Render(0);
	}

};

