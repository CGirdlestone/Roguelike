#ifndef PARTICLE_H
#define PARTICLE_H

#include "Components.h"

struct Particle
{
	Particle(double _x, double _y, int _target_x, int _target_y, double vx, double vy, Renderable& renderable, Animation& anim);
	Particle(double _x, double _y, Uint32 lifetime, Uint32 currentLife, Renderable& renderable, Animation& anim);
	~Particle();
	double x, y;
	int target_x, target_y;
	double vx, vy;
	Renderable renderable;
	Animation animation;
	bool is_alive;
	Uint32 lifetime;
	Uint32 currentLife;
	double ax, ay;
};

#endif
