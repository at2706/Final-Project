#pragma once
#include "Sprite.h"
#include "Utilities.h"
#include "Matrix.h"

class Entity {
public:
	Entity(Sprite *s, float x = 0, float y = 0);
	Entity(Sprite *s, EntityType t);
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
	EntityType type;

	Matrix matrix;
	Matrix rotationMatrix;
	Vector position;
	Vector scale;
	GLfloat speed;		//Maximum velocity on the x-axis
	Vector velocity;
	Vector acceleration;
	Vector friction;

	GLfloat healthMax;
	GLfloat health;

	GLboolean isStatic;
	GLboolean isIdle;
	GLboolean visible;

	GLboolean enableGravity;
	GLboolean enableBounce;
	GLboolean reviveable;
	GLboolean healthBar;

	GLboolean enableCollisions;
	GLboolean collidedTop;
	GLboolean collidedBottom;
	GLboolean collidedLeft;
	GLboolean collidedRight;

private:
	GLboolean collidesWith(Entity *e);
	GLvoid collisionPenX();
	GLvoid collisionPenY();
	GLvoid renderHealthBar();

	static list<Entity*> entities;
	list<Entity*>::iterator it;
	Vector rotation;	//in radians
};