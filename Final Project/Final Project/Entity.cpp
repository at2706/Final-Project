#include "Entity.h"
#include "GameApp.h"

GameApp *Entity::world = nullptr;
list<Entity*> Entity::entities;
vector<Entity*> Entity::killQueue;
Entity::Entity(vector<Sprite*> *a, float x, float y) {
	animation = a;
	sprite = animation->at(0);
	
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
	animation = nullptr;
	sprite = s;
	setScale();
	setPosition(x, y);
	flags.visible = true;
	flags.healthBar = true;
	flags.moveable = true;
	gravity.y = GRAVITY;
	type = t;

	presets(t);

	entities.push_back(this);
	it = --entities.end();
}
Entity::Entity(vector<Sprite*> *a, EntityType t, float x, float y){
	animation = a;
	sprite = animation->at(0);
	setScale();
	setPosition(x, y);
	flags.visible = true;
	flags.healthBar = true;
	flags.moveable = true;
	gravity.y = GRAVITY;
	type = t;

	presets(t);

	entities.push_back(this);
	it = --entities.end();
}

GLvoid Entity::presets(EntityType t){
	switch (t){
	case HERO:
		healthMax = 200;
		health = 15;
		speed = 3;
		acceleration.x = 8;
		friction.x = 8;
		friction.y = 2;

		flags.revivable = true;
		gravity.enabled = true;
		collision.enabled = true;
		flags.healthBar = true;
		flags.idle = true;
		break;

	case LADDER:
		shape = POINT;
	case PLATFORM:
		flags.moveable = false;
		collision.enabled = true;
		break;

	case PROJECTILE:
		speed = 6;
		velocity.x = 1;
		timeDeath = 2;
		collision.enabled = true;
		gravity.enabled = false;
		shape = POINT;
		break;

	case SHOOTER:
		value += 75;
	case CRAWLER:
		value += 100;
		healthMax = 100;
		health = 100;
		speed = 0.5f;
		acceleration.x = 4;
		friction.x = 4;
		friction.y = 4;
		gravity.enabled = true;
		collision.enabled = true;
		flags.healthBar = true;
		break;
	}
}

Entity::~Entity(){
	entities.erase(it);
}

