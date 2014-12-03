#include "Weapon.h"


Weapon::Weapon(Sprite *s)
	: sprite(s){
}


Weapon::~Weapon()
{
}


GLvoid Weapon::fire(){
	if (cooldown > fireRate){


	}
}