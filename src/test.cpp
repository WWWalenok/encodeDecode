
#include <string>
#include <vector>

#include "encodeDecode.hpp"

namespace ttt{

struct Vect3f
{
	float x, y, z;
};

struct Unit 
{
	Vect3f pos, vel;

	double hp;

	uint16_t arrmor;

	std::string name;
};
}

__GEN_STRUCT_SERIALIZE_DESERIALIZE(ttt::Unit, ttt_Unit,
	d->pos, 
	d->vel, 
	d->hp,
	d->arrmor,
	d->name);

__GEN_STRUCT_SERIALIZE_DESERIALIZE(ttt::Vect3f, ttt_Vect3f, 
	d->x, 
	d->y, 
	d->z);


#include <iostream>

int main() 
{
	data_t data;
	ttt::Unit a {{10,10,10},{0,0,0},5800,16, "gamma" };
	ttt::Unit b {{50,50,50},{10,0,0},0,0, "betta" };


	Serialize(data, &a);

	Deserialize(data, &b);

	std::cout << b.name;
}
