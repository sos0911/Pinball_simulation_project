#include <iostream>
#include "GL/freeglut.h"
#include <vector>
#include "ball.cpp"
#define PI 3.14159

using namespace std;

typedef pair<float, float> pff;

// Window screen size
int scr_width = 640;
int scr_height = 640;

int velocityIterations = 8;	//the number of iterations for computing the impulses
int positionIterations = 3;	//the number of iterations for adjusting the position

// �߷º���
pff gravity = { 0.0f, -10.0f };
// �ʱ� �ӵ�(x��)
float init_velo = 3.0f;
// ��ư �������� ��
float button_power = 3.5f;

// �� �մ� �� ��
pair<pff, pff> groundpoint = { {-25.0f, 0.0f}, {25.0f, 0.0f} };
// �� �������
float friction = 0.2f;

// �������׷�
float air_power = 0.5f;

ball main_ball(-1.0f, 30.0f, 3.0f, 1.0f, 0.0f);

// ��ε� ����
vector<pff> paths;


float g_hz = 60.0f;			//frequency
float timeStep = 1.0f / g_hz;

bool bstart = false;
bool left_button = false;
bool right_button = false;

void Dokeyboard(unsigned char key, int x, int y)
{
	// Ű���� �ش� key�� ��� �������� ��
	switch (key)
	{
	case 'p':
		bstart = !bstart;
		break;
	case 'a':
		left_button = true;
		right_button = false;
		break;
	case 'd':
		right_button = true;
		left_button = false;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void DoRelasekey(unsigned char key, int x, int y) {
	// Ű���� �ش� key�� ��� �ö���� ��
	switch (key) {
	case 'a':
		left_button = false;
		break;
	case 'd':
		right_button = false;
		break;
	default:
		break;
	}
	// Ű���� �Է� �������� ���� ������ ���� ����
	glutPostRedisplay();
}

void Render()
{
	// Initialize glut
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluOrtho2D(-25.0f, 25.0f, -5.0f, 45.0f);

	// �� �׸���
	// get position and angle by body(ground)
	pff position = { 0,0 };
	float angle = 0.0f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.first, position.second, 0.0f);	// Translation
	glRotatef(angle, 0.0f, 0.0f, 1.0f);			// Rotation
	glColor3f(0.2f, 0.2f, 0.2f);				// Set color

	//Draw the edge shape with 2 vertices
	glBegin(GL_QUADS);
	glVertex3f(groundpoint.first.first, groundpoint.first.second, 0.0f);
	glVertex3f(groundpoint.first.first, -5.0f, 0.0f);
	glVertex3f(groundpoint.second.first, -5.0f, 0.0f);
	glVertex3f(groundpoint.second.first, groundpoint.second.second, 0.0f);
	glEnd();

	glPopMatrix();

	// �� �׸���
	// �� ���� ���� �� ��ü�� ��� �׸� ���̴�.
	position = main_ball.Pos;
	// ���� ȸ�� ������ �ӵ��� ����ϰ� ��.
	angle = main_ball.angle;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.first, position.second, 0.0f);	// Translation
	glRotatef(angle, 0.0f, 0.0f, 1.0f);			// Rotation
	glColor3f(1.0f, 0.0f, 0.0f);				// Set color

	//Draw the edge shape with 2 vertices
	glBegin(GL_LINES);
	glVertex2d(0.0f, 0.0f);
	glVertex2d(main_ball.radius, 0.0f);
	glEnd();

	// �� �׸���
	glBegin(GL_LINE_STRIP);
	for (double dangle = 0.0f; dangle <= 360.0f; dangle += 1.0f) {
		double degtorad = dangle * PI / 180.0;
		glVertex2f(cos(degtorad) * main_ball.radius, sin(degtorad) * main_ball.radius);
	}
	glEnd();

	glPopMatrix();

	// �� ���� �׸���
	position = { 0,0 };
	angle = 0.0f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.first, position.second, 0.0f);	// Translation
	glRotatef(angle, 0.0f, 0.0f, 1.0f);			// Rotation
	glColor3f(0.0f, 0.0f, 0.0f);				// Set color

	glBegin(GL_POINTS);
	for(int i=0;i<paths.size();i++)
		glVertex2f(paths[i].first, paths[i].second);
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}

