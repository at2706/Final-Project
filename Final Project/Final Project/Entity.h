#pragma once
#include "Sprite.h"

#include "Utilities.h"
#include "Matrix.h"

class Entity {
public:
	Entity(Sprite *s, float x, float y);
	~Entity();

	void Update(float elapsed);
	void FixedUpdate();
	void Render();
	void buildMatrix();
	void setRotation(float); //Takes in degree arguement

	void rotate(GLfloat degree);

	void moveX();
	void moveY();

	void decelerateX();
	void decelerateY();

	void moveR();
	void decelerateR();

	Sprite *sprite;
	
	Matrix matrix;
	Vector position;
	Vector scale;
	float speed;		//Maximum velocity on the x-axis
	Vector velocity;
	Vector acceleration;
	Vector friction;

	unsigned int texture;

	std::vector<float> vertices;
	std::vector<float> uvs;
	bool visible;

private:
	static list<Entity*> entities;
	list<Entity*>::iterator it;
	Vector rotation;	//in radians
};