bool raySegmentIntersect(const Vector &rayOrigin, const Vector &rayDirection, const
	Vector &linePt1, const Vector &linePt2, GLfloat &dist){
	Vector seg1 = linePt1;
	Vector segD;
	segD.x = linePt2.x - seg1.x;
	segD.y = linePt2.y - seg1.y;
	float raySlope = rayDirection.y / rayDirection.x;
	float n = ((seg1.x - rayOrigin.x)*raySlope + (rayOrigin.y - seg1.y)) / (segD.y -
		segD.x*raySlope);
	if (n < 0 || n > 1)
		return false;
	float m = (seg1.x + segD.x * n - rayOrigin.x) / rayDirection.x;
	if (m < 0)
		return false;
	dist = m;
	return true;
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
	memset(collision.points, 0, sizeof collision.points);
	animationElapsed += FIXED_TIMESTEP;
	if (animation != nullptr){
		if (animationIndex >= animation->size()) animationIndex = 0.0f;
		if (animationElapsed > 1.0 / framesPerSecond) {
			sprite = animation->at(animationIndex);
			animationIndex++;
			animationElapsed = 0.0;
			if (animationIndex > animation->size() - 1) {
				animationIndex = 0;
			}
		}
	}

	if (flags.moveable){
		if(type != FLYER && type != PROJECTILE && !flags.deathMark)
			gravity.enabled = true;
		if (weapon != nullptr)
			weapon->cooldown += FIXED_TIMESTEP;
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
	if (timeDeath < timeAlive)
		suicide();
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
	if (weapon->cooldown > weapon->fireRate) {
		Entity *bullet = new Entity(weapon->sprite, PROJECTILE, -0.2f, 0.5f);
		GLfloat randomish = (0.05f * ((float)rand() / (float)RAND_MAX) - 0.025f);
		bullet->setPosition(((sprite->size.x + 0.1f + (fabs(velocity.x) *0.05f)) * cos(rotation.y) / 2) + position.x, position.y + randomish);
		bullet->health = weapon->damage;
		bullet->velocity.x = weapon->speed * cos(rotation.y);
		weapon->cooldown = 0.0f;
	}
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
		GLfloat top = position.y + (scale.y * (sprite->size.y) / 2);
		GLfloat bot = position.y - (scale.y * (sprite->size.y) / 2);
		GLfloat left = position.x - (scale.x * (sprite->size.x) / 2);
		GLfloat right = position.x + (scale.x * (sprite->size.x) / 2);
		if (e->shape == BOX){
			GLfloat etop = e->position.y + (e->scale.y * (e->sprite->size.y) / 2);
			GLfloat ebot = e->position.y - (e->scale.y * (e->sprite->size.y) / 2);
			GLfloat eleft = e->position.x - (e->scale.x * (e->sprite->size.x) / 2);
			GLfloat eright = e->position.x + (e->scale.x * (e->sprite->size.x) / 2);

			return !(bot > etop || top < ebot || left > eright || right < eleft);
		}
		else if (e->shape == CIRCLE){
			GLfloat d1 = distance(position, e->position);
			GLfloat radius = distance(e->position, Vector(e->scale.x *sprite->size.x, e->scale.y * e->sprite->size.y));
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
			GLfloat etop = e->position.y + (e->scale.y * (e->sprite->size.y) / 2);
			GLfloat ebot = e->position.y - (e->scale.y * (e->sprite->size.y) / 2);
			GLfloat eleft = e->position.x - (e->scale.x * (e->sprite->size.x) / 2);
			GLfloat eright = e->position.x + (e->scale.x * (e->sprite->size.x) / 2);
			return ((position.x > eleft && position.x < eright)
				&& (position.y > ebot && position.y < etop));
		}
		else if (e->shape == CIRCLE){
			GLfloat d1 = distance(position, e->position);
			GLfloat radius = distance(e->position, Vector(e->scale.x *sprite->size.x, e->scale.y * e->sprite->size.y));
			return d1 < radius;
		}
		else if (e->shape == POINT){
			GLfloat d1 = distance(position, e->position);
			return d1 < 0.001f;
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
	case HERO:
		if (!collision.points[0] && e->flags.deathMark && e->type == HERO){
			GLfloat direction = (((sprite->size.x) / 2) + 0.02f) * cos(rotation.y) + position.x;
			collision.points[0] = e->collidesWith(direction, position.y) || collidesWith(e);
			if (collision.points[0])
				world->players[value].target = e;
		}
		break;
	case SHOOTER:
		if (e->type == HERO && !e->flags.deathMark){
			GLfloat range = 4.0f;
			GLfloat top = e->position.y + ((e->sprite->size.y) / 2);
			GLfloat bot = e->position.y - ((e->sprite->size.y) / 2);
			GLfloat direction1 = ((sprite->size.x) / 2) + range * cos(rotation.y) + position.x;
			GLfloat distance;
			collision.points[1] = collision.points[1] || raySegmentIntersect(position, Vector(direction1, position.y + 0.2f), Vector(e->position.x, top), Vector(e->position.x, bot), distance);
		}
	case CRAWLER:
		if (!e->flags.deathMark){
			GLfloat bottom1 = position.y - ((sprite->size.y) / 2) - 0.001f;
			GLfloat direction = (((sprite->size.x) / 2) + 0.02f) * cos(rotation.y) + position.x;
			collision.points[0] = collision.points[0] || (e->collidesWith(direction, bottom1));
		}
		break;
	}
}

GLvoid Entity::collisionCheckX(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		collisionCheckPoints((*it2));
		if (this != (*it2) && collidesWith((*it2))){
			(*it2)->collisionEffectX(this);
			collisionEffectX((*it2));
			
		}
	}
}
GLvoid Entity::collisionCheckY(){
	list<Entity*>::iterator end = entities.end();
	for (list<Entity*>::iterator it2 = entities.begin(); it2 != end; ++it2){
		collisionCheckPoints((*it2));
		if (this != (*it2) && collidesWith((*it2))){
			(*it2)->collisionEffectX(this);
			collisionEffectY((*it2));
		}
	}
}

