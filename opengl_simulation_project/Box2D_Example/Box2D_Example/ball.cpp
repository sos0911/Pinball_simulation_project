#include <iostream>
#include "GL/freeglut.h"
#include <vector>

using namespace std;

typedef pair<float, float> pff;

class ball {
public:
	pff Acc, Vel, Pos;
	float radius;
	float mass;
	float angle; // degree�� ǥ��. 360 ������ 0���� clapping

	ball(float x, float y, float _radius, float _mass, float _angle) {
		Pos = { x,y };
		radius = _radius;
		mass = _mass;
		angle = _angle;
		// ���ӵ� ���� update()���� ����
	}
};