#pragma once
#include "Sprite.h"
#include "Utilities.h"
#include "Matrix.h"
#include "Weapon.h"
struct Flags{
	Flags() : visible(true){};

	GLboolean visible;
	GLboolean moveable;
	GLboolean bounce;
	GLboolean idle;
	GLboolean revivable;
	GLboolean healthBar;
	GLboolean deathMark;
};
struct Gravity{
	Gravity() : enabled(true){};
	GLboolean enabled;
	GLfloat x;
	GLfloat y;
};
struct Collision{
	Collision() : enabled(true){};
	GLboolean enabled;
	GLboolean top;
	GLboolean bottom;
	GLboolean left;
	GLboolean right;
	GLboolean points[5];
	GLfloat distance;
};

enum EntityType { DEFAULT, HERO, PLATFORM, LADDER, FLYER, PROJECTILE, CRAWLER, SHOOTER };
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
	Weapon *weapon;
	EntityType type;
	CollisionShape shape;

	Matrix matrix;
	Matrix translateMatrix;
	Matrix rotationMatrix;
	GLfloat animationElapsed = 0.0f;
	GLfloat framesPerSecond = 30.0f;

	Vector position;
	Vector scale;
	GLfloat speed;		//Maximum velocity on the x-axis
	Vector velocity;
	Vector acceleration;
	Vector friction;

	Flags flags;
	Gravity gravity;
	Collision collision;
	
protected:
	GLvoid collisionEffectX(Entity *e);
	GLvoid collisionEffectY(Entity *e);

private:
	GLfloat distance(Vector v2, Vector v1);
	GLboolean collidesWith(Entity *e);
	GLboolean collidesWith(GLfloat x, GLfloat y);
	GLvoid collisionPenX(Entity *e);
	GLvoid collisionPenY(Entity *e);
	GLvoid collisionCheckPoints(Entity *e);
	GLvoid collisionCheckX();
	GLvoid collisionCheckY();
	GLvoid deathEffect();
	GLvoid AI();
	
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