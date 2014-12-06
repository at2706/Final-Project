#include "Entity.h"

list<Entity*> Entity::entities;
vector<Entity*> Entity::killQueue;
Entity::Entity(Sprite *s, float x, float y) {
	sprite = s;

	setScale();
	setPosition(x, y);

	healthMax = 100;
	health = 100;
	
	flags.visible = true;
	flags.idle = true;
	collision.enabled = true;
	gravity.enabled = true;
	gravity.y = GRAVITY;
	flags.healthBar = true;

	entities.push_back(this);
	it = --entities.end();
}
Entity::Entity(Sprite *s, EntityType t, float x, float y){
	sprite = s;
	setScale();
	setPosition(x, y);
	flags.visible = true;
	flags.healthBar = true;
	gravity.y = GRAVITY;
	type = t;

	switch (t){
	case HERO:
		healthMax = 500;
		health = 500;
		speed = 2;
		acceleration.x = 4;
		friction.x = 4;
		friction.y = 4;

		flags.revivable = true;
		gravity.enabled = true;
		collision.enabled = true;
		flags.healthBar = true;
		flags.idle = true;
	break;

	case PLATFORM:
	case LADDER:
		flags.moveable = true;
		collision.enabled = true;
		break;

	case PROJECTILE:
		speed = 6;
		velocity.x = 1;
		timeDeath = 2;
		collision.enabled = true;
		shape = POINT;
		break;

	case CRAWLER:
		healthMax = 500;
		health = 500;
		speed = 0.5;
		acceleration.x = 4;
		friction.x = 4;
		friction.y = 4;
		gravity.enabled = true;
		collision.enabled = true;
		flags.healthBar = true;
		setRotation(180.0f);
		break;
	}

	entities.push_back(this);
	it = --entities.end();
}

Entity::~Entity(){
	entities.erase(it);
}

GLfloat Entity::distance(Vector v1, Vector v2){
	GLfloat a = v2.x - v1.x;
	GLfloat b = v2.y - v1.y;
	return (GLfloat)sqrt(a*a + b*b);
}

GLvoid Entity::Update(float elapsed) {
}

GLvoid Entity::FixedUpdate() {
	collision.top = false;
	collision.bottom = false;
	collision.left = false;
	collision.right = false;
	collision.points = false;
	
	if (!flags.moveable){
		if(type != FLYER && type != PROJECTILE)
			gravity.enabled = true;
		if (!flags.idle){
			moveY();
			decelerateY();
			collisionCheckY();
			//tileCollisionY(g);
			moveX();
			decelerateX();
			collisionCheckX();
			//tileCollisionX(g);
		}
		else{
			decelerateY();
			collisionCheckY();
			//tileCollisionY(g);
			decelerateX();
			collisionCheckX();
			//tileCollisionX(g);
		}

		if (gravity.enabled){
			//velocity.x += g->gravity.x * FIXED_TIMESTEP;
			velocity.y += gravity.y * FIXED_TIMESTEP;
		}
	}

	AI();
	if (timeDeath > 0) timeAlive += FIXED_TIMESTEP;
	if (timeDeath < timeAlive) suicide();
}
GLvoid Entity::fixedUpdateAll(){
	for (vector<Entity*>::iterator it = killQueue.begin(); it != killQueue.end(); ++it)
		delete (*it);
	killQueue.erase(killQueue.begin(), killQueue.end());
	for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		(*it)->FixedUpdate();
}