void Update(int value)
{
	if (bstart)
	{	// update the simulation
		// ���⿡ �� frame �� ������Ʈ�Ǵ� ���ӵ�.. ���� ���´�.
		// x�����δ� ��� ��ӵ��� ���Ѵ�.

		if (main_ball.Pos.second > groundpoint.first.second + main_ball.radius) {

			// ���� �ٴڿ� ��� ��
			main_ball.Acc.second = gravity.second / main_ball.mass;

			// x�� �������δ� Ư���� �ӵ��� ����
			main_ball.Vel.first = init_velo;
			main_ball.Vel.second += main_ball.Acc.second * timeStep;

			main_ball.Pos.first += main_ball.Vel.first * timeStep;
			main_ball.Pos.second += main_ball.Vel.second * timeStep;
		}
		else {
			// �ٴڿ� ���� ��
			// �¿�θ� �������� �Ѵ�.
			// x�����θ�!!
			// �������ʹ� Ű����� ���ϴ� ���� �޾ƾ� ��.

			float final_power = (1 - friction) * button_power;

			if (right_button) {
				main_ball.Acc.first = final_power / main_ball.mass;
				main_ball.Vel.first += main_ball.Acc.first * timeStep;

				main_ball.Pos.first += main_ball.Vel.first * timeStep;
			}
			else if(left_button){
				main_ball.Acc.first = -final_power / main_ball.mass;
				main_ball.Vel.first += main_ball.Acc.first * timeStep;

				main_ball.Pos.first += main_ball.Vel.first * timeStep;
			}
			else {
				// ���� �ƿ� ���� ����
				// ��� ����д� ����
				// ���⸶���� �𵨸� -> �ӵ��� ������ �������� ��� �����.

				// ���������� �帣�� ��
				if (main_ball.Vel.first > 0) {
					main_ball.Acc.first = -air_power / main_ball.mass;
					main_ball.Vel.first += main_ball.Acc.first * timeStep;

					if (main_ball.Vel.first < 0)
						main_ball.Vel.first = 0;

					main_ball.Pos.first += main_ball.Vel.first * timeStep;
				}
				else if(main_ball.Vel.first < 0){
					main_ball.Acc.first = air_power / main_ball.mass;
					main_ball.Vel.first += main_ball.Acc.first * timeStep;

					if (main_ball.Vel.first > 0)
						main_ball.Vel.first = 0;

					main_ball.Pos.first += main_ball.Vel.first * timeStep;
				}
			}
		}

		// �� �� ���� ���ư��� �ϱ�
		main_ball.angle -= main_ball.Vel.first;
		if (main_ball.angle < 0.0f)
			main_ball.angle = 360.0f;
		else if (main_ball.angle > 360.0f)
			main_ball.angle = 0.0f;

		paths.push_back(main_ball.Pos);

	}

	glutPostRedisplay();
	glutTimerFunc(20, Update, 0);	//Recursive function
}

void Reshape(int _width, int _height)
{
	scr_width = _width;
	scr_height = _height;
	glViewport(0, 0, _width, _height);
}

int main(int argc, char** argv)
{
	// Initialize glut
	glutInitWindowSize(scr_width, scr_height);
	glutInit(&argc, argv);
	glutCreateWindow("opengl_simulation");
	
	glutDisplayFunc(Render);		//If you want to render, Use it.
	glutReshapeFunc(Reshape);		//Reshape by window size
	glutTimerFunc(20, Update, 0);	//Update physics simulation

	glutKeyboardFunc(Dokeyboard);	//If you want to use keyborad event,
	glutKeyboardUpFunc(DoRelasekey);

	glutMainLoop();

	return 0;
}