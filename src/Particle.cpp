#include "Particle.h"
#include "cmath"

Particle::Particle(double _x, double _y, int _target_x, int _target_y, double _vx, double _vy, Renderable& _renderable, Animation& _anim, const std::string& _effect):
	x(_x), y(_y), target_x(_target_x), target_y(_target_y), vx(_vx), vy(_vy), renderable(_renderable), animation(_anim), is_alive(true), lifetime(500), currentLife(0), effect(_effect)
{
	double theta = std::atan2((double)(_target_y - _y) + 0.5, (double)(_target_x - _x) + 0.5);

	ax = std::cos(theta);
	ay = std::sin(theta);
}

Particle::Particle(double _x, double _y, Uint32 _lifetime, Uint32 _currentLife, Renderable& _renderable, Animation& _anim, const std::string& _effect):
	x(_x), y(_y), target_x(-1), target_y(-1), vx(0), vy(0), renderable(_renderable), animation(_anim), is_alive(true), lifetime(_lifetime), currentLife(_currentLife), ax(0), ay(0), effect(_effect)
{

}

Particle::~Particle()
{

}