GLvoid Entity::Render() {
	if (flags.visible){

		buildMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.ml);

		sprite->render();

		if (flags.healthBar && health/healthMax < 0.95)
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
GLvoid Entity::setRotation(GLfloat y){
	rotation.y = (y * PI) / 180.0f;
}
GLvoid Entity::setPosition(GLfloat x, GLfloat y, GLfloat z){
	position.x = x;
	position.y = y;
	position.z = z;
}

GLvoid Entity::shoot(){
	Entity *bullet = new Entity(projectile, PROJECTILE, -0.2f, 0.5f);
	bullet->setPosition((sprite->size.x * cos(rotation.y) /2) + position.x, position.y);
	bullet->velocity.x *= cos(rotation.y);
}
GLvoid Entity::suicide(){
	if (!flags.deathMark){
		killQueue.push_back(this);
		flags.deathMark = true;
	}
}

GLboolean Entity::collidesWith(Entity *e){
	if (!collision.enabled || !e->collision.enabled) return false;

	if (shape == BOX){
		GLfloat top = position.y + ((sprite->size.y) / 2);
		GLfloat bot = position.y - ((sprite->size.y) / 2);
		GLfloat left = position.x - ((sprite->size.x) / 2);
		GLfloat right = position.x + ((sprite->size.x) / 2);
		if (e->shape == BOX){
			GLfloat etop = e->position.y + ((e->sprite->size.y) / 2);
			GLfloat ebot = e->position.y - ((e->sprite->size.y) / 2);
			GLfloat eleft = e->position.x - ((e->sprite->size.x) / 2);
			GLfloat eright = e->position.x + ((e->sprite->size.x) / 2);

			return !(bot > etop || top < ebot || left > eright || right < eleft);
		}
		else if (e->shape == CIRCLE){
			GLfloat d1 = distance(position, e->position);
			GLfloat radius = distance(e->position, Vector(e->sprite->size.x, e->sprite->size.y));
			return d1 < radius;
		}
		else if (e->shape == POINT){
			return ((e->position.x > left && e->position.x < right)
				&& (e->position.y > bot && e->position.y < top));
		}
	}
	else if (shape == CIRCLE){
		GLfloat radius = distance(position, Vector(sprite->size.x, sprite->size.y));
		GLfloat d1 = distance(position, e->position);
		return d1 < radius;
	}
	else if (shape == POINT){
		if (e->shape == BOX){
			GLfloat etop = e->position.y + ((e->sprite->size.y) / 2);
			GLfloat ebot = e->position.y - ((e->sprite->size.y) / 2);
			GLfloat eleft = e->position.x - ((e->sprite->size.x) / 2);
			GLfloat eright = e->position.x + ((e->sprite->size.x) / 2);
			return ((position.x > eleft && position.x < eright)
				&& (position.y > ebot && position.y < etop));
		}
		else if (e->shape == CIRCLE){
			GLfloat d1 = distance(position, e->position);
			GLfloat radius = distance(e->position, Vector(e->sprite->size.x, e->sprite->size.y));
			return d1 < radius;
		}
		else if (e->shape == POINT){
			GLfloat d1 = distance(position, e->position);
			return d1 > 0.0001f;
		}
	}
	
	return false;
}
GLboolean Entity::collidesWith(GLfloat x, GLfloat y){
	if (!collision.enabled) return false;
	GLfloat top = position.y + ((sprite->size.y) / 2);
	GLfloat bot = position.y - ((sprite->size.y) / 2);
	GLfloat left = position.x - ((sprite->size.x) / 2);
	GLfloat right = position.x + ((sprite->size.x) / 2);

	return ((x > left && x < right) && (y > bot && y < top));
}

GLvoid Entity::collisionCheckPoints(Entity *e){
	switch (type){
	case CRAWLER:
		GLfloat bottom = position.y - ((sprite->size.y) / 2) - 0.001f;
		GLfloat direction = ((sprite->size.x) / 2) * cos(rotation.y) + position.x + 0.001f;
		collision.points = collision.points || (e->collidesWith(direction, bottom));
		break;
	}
}

GLvoid Entity::collisionCheckX(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		collisionCheckPoints((*it2));
		if (this != (*it2) && collidesWith((*it2))){
			collisionEffectX((*it2));
		}
	}
}
GLvoid Entity::collisionCheckY(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		collisionCheckPoints((*it2));
		if (this != (*it2) && collidesWith((*it2))){
			collisionEffectY((*it2));
		}
	}
}

