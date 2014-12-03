#pragma once
#include "Sprite.h"
#include "Utilities.h"
#include "Matrix.h"

class Entity {
public:
	Entity(Sprite *s, float x = 0, float y = 0);
	Entity(Sprite *s, EntityType t, float x = 0, float y = 0);
	~Entity();

	enum CollisionShape{ BOX, POINT, CIRCLE };
	GLvoid Update(float elapsed);
	GLvoid FixedUpdate();
	static GLvoid fixedUpdateAll();
	GLvoid Render();
	static GLvoid renderAll();
	GLvoid buildMatrix();
	GLvoid setScale(GLfloat x = 1.0f, GLfloat y = 1.0f);
	GLvoid setRotation(float); //Takes in degree arguement
	GLvoid setPosition(GLfloat x, GLfloat y, GLfloat z = 0.0f);

	GLvoid rotate(GLfloat degree);
	GLvoid modHealth(GLfloat amount);
	GLvoid shoot();
	GLvoid suicide();

	GLvoid moveX();
	GLvoid moveY();

	GLvoid decelerateX();
	GLvoid decelerateY();

	GLvoid moveR();
	GLvoid decelerateR();

	Sprite *sprite;
	Sprite *projectile;
	EntityType type;
	CollisionShape shape;

	Matrix matrix;
	Matrix translateMatrix;
	Matrix rotationMatrix;
	Vector position;
	Vector scale;
	GLfloat speed;		//Maximum velocity on the x-axis
	Vector velocity;
	Vector acceleration;
	Vector friction;

	GLboolean isStatic;
	GLboolean isIdle;
	GLboolean visible;
	GLboolean deathMark;

	GLboolean enableGravity;
	GLboolean enableBounce;
	GLboolean revivable;
	GLboolean healthBar;

	GLboolean enableCollisions;
	GLboolean collidedTop;
	GLboolean collidedBottom;
	GLboolean collidedLeft;
	GLboolean collidedRight;
	
protected:
	GLvoid collisionEffectX(Entity *e);
	GLvoid collisionEffectY(Entity *e);

private:
	GLfloat distance(Vector v2, Vector v1);
	GLboolean collidesWith(Entity *e);
	GLvoid collisionPenX();
	GLvoid collisionPenY();
	GLvoid deathEffect();
	
	static list<Entity*> entities;
	static vector<Entity*> killQueue;
	list<Entity*>::iterator it;

	GLvoid renderHealthBar();
	
	Vector rotation;	//in radians

	GLfloat healthMax;
	GLfloat health;

	GLfloat timeAlive;
	GLfloat timeDeath;

};