GLvoid Entity::collisionPenX(Entity *e){
	if (flags.moveable && !flags.deathMark && !e->flags.deathMark)
	{
		GLfloat distance_x = fabs(e->position.x - position.x);
		GLfloat width1 = sprite->size.x * 0.5f * scale.x;
		GLfloat width2 = e->sprite->size.x * 0.5f * e->scale.x;
		GLfloat xPenetration = fabs(distance_x - width2 - width1);

		if (position.x > e->position.x){
			position.x += xPenetration + 0.0001f;
			collision.left = true;
			e->collision.right = true;
		}
		else{
			position.x -= xPenetration + 0.0001f;
			collision.right = true;
			e->collision.left = true;
		}
		velocity.x = 0.0f;
	}
}
GLvoid Entity::collisionPenY(Entity *e){
	if (flags.moveable)
	{
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
}

GLvoid Entity::collisionEffectX(Entity *e){
	switch (e->type){
	case SHOOTER:
	case CRAWLER:
		if (flags.moveable && !flags.deathMark){
			velocity.y = 2.0f;
			if (collision.left) setRotation(180.0f);
			else setRotation(0.0f);
			velocity.x = -cos(rotation.y) * 2;
			modHealth(-10);
		}
		break;
	case PROJECTILE:
		if (type != PROJECTILE && type != LADDER && !flags.deathMark) {
			modHealth(-e->health);
			e->suicide();
		}
		break;
	case LADDER:
		break;
	default:
		if (type != PROJECTILE || !(flags.moveable && e->flags.moveable))
		collisionPenX(e);
	}
}
GLvoid Entity::collisionEffectY(Entity *e){
	switch (e->type){
	case SHOOTER:
	case CRAWLER:
		if (flags.moveable && !flags.deathMark){
			velocity.y = 1.0f;
			velocity.x = -cos(rotation.y);
			modHealth(-10);
		}
		break;
	case PROJECTILE:
		break;
	case LADDER:
		if (type != PROJECTILE) {
			gravity.enabled = false;
			velocity.y = 0;
			//collision.bottom = true;
		}
		break;
	default:
		if (type != PROJECTILE && !(flags.moveable && e->flags.moveable))
			collisionPenY(e);
	}
}
GLvoid Entity::deathEffect(){
	switch (type){
	case HERO:
		animation = world->animHeroDead;
		break;

	default:
		;
	}
}

GLvoid Entity::AI(){
	switch (type){
	case HERO:
		
		break;
	case SHOOTER:
		if (collision.points[1]){
			shoot();
			flags.idle = true;
		}
		else flags.idle = false;
	case CRAWLER:
		if (collision.left || collision.right || (collision.bottom && !collision.points[0]))
			rotate(180.0f);
		break;
	}
}

GLvoid Entity::rotate(GLfloat y){
	rotation.y += (y * PI) / 180.0f;
	if (rotation.y > PI) rotation.y = fmod(rotation.y, PI);
}
GLvoid Entity::modHealth(GLfloat amount){
	if (flags.moveable){
		health += amount;
		if (health < 0){
			if (!flags.revivable)
				suicide();
			else {
				flags.deathMark = true;
				flags.idle = true;
				velocity.x = 0.0f;
				velocity.y = 0.0f;
			}
			health = 0;
			deathEffect();
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
	//if (velocity.y > 6) velocity.y = 6;
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
	Color color = { 1.2f - (health / healthMax), (health / healthMax), 0, 1 };
	GLfloat vertexData[] = { -0.0725f / 2, 0.085f,
		-0.0725f / 2, 0.07f,
		0.0725f / 2, 0.07f,
		0.0725f / 2, 0.085f };

	GLfloat colorData[] = { color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a };

	glColorPointer(4, GL_FLOAT, 0, colorData);
	glVertexPointer(2, GL_FLOAT, 0, vertexData);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	GLfloat vertexData2[] = { (-0.0725f / 2), 0.085f,
		(-0.0725f / 2), 0.07f,
		-(0.0725f / 2) + (0.0725f * (health / healthMax)), 0.07f,
		-(0.0725f / 2) + (0.0725f * (health / healthMax)), 0.085f };

	glColorPointer(4, GL_FLOAT, 0, colorData);
	glVertexPointer(2, GL_FLOAT, 0, vertexData2);

	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}