GLvoid Entity::collisionPenX(Entity *e){
	GLfloat distance_x = fabs(e->position.x - position.x);
	GLfloat width1 = sprite->size.x * 0.5f * scale.x;
	GLfloat width2 = e->sprite->size.x * 0.5f * e->scale.x;
	GLfloat xPenetration = fabs(distance_x - width2 - width1);

	if (position.x > e->position.x){
		position.x += xPenetration + 0.0001f;
		collision.right = true;
		e->collision.left = true;
	}
	else{
		position.x -= xPenetration + 0.0001f;
		collision.left = true;
		e->collision.right = true;
	}
	velocity.x = 0.0f;
}
GLvoid Entity::collisionPenY(Entity *e){
	GLfloat distance_y = fabs(e->position.y - position.y);
	GLfloat height1 = sprite->size.y * 0.5f * scale.y;
	GLfloat height2 = e->sprite->size.y * 0.5f * e->scale.y;
	GLfloat yPenetration = fabs(distance_y - height2 - height1);

	if (position.y > e->position.y){
		position.y += yPenetration + 0.0001f;
		collision.bottom = true;
		e->collision.top = true;
	}
	else{
		position.y -= yPenetration + 0.0001f;
		collision.top = true;
		e->collision.bottom = true;
	}

	velocity.y = flags.bounce ? -velocity.y : 0.0f;
}

GLvoid Entity::collisionEffectX(Entity *e){
	switch (e->type){
	case PROJECTILE:
		if (e->type != PROJECTILE) {
			modHealth(-300);
			suicide();
		}
		break;
	case LADDER:
		break;
	default:
		collisionPenX(e);
	}
}
GLvoid Entity::collisionEffectY(Entity *e){
	switch (e->type){
	case PROJECTILE:
		break;
	case LADDER:
		gravity.enabled = false;
		velocity.y = 0;
		collision.bottom = true;
		break;
	default:
		collisionPenY(e);
	}
}
GLvoid Entity::deathEffect(){

}

GLvoid Entity::AI(){
	switch (type){
	case CRAWLER:
		if (collision.left || collision.right)
			rotate(180.0f);
		if ((collision.bottom && !collision.points))
			rotate(180.0f);
	}
	
}

GLvoid Entity::rotate(GLfloat y){
	rotation.y += (y * PI) / 180.0f;
}
GLvoid Entity::modHealth(GLfloat amount){
	if (!flags.moveable){
		health += amount;
		if (health < 0){
			if (!flags.revivable)
				suicide();
			else health = 0;
		}
	}
}

GLvoid Entity::moveX(){
	velocity.x += acceleration.x * FIXED_TIMESTEP * cos(rotation.y);

	//Speed Limit
	if (velocity.x > speed)			velocity.x = speed;
	else if (velocity.x < -speed)	velocity.x = -speed;

}
GLvoid Entity::moveY(){
	velocity.y += acceleration.y * FIXED_TIMESTEP * sin(rotation.y);

	//Speed Limit for y, but it makes gravity weird
	//if (velocity.y > speed * sin(rotation.z)) velocity.y = speed * sin(rotation.z);
	//else if (velocity.y < -speed * fabs(sin(rotation.z))) velocity.y = -speed * fabs(sin(rotation.z));
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

	GLfloat vertexData2[] = { (-sprite->size.x / 2), 0.1f,
		(-sprite->size.x / 2), 0.08f,
		-(sprite->size.x / 2) + ((sprite->size.x) * (health / healthMax)), 0.08f,
		-(sprite->size.x / 2) + ((sprite->size.x) * (health / healthMax)), 0.1f };

	glColorPointer(4, GL_FLOAT, 0, colorData);
	glVertexPointer(2, GL_FLOAT, 0, vertexData2);

	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}