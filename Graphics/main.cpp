
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"

const double PI = 3.14;
const int GSZ = 100;

const int H = 600;
const int W = 600;

double angle = 0;


typedef struct {
	double x, y, z;
} POINT3;

//roof color global
typedef struct {
	double red, green, blue;
} COLOR;
bool isCapturedRoofColor = false;
double xRoofColor = 5;
COLOR roofColors[3] = { {100.0/255.0, 169.0 / 255.0, 169.0 / 255.0} , {178.0 / 255.0, 34.0 / 255.0, 34.0 / 255.0}, {110.0/255.0, 42.0/255.0, 42.0 / 255.0}};
int indexRoofColors = 0; //0 or 1 or 2

//stores global
int numStores = 1;
bool isCapturedStores = false;
double xStores = 5;

//windows global
int numWindows = 1;
bool isCapturedWindows = false;
double xWindows = 5;

POINT3 eye = {2,26,25 };

double sight_angle = PI;
POINT3 sight_dir = {sin(sight_angle),0,cos(sight_angle)}; // in plane X-Z
double speed = 0;
double angular_speed = 0;

double ground[GSZ][GSZ] = { 0 };


void init()
{
	//                 R     G    B
	glClearColor(0.5,0.7,1,0);// color of window background

	glEnable(GL_DEPTH_TEST);

	int i, j;

	for (i = 0;i < GSZ;i++)
		for (j = 0;j < GSZ;j++)
		{
			ground[i][j] = 0;
		}
}

void DrawFloor()
{
	int i,j;

	glColor3d(0, 1, 0.3);

	for(i=1;i<GSZ;i++)
		for (j = 1;j < GSZ;j++)
		{
			glColor3d(0.0, 0.8,0.0);
			glBegin(GL_POLYGON);
			glVertex3d(j-GSZ/2, ground[i][j], i-GSZ/2);
			glVertex3d(j - GSZ / 2, ground[i - 1][j], i - 1 - GSZ / 2);
			glVertex3d(j - 1 - GSZ / 2, ground[i - 1][j - 1], i - 1 - GSZ / 2);
			glVertex3d(j - 1 - GSZ / 2, ground[i ][j - 1], i - GSZ / 2);
			glEnd();
		}
}

// cone
void DrawCone(int n, double topr, double bottomr, double topY, double downY)
{
	double alpha, teta = 2 * PI / n;
	COLOR currentColor = roofColors[indexRoofColors]; // Store the initial color
	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(currentColor.red, currentColor.green, currentColor.blue);;
		glVertex3d(topr * sin(alpha), topY, topr * cos(alpha));// 1
		glVertex3d(topr * sin(alpha + teta), topY, topr * cos(alpha + teta)); //2
		glVertex3d(bottomr * sin(alpha + teta), downY, bottomr * cos(alpha + teta));// 3
		glVertex3d(bottomr * sin(alpha), downY, bottomr * cos(alpha)); //4
		glEnd();

		// Update the current color for the next polygon
		currentColor.red -= 0.05;
		currentColor.green -= 0.05;
		currentColor.blue -= 0.05;
	}
}

// n = 4 is squre
void DrawCylinder(int n, double topY, double downY)
{
	COLOR wallColor = { 244.0 / 255.0, 164.0 / 255.0, 96.0 / 255.0 };
	double alpha, teta = 2 * PI / n;
	for (alpha = 0; alpha < 2 * PI;alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(wallColor.red, wallColor.green, wallColor.blue);
		glVertex3d(sin(alpha), topY, cos(alpha));
		glVertex3d(sin(alpha + teta), topY, cos(alpha + teta));
		glVertex3d(sin(alpha + teta), downY, cos(alpha + teta));
		glVertex3d(sin(alpha), downY, cos(alpha));
		glEnd();
		wallColor.red -= 0.05;
		wallColor.green -= 0.05;
		wallColor.blue -= 0.05;
	}
}

void drawWindows(int numWindows, int numFloor, COLOR * windowColor) {
	double radius = 1.01;
	int parts = (numWindows * 2) + 1;
	double alpha = 0, teta = PI / 2;
	double x2 = sin(alpha + teta);
	double y2 = 1;
	double z2 = cos(alpha + teta);
	double x1 = sin(alpha);
	double y1 = 1;
	double z1 = cos(alpha);
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
	double size = distance / parts;
	double dx = (x2 - x1) / parts;
	double dz = (z2 - z1) / parts;
	for (int i = 0; i < parts; i++ ) {
		if (i % 2 == 1) {
			glBegin(GL_POLYGON);
			glColor3d(windowColor->red, windowColor->green, windowColor->blue);
			glVertex3d(radius * (x1 + (i * dx)), numFloor - 0.33, radius * (z1 + (i * dz)));// left top
			glVertex3d(radius * (x1 + ((i + 1) * dx)), numFloor - 0.33, radius * (z1 + ((i + 1) * dz)));//right top
			glVertex3d(radius * (x1 + ((i + 1) * dx)), numFloor - 0.66, radius * (z1 + ((i + 1) * dz)));//right bottom
			glVertex3d(radius * (x1 + (i * dx)), numFloor - 0.66, radius * (z1 + (i * dz)));//left bottom
			glEnd();
		}
	}
}




