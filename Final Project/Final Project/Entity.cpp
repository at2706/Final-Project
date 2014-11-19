#include "Entity.h"

list<Entity*> Entity::entities;
Entity::Entity(Sprite *s, float x, float y) {
	sprite = s;

	position.x = x;
	position.y = y;

	entities.push_back(this);
	it = --entities.end();
}

Entity::~Entity(){
	entities.erase(it);
}

void Entity::Update(float elapsed) {

}

void Entity::FixedUpdate() {

}

void Entity::Render() {

}

void Entity::buildMatrix(){
	Matrix scaleMatrix;
	scaleMatrix.m[0][0] = scale.x;
	scaleMatrix.m[1][1] = scale.y;
	//scaleMatrix.m[2][2] = scale.z;

	Matrix translateMatrix;
	translateMatrix.m[3][0] = position.x;
	translateMatrix.m[3][1] = position.y;
	//translateMatrix.m[3][2] = position.z;

	/*Matrix rotationXMatrix;
	rotationXMatrix.m[1][1] = cos(rotation.x);
	rotationXMatrix.m[2][1] = -sin(rotation.x);
	rotationXMatrix.m[1][2] = sin(rotation.x);
	rotationXMatrix.m[2][2] = cos(rotation.x);

	Matrix rotationYMatrix;
	rotationYMatrix.m[0][0] = cos(rotation.y);
	rotationYMatrix.m[2][0] = sin(rotation.y);
	rotationYMatrix.m[0][2] = -sin(rotation.y);
	rotationYMatrix.m[2][2] = cos(rotation.y);*/

	Matrix rotationZMatrix;
	rotationZMatrix.m[0][0] = cos(rotation.z);
	rotationZMatrix.m[1][0] = -sin(rotation.z);
	rotationZMatrix.m[0][1] = sin(rotation.z);
	rotationZMatrix.m[1][1] = cos(rotation.z);

	matrix = scaleMatrix * rotationZMatrix /** rotationYMatrix * rotationXMatrix*/ * translateMatrix;
}


void Entity::setRotation(GLfloat z){
	rotation.z = (z * PI) / 180.0f;
}

GLvoid Entity::rotate(GLfloat z){
	rotation.z += (z * PI) / 180.0f;
}
GLvoid Entity::moveX(){
	velocity.x += acceleration.x * FIXED_TIMESTEP * cos(rotation.z);

	//Speed Limit
	if (velocity.x > speed)			velocity.x = speed;
	else if (velocity.x < -speed)	velocity.x = -speed;
	position.x += velocity.x * FIXED_TIMESTEP;

}
GLvoid Entity::moveY(){
	velocity.y += acceleration.y * FIXED_TIMESTEP * sin(rotation.z);

	//Speed Limit for y, but it makes gravity weird
	//if (velocity.y > speed * sin(rotation.z)) velocity.y = speed * sin(rotation.z);
	//else if (velocity.y < -speed * fabs(sin(rotation.z))) velocity.y = -speed * fabs(sin(rotation.z));
	position.y += velocity.y * FIXED_TIMESTEP;
}
GLvoid Entity::decelerateX(){
	velocity.x = lerp(velocity.x, 0.0f, FIXED_TIMESTEP * friction.x);
	position.x += velocity.x * FIXED_TIMESTEP;
}
GLvoid Entity::decelerateY(){
	velocity.y = lerp(velocity.y, 0.0f, FIXED_TIMESTEP * friction.y);
	position.y += velocity.y * FIXED_TIMESTEP;
}