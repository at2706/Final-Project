#include "Entity.h"

list<Entity*> Entity::entities;
Entity::Entity(Sprite *s, float x, float y) {
	sprite = s;

	setScale();

	position.x = x;
	position.y = y;

	healthMax = 100;
	health = 50;
	
	visible = true;
	isIdle = true;
	enableCollisions = true;
	enableGravity = true;
	healthBar = true;

	entities.push_back(this);
	it = --entities.end();
}
Entity::Entity(Sprite *s, EntityType t){
	switch (t){
	case HERO:

	break;
	}
}

Entity::~Entity(){
	entities.erase(it);
}

GLvoid Entity::Update(float elapsed) {
}

GLvoid Entity::FixedUpdate() {
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;
	if (!isStatic){

		if (!isIdle){
			moveY();
			collisionPenY();
			//tileCollisionY(g);
 			moveX();
			collisionPenX();
			//tileCollisionX(g);
		}
		else{
			decelerateY();
			collisionPenY();
			//tileCollisionY(g);
			decelerateX();
			collisionPenX();
			//tileCollisionX(g);
		}

		if (enableGravity){
			//velocity.x += g->gravity.x * FIXED_TIMESTEP;
			velocity.y += -2.8f * FIXED_TIMESTEP;
		}
	}
}
GLvoid Entity::fixedUpdateAll(){
	for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		(*it)->FixedUpdate();
}

GLvoid Entity::Render() {
	if (visible){

		buildMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.ml);

		sprite->render();

		if (health/healthMax < 0.95)
			renderHealthBar();

		glPopMatrix();
	}
}
GLvoid Entity::renderAll(){
	for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		(*it)->Render();
}

GLvoid Entity::buildMatrix(){
	Matrix scaleMatrix;
	scaleMatrix.m[0][0] = scale.x;
	scaleMatrix.m[1][1] = scale.y;
	scaleMatrix.m[2][2] = scale.z;

	Matrix translateMatrix;
	translateMatrix.m[3][0] = position.x;
	translateMatrix.m[3][1] = position.y;
	translateMatrix.m[3][2] = position.z;

	Matrix rotationXMatrix;
	rotationXMatrix.m[1][1] = cos(rotation.x);
	rotationXMatrix.m[2][1] = -sin(rotation.x);
	rotationXMatrix.m[1][2] = sin(rotation.x);
	rotationXMatrix.m[2][2] = cos(rotation.x);

	Matrix rotationYMatrix;
	rotationYMatrix.m[0][0] = cos(rotation.y);
	rotationYMatrix.m[2][0] = sin(rotation.y);
	rotationYMatrix.m[0][2] = -sin(rotation.y);
	rotationYMatrix.m[2][2] = cos(rotation.y);

	Matrix rotationZMatrix;
	rotationZMatrix.m[0][0] = cos(rotation.z);
	rotationZMatrix.m[1][0] = -sin(rotation.z);
	rotationZMatrix.m[0][1] = sin(rotation.z);
	rotationZMatrix.m[1][1] = cos(rotation.z);

	rotationMatrix = rotationXMatrix * rotationYMatrix * rotationZMatrix;
	matrix = scaleMatrix * rotationMatrix * translateMatrix;
}

GLvoid Entity::setScale(GLfloat x, GLfloat y){
	scale.x = x;
	scale.y = y;
}

GLvoid Entity::setRotation(GLfloat z){
	rotation.y = (z * PI) / 180.0f;
}

GLboolean Entity::collidesWith(Entity *e){
	if (!enableCollisions || !e->enableCollisions) return false;
	GLfloat top = position.y + ((sprite->size.y) / 2);
	GLfloat bot = position.y - ((sprite->size.y) / 2);
	GLfloat left = position.x - ((sprite->size.x) / 2);
	GLfloat right = position.x + ((sprite->size.x) / 2);

	GLfloat etop = e->position.y + ((e->sprite->size.y) / 2);
	GLfloat ebot = e->position.y - ((e->sprite->size.y) / 2);
	GLfloat eleft = e->position.x - ((e->sprite->size.x) / 2);
	GLfloat eright = e->position.x + ((e->sprite->size.x) / 2);

	return !(bot > etop || top < ebot || left > eright || right < eleft);
}

GLvoid Entity::collisionPenX(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		if (this != (*it2) && collidesWith((*it2))){
			GLfloat distance_x = fabs((*it2)->position.x - position.x);
			GLfloat width1 = sprite->size.x * 0.5f * scale.x;
			GLfloat width2 = (*it2)->sprite->size.x * 0.5f * (*it2)->scale.x;
			GLfloat xPenetration = fabs(distance_x - width1 - width2);

			if (position.x > (*it2)->position.x){
				position.x += xPenetration + 0.0001f;
				collidedRight = true;
			}
			else{
				position.x -= xPenetration + 0.0001f;
				collidedLeft = true;
			}

			velocity.x = enableBounce ? -velocity.x : 0.0f;
		}
	}
}
GLvoid Entity::collisionPenY(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		if (this != (*it2) && collidesWith((*it2))){
			GLfloat distance_y = fabs((*it2)->position.y - position.y);
			GLfloat height1 = sprite->size.y * 0.5f * scale.y;
			GLfloat height2 = (*it2)->sprite->size.y * 0.5f * (*it2)->scale.y;
			GLfloat yPenetration = fabs(distance_y - height1 - height2);

			if (position.y > (*it2)->position.y){
				position.y += yPenetration + 0.0001f;
				collidedBottom = true;
			}
			else{
				position.y -= yPenetration + 0.0001f;
				collidedTop = true;
			}

			velocity.y = enableBounce ? -velocity.y : 0.0f;
		}
	}
}

GLvoid Entity::rotate(GLfloat z){
	rotation.z += (z * PI) / 180.0f;
}
GLvoid Entity::moveX(){
	velocity.x += acceleration.x * FIXED_TIMESTEP * cos(rotation.y);

	//Speed Limit
	if (velocity.x > speed)			velocity.x = speed;
	else if (velocity.x < -speed)	velocity.x = -speed;
	position.x += velocity.x * FIXED_TIMESTEP;

}
GLvoid Entity::moveY(){
	velocity.y += acceleration.y * FIXED_TIMESTEP * sin(rotation.y);

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

GLvoid Entity::renderHealthBar(){
	glMultMatrixf(rotationMatrix.inverse().ml);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Color color = { 1 - (health / healthMax), (health / healthMax), 0, 1 };
	GLfloat vertexData[] = { -sprite->size.x / 2, 0.1f,
		-sprite->size.x / 2, 0.08f,
		sprite->size.x / 2, 0.08f,
		sprite->size.x / 2, 0.1f };

	GLfloat colorData[] = { color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a };

	glColorPointer(4, GL_FLOAT, 0, colorData);
	glVertexPointer(2, GL_FLOAT, 0, vertexData);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	GLfloat vertexData2[] = { (-sprite->size.x / 2) + 0.0025f, 0.1f,
		(-sprite->size.x / 2) + 0.0025f, 0.08f,
		-(sprite->size.x / 2) + 0.0025f + ((sprite->size.x) * (health / healthMax)), 0.08f,
		-(sprite->size.x / 2) + 0.0025f + ((sprite->size.x) * (health / healthMax)), 0.1f };

	GLfloat colorData2[] = { color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a };

	glColorPointer(4, GL_FLOAT, 0, colorData2);
	glVertexPointer(2, GL_FLOAT, 0, vertexData2);

	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}