void drawFloorBilding(int numFloor, int numWindows) {
	//high layer
	DrawCylinder(4, numFloor - 0.00, numFloor - 0.33);
	//windows layer
	DrawCylinder(4, numFloor -  0.33, numFloor - 0.66);
	//bottom layer 
	DrawCylinder(4, numFloor - 0.66, numFloor - 1.00);
	
	//draw windows
	COLOR windowColor = { 0.0, 0.0, 0.8 };
	for (int i = 0; i <= 360; i += 90) {
		glPushMatrix();
		glRotated(i, 0, 1, 0);
		drawWindows(numWindows, numFloor, & windowColor);
		glPopMatrix();
		windowColor.blue -= 0.05;
	}
}

void drawHouse(int numFloors,int numWindows) {
	//draw walls
	for (int i = 1; i <= numFloors; i++) {
		drawFloorBilding(i, numWindows);
	}

	// draw roof
	DrawCone(4, 0, 1, numFloors + 1.00, numFloors +0.00);
}

void roofPartSlider() {
	//title
	glColor3d(1.0, 1.0, 1.0);
	glRasterPos2d(29, 90);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'R');
	glRasterPos2d(41, 90);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'o');
	glRasterPos2d(52, 90);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'o');
	glRasterPos2d(63, 90);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'f');

	glColor3d(169.0/255.0, 169.0 / 255.0, 169.0 / 255.0);
	glBegin(GL_POLYGON);
	glVertex2d(0, 80);
	glVertex2d(100, 80);
	glVertex2d(100, 70);
	glVertex2d(0, 70);
	glEnd();
	
	// the line
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(5, 75);
	glVertex2d(95, 75);
	glEnd();

	//button
	glColor3d(139.0 / 255.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2d(xRoofColor, 77);
	glVertex2d(xRoofColor + 4, 77);
	glVertex2d(xRoofColor + 4, 73);
	glVertex2d(xRoofColor, 73);
	glEnd();
}

void storesPartSlider() {
	//title
	glColor3d(1.0, 1.0, 1.0);
	glRasterPos2d(20, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'N');
	glRasterPos2d(30, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'u');
	glRasterPos2d(37, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'm');
	glRasterPos2d(50, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'S');
	glRasterPos2d(58, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 't');
	glRasterPos2d(63, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'o');
	glRasterPos2d(70, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'r');
	glRasterPos2d(75, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'e');
	glRasterPos2d(81, 60);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 's');
	//the number
	glRasterPos2d(48, 50);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, numStores + '0');
	

	glColor3d(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0);
	glBegin(GL_POLYGON);
	glVertex2d(0, 45);
	glVertex2d(100, 45);
	glVertex2d(100, 35);
	glVertex2d(0, 35);
	glEnd();

	// the line
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(5, 40);
	glVertex2d(95, 40);
	glEnd();

	//button
	glColor3d(139.0 / 255.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2d(xStores, 42);
	glVertex2d(xStores + 4, 42);
	glVertex2d(xStores + 4, 38);
	glVertex2d(xStores, 38);
	glEnd();
}

void windowsPartSlider() {
	//title
	glColor3d(1.0, 1.0, 1.0);
	glRasterPos2d(15, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'N');
	glRasterPos2d(24, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'u');
	glRasterPos2d(30, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'm');
	glRasterPos2d(44, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'W');
	glRasterPos2d(55, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'i');
	glRasterPos2d(58, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'n');
	glRasterPos2d(64, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'd');
	glRasterPos2d(70, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'o');
	glRasterPos2d(76, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 'w');
	glRasterPos2d(84, 30);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 's');
	//the number
	glRasterPos2d(48, 20);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, numWindows + '0');

	glColor3d(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0);
	glBegin(GL_POLYGON);
	glVertex2d(0, 10);
	glVertex2d(100, 10);
	glVertex2d(100, 0);
	glVertex2d(0, 0);
	glEnd();

	// the line
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(5, 5);
	glVertex2d(95, 5);
	glEnd();

	//button
	glColor3d(139.0 / 255.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2d(xWindows, 7);
	glVertex2d(xWindows + 4, 7);
	glVertex2d(xWindows + 4, 3);
	glVertex2d(xWindows, 3);
	glEnd();
}

