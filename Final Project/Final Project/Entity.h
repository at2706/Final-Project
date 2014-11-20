#pragma once
#include "Sprite.h"
#include "Utilities.h"
#include "Matrix.h"

class Entity {
public:
	Entity(Sprite *s, float x = 0, float y = 0);
	~Entity();

	GLvoid Update(float elapsed);
	GLvoid FixedUpdate();
	static GLvoid fixedUpdateAll();
	GLvoid Render();
	static GLvoid renderAll();
	GLvoid buildMatrix();
	GLvoid setScale(GLfloat x = 1.0f, GLfloat y = 1.0f);
	GLvoid setRotation(float); //Takes in degree arguement

	GLvoid rotate(GLfloat degree);

	GLvoid moveX();
	GLvoid moveY();

	GLvoid decelerateX();
	GLvoid decelerateY();

	GLvoid moveR();
	GLvoid decelerateR();

	Sprite *sprite;
	
	Matrix matrix;
	Vector position;
	Vector scale;
	float speed;		//Maximum velocity on the x-axis
	Vector velocity;
	Vector acceleration;
	Vector friction;

	vector<float> vertices;
	vector<float> uvs;
	bool visible;

private:
	static list<Entity*> entities;
	list<Entity*>::iterator it;
	Vector rotation;	//in radians
};