void DrawSlider()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//background slider
	glColor3d(roofColors[indexRoofColors].red, roofColors[indexRoofColors].blue, roofColors[indexRoofColors].green); // transparent gray
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(0, 100);
	glVertex2d(100, 100);
	glVertex2d(100, 0);
	glEnd();
	glDisable(GL_BLEND);

	//roof part
	roofPartSlider();

	//stores part
	storesPartSlider();

	//windows part
	windowsPartSlider();
}

// put all the drawings here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-buffer
	glViewport(0, 0, W, H);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // unity matrix of projection

	glFrustum(-1, 1, -1, 1, 0.75, 300);
	gluLookAt(eye.x, eye.y, eye.z,  // eye position
		eye.x+ sight_dir.x, eye.y-0.3, eye.z+sight_dir.z,  // sight dir
		0, 1, 0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // unity matrix of model

	DrawFloor();

	glPushMatrix();
	
	glScaled(5, 5, 5);
	drawHouse(numStores, numWindows);//mid bottom always (0, 0, 0) to changed translate this with tranformation
	glPopMatrix();

	// slider
	glViewport(500, 0, 100, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // unity matrix of projection
	glOrtho(0, 100, 0, 100, -1, 1);  // 2d
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // unity matrix of projection
	glDisable(GL_DEPTH_TEST);
	DrawSlider();
	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers(); // show all
}

void idle() 
{
	int i, j;
	double dist;
	angle += 0.02;

	// ego-motion  or locomotion
	sight_angle += angular_speed;
	// the direction of our sight (forward)
	sight_dir.x = sin(sight_angle);
	sight_dir.z = cos(sight_angle);
	// motion
	eye.x += speed * sight_dir.x;
	eye.y += speed * sight_dir.y;
	eye.z += speed * sight_dir.z;


	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (x >= 505 && x <= 595 && y <= 600 - (6 * 38) && y >= 600 - (6 * 42))//y is button the y of button 0 up 600 down    42 and 38 are the y in the slider
			isCapturedStores = true;
		else if (x >= 505 && x <= 595 && y <= 600-(6*3) && y >= 600 - (6*7))//y is button the y of button 0 up 600 down       7 and 3 are the y in the slider
			isCapturedWindows = true;
		else if (x >= 505 && x <= 595 && y <= 600 - (6 * 73) && y >= 600 - (6 * 77))//y is button the y of button 0 up 600 down     77 and 73 are the y in the slider
			isCapturedRoofColor = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isCapturedStores = false; // release mouse
		isCapturedWindows = false; // release mouse
		isCapturedRoofColor = false;// release mouse
	}
}

void mouseMotion(int x, int y)
{
	// updates stores
	if (isCapturedStores)
	{
		if (x >= 505 && x <= 527.5) {
			numStores = 1;
			xStores = x % 500;
		}
		else if (x >= 527.5 && x <= 550) {
			numStores = 2;
			xStores = x % 500;
		}
		else if (x >= 550 && x <= 572.5) {
			numStores = 3;
			xStores = x % 500;
		}
		else if (x >= 572.5 && x <= 595) {
			numStores = 4;
			xStores = x % 500;
		}
	}

	// updates windows
	else if (isCapturedWindows)
	{
		if (x >= 505 && x <= 527.5) {
			numWindows = 1;
			xWindows = x % 500;
		}
		else if (x >= 527.5 && x <= 550) {
			numWindows = 2;
			xWindows = x % 500;
		}
		else if (x >= 550 && x <= 572.5) {
			numWindows = 3;
			xWindows = x % 500;
		}
		else if (x >= 572.5 && x <= 595) {
			numWindows = 4;
			xWindows = x % 500;
		}
	}

	// updates roof color
	else if (isCapturedRoofColor)
	{
		if (x >= 505 && x <= 535) {
			indexRoofColors = 0;
			xRoofColor = x % 500;
		}
		else if (x >= 535 && x <= 565) {
			indexRoofColors = 1;
			xRoofColor = x % 500;
		}
		else if (x >= 565 && x <= 595) {
			indexRoofColors = 2;
			xRoofColor = x % 500;
		}
	}
}

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: // turns the user to the left
		angular_speed += 0.0001;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.0001;
		break;
	case GLUT_KEY_UP: // increases the speed
		speed+= 0.0001;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.0001;
		break;
	case GLUT_KEY_PAGE_UP:
		eye.y += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eye.y -= 0.1;
		break;

	}
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	init();

	glutMainLoop();
}