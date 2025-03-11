
/*Yagmur Gocmen
ID : 22202755
Sec: 03
Homework -2
************************************/
 //Additional: we can rotate the cannon
//aim: Shoot the UFO's until they reach the block's leg


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 800
#define D2R 0.0174532

#define PI  3.14159265358
#define TIMER_PERIOD 30 //period for the timer
#define TIMER_ON 1 //0: disable , 1: enable
#define MAXBULLET  10
#define MAXENEMY 5
#define TIME 20
#define FIRE_PERIOD 20

// Global variables
bool activeTimer = true,
up = false, down = false, right = false, left = false;

int  winWidth, winHeight; // Window's width and height
int xC, yC, xV = 0, yV = 0; //UFO's and curtains' positions
double size = 0.5; //initial size

double unit = 30;//to make easier the calculations
double angle = 0;//angle of player and bullet

typedef struct point {
	double x, y;
}point_t;

typedef struct player {
	point_t pos;
}player_t; 

double initialX = winWidth / 2 + 3.5 * unit;
double initialBulletX = -winWidth / 5+ 3.5 * unit-11;

player_t pl = { {initialX,-winHeight / 2 + 3 * unit} }; //initializing the start point and angle

typedef struct bullet {
 
	point_t bulPosition = { initialBulletX,0 };
	double bulRad = unit / 3;
	double bulAngle = angle;
	bool moving = false;

}bullet_t;

bullet_t b[MAXBULLET];

typedef struct ufo {
	point_t pos;
}ufo_t;

ufo_t u[MAXENEMY] = { winWidth - 3 * unit,-250 };

//initial values

int currNumofUFO = 1,
totUFO = 0,
hitted =0,
speed = 0;

bool space = false;
double remainingTime = TIME;

bool animation = 0;
int mode = 0;
double perc; //percentage of the success


// To draw a filled circle, centered at (x,y) with radius r
void circle(int x, int y, int r) {
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}



void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, str[i]);
}


void vprint2(int x, int y, float size, const char* string, ...) 
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}


//Drawing stars for background
void drawStars()
{
	int space = 30; //space between stars

	glPointSize(2.0);
	glBegin(GL_POINTS);

	for (int i = -350; i < 500; i +=  space)//for x
	{
		for (int j = -400; j < 400; j += space)//for y
		{
			int x = i + rand() % space;  //stars' x coordinates
			int y = j + rand() % space; //stars' y coordinates
			glVertex2f(x, y); //printing stars
		}
	}
	glEnd();
}


void drawBlock() 
{
	glColor3f(1, 1, 2);
	glRectf(-winWidth / 2 + 2 * unit, -winHeight / 2 + unit, -winWidth / 2 + 2.4 * unit, winHeight / 2 - 1.3 * unit);//left foot of block
	glRectf(-winWidth / 2 + 4.5 * unit, -winHeight / 2 + unit, -winWidth / 2 + 4.9 * unit, winHeight / 2 - 1.3 * unit);//right foot of block
}


void vertex(point_t P, point_t pos, double angle) 
{
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + -winWidth / 2 + 3.2 * unit;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + pos.y;
	glVertex2d(xp, yp);
}

void drawPlayer() 
{

	glColor3f(1, 0, 2);
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-winWidth / 2 + 2.8 * unit, pl.pos.y - 45);
	glVertex2f(-winWidth / 2 + 3.4 * unit, pl.pos.y - 12);
	glVertex2f(-winWidth / 2 + 3.4 * unit, pl.pos.y - 12);
	glVertex2f(-winWidth / 2 + 4 * unit, pl.pos.y - 45);
	glEnd();//stand of cannon

	glColor3f(0.75, 0.75, 0.5);

	glBegin(GL_POLYGON);
	vertex({ 7,18 }, pl.pos, angle * D2R);
	vertex({ 54,12 }, pl.pos, angle * D2R);
	vertex({ 54,-12 }, pl.pos, angle * D2R);
	vertex({ 7, -18 }, pl.pos, angle * D2R);
	glEnd();//turning cannon
	circle(-winWidth / 2 + 3.2 * unit, pl.pos.y, 20);//cannon

	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(-winWidth / 2 + 4 * unit, pl.pos.y - 45);
	glEnd();

}

void drawUFO(ufo_t u)
{
	glColor3f(0.18, 0.999, 0.99); //glass
	circle(u.pos.x + 0 + xV, u.pos.y + -60 * size + yV, 55 * size);

	glBegin(GL_QUADS);	//the bottom side of the ufo
	glColor3f(0.6, 0.97, 0.2); 
	glVertex2f(u.pos.x + -44 * size + xV, u.pos.y + -93 * size + yV);
	glVertex2f(u.pos.x + -160 * size + xV, u.pos.y + -150 * size + yV);
	glVertex2f(u.pos.x + 160 * size + xV, u.pos.y + -150 * size + yV);
	glVertex2f(u.pos.x + 44 * size + xV, u.pos.y + -93 * size + yV);
	glEnd();

	glVertex2f(u.pos.x + (-320 / 7) * size + xV, u.pos.y + -200 * size + yV);
	glVertex2f(u.pos.x + -60 * size + xV, u.pos.y + -240 * size + yV);

	glVertex2f(u.pos.x + 0 + xV, u.pos.y + -220 * size + yV);
	glVertex2f(u.pos.x + 0 + xV, u.pos.y + -260 * size + yV);

	glVertex2f(u.pos.x + (320 / 7) * size + xV, u.pos.y + -200 * size + yV);
	glVertex2f(u.pos.x + 60 * size + xV, u.pos.y + -240 * size + yV);

}


int fire() //controlling the shoot
{
	int i;
	for (i = 0; i < MAXBULLET; i++)
	{
		if (b[i].moving == false)
			return (i);
	}
	return (-1);
}

void drawBullet(int i) 
{
	glColor3ub(123, 176, 128);
	circle(b[i].bulPosition.x, b[i].bulPosition.y, b[i].bulRad);
}

void initializeUFO(ufo* u)
{
	u->pos.x = winWidth / 2;
	u->pos.y = -winHeight / 2 + 5 * unit + rand() % 15 * unit;
}

void finInfo() //info of the last screen
{
	glColor3ub(64, 64, 64);
	glRectf(-winWidth / 3, -winHeight / 3, winWidth / 3, winHeight / 3);
	glColor3ub(255, 255, 255);

	perc = hitted / (totUFO * 1.0);
	vprint2(-winWidth / 3 + 90, winHeight / 3 - 70, 0.2, "Your percentage was  %.1f/100", perc * 100);
	
	vprint2(-winWidth / 3 + 90, -winHeight / 3 + 100, 0.2, "PRESS <F1> TO GIVE ANOTHER TRY");

}

void startInfo() //info's that are in the start screen
{
	glColor3ub(64, 64, 64);
	glRectf(-winWidth / 3, -winHeight / 3, winWidth / 3, winHeight / 3);
	glColor3ub(255, 255, 255);

	vprint2(-winWidth / 3 + 150, -winHeight / 3 + 245, 0.2, "PRESS <F1> TO START");
	vprint2(-winWidth / 3 + 150, -winHeight / 3 + 150, 0.2, "IF YOU WANT TO GET POINTS");
	vprint2(-winWidth / 3 + 150, -winHeight / 3 + 100, 0.2, "YOU HAVE TO DO HEADSHOT!!");
}

void resetAll() //reseting all values
{
	currNumofUFO = 1;
	totUFO = 0;
	speed = 0;
	space = false;
	remainingTime = TIME;
	hitted = 0;
	angle = 0;

	for (int i = 0; i < MAXENEMY; i++)
		u[i] = { winWidth - 3 * unit,-250 };
	for (int j = 0; j < MAXBULLET; j++)
	{
		b[j].bulAngle = angle;
		b[j].bulRad = unit / 2;
		b[j].moving = false;
		b[j].bulPosition.x = -250;
		b[j].bulPosition.y = 0;
	}
	pl = { {initialX , 0} };
}

void drawInfo()
{
	glColor3f(0.3,0,1);
	glRectf(winWidth / 2, winHeight / 2, -winWidth / 2, winHeight / 2 - 100);
	glLineWidth(8);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(winWidth / 2, winHeight / 2);
	glVertex2f(winWidth / 2, winHeight / 2 - 100);
	glVertex2f(-winWidth / 2, winHeight / 2 - 100);
	glVertex2f(-winWidth / 2, winHeight / 2);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(winWidth / 2 - 333, winHeight / 2);
	glVertex2f(winWidth / 2 - 333, winHeight / 2 - 100);
	glVertex2f(-winWidth / 2 + 333, winHeight / 2);
	glVertex2f(-winWidth / 2 + 333, winHeight / 2 - 100);

	glEnd();
	glLineWidth(1);

	vprint2(-winWidth / 2 + 60, winHeight / 2 - 40, 0.2, "Remaining Time");
	vprint2(-winWidth / 2 + 120, winHeight / 2 - 80, 0.2, "%.2f", remainingTime);
	vprint2(-winWidth / 2 + 410, winHeight / 2 - 40, 0.2, "Total UFOs");
	vprint2(-winWidth / 2 + 490, winHeight / 2 - 80, 0.2, "%d", totUFO);
	vprint2(-winWidth / 2 + 750, winHeight / 2 - 40, 0.2, "Hit UFOs", hitted);
	vprint2(-winWidth / 2 + 820, winHeight / 2 - 80, 0.2, "%d", hitted);

	glColor3ub(0, 0, 0);
	glRectf(winWidth / 2, -winHeight / 2, -winWidth / 2, -winHeight / 2 + 40);
	glColor3ub(255, 255, 255);
	vprint2(-winWidth / 2 + 10, -winHeight / 2 + 10, 0.15, "<Space> for shooting,  <up> or <down> or <right> or <left> for aiming, <F1> to give a break");
}

// To display onto window using OpenGL commands
//
void display() {


	//Clear window to black 
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	drawBlock();
	
	drawStars();


	for (int i = 0; i < MAXBULLET; i++)
		if (b[i].moving)
			drawBullet(i);
	drawPlayer();

	for (int i = 1; i < currNumofUFO; i++)
		drawUFO(u[i]);

	drawInfo();
	if (mode == 0)
		startInfo();

	if (remainingTime <= 0)
		finInfo();
	

	glutSwapBuffers();
}

// Key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	if (key == ' ')
		space = true;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		space = false;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}



void onSpecialKeyDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		up = true;
		break;
	case GLUT_KEY_DOWN:
		down = true;
		break;
	case GLUT_KEY_LEFT:
		left = true;
		break;
	case GLUT_KEY_RIGHT:
		right = true;
		break;
	case GLUT_KEY_F1:
		animation = !animation;
		if (remainingTime <= 0)
			resetAll();
		mode = 1;
		break;


	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		up = false;
		break;
	case GLUT_KEY_DOWN:
		down = false;
		break;
	case GLUT_KEY_LEFT:
		left = false;
		break;
	case GLUT_KEY_RIGHT:
		right = false;
		break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onClick(int button, int stat, int x, int y)
{
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y)
{

	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) 
{

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onTimer(int v) 
{

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (remainingTime < 0)
	{
		animation = false;
		remainingTime = 0;
	}
	if (animation && mode)
	{
		if (remainingTime > 0)
			remainingTime -= TIMER_PERIOD / 1000.;

		if (right && angle > -30) 
			angle -= 3;
		

		if (left && angle < 50) 
			angle += 3;
		

		if (up && pl.pos.y < winHeight / 2 - 3 * unit) 
			pl.pos.y += 10;

		if (down && pl.pos.y > -winHeight / 2+3*unit) 
			pl.pos.y -= 10;
		
		if (space && speed == 0)
		{
			int canFired = fire();
			
			if (space && speed == 0)
			{
				int canFired = fire();
				if (canFired != -1) 
				{
					// Calculate the initial position based on player's cannon with an offset for higher position
					double bulletX = -winWidth / 2 + 4 * unit + 20 * cos(angle * D2R);
					double bulletY = pl.pos.y - 45 + 20 * sin(angle * D2R) + 20; // Add an offset of 20 units for higher position

					b[canFired].bulPosition.x = bulletX;
					b[canFired].bulPosition.y = bulletY;
					b[canFired].bulAngle = angle;
					b[canFired].moving = true;
					speed = FIRE_PERIOD;
				}
			}

		}

		if (speed > 0)
			speed--;

		for (int i = 0; i < MAXBULLET; i++) 
		{
			if (b[i].moving)
			{
				b[i].bulPosition.x += 15*cos(b[i].bulAngle * D2R);
				b[i].bulPosition.y +=  15*sin(b[i].bulAngle * D2R);

				if (!(b[i].bulPosition.x < winWidth / 2 + b[i].bulRad && b[i].bulPosition.y < winHeight / 2 + b[i].bulRad && b[i].bulPosition.y > -winHeight / 2 - b[i].bulRad))
					b[i].moving = false;

				int k;
				for (k = 0; k < MAXENEMY; k++)
					if (fabs(b[i].bulPosition.x - u[k].pos.x) < 3 * unit && fabs(b[i].bulPosition.y - u[k].pos.y) < 2 * unit)
					{
						u[k].pos.y += 50 * unit;
						hitted++;
					}
			}
		}

		for (int i = 0; i < currNumofUFO; i++) 
		{ 

			if (u[i].pos.x > -winWidth / 2 - 3 * unit)
				u[i].pos.x -= 5;

			else 
			{
				initializeUFO(&u[i]);
				totUFO++;
			}

			if (currNumofUFO < MAXENEMY && u[i].pos.x < winWidth / 2 - 200 * (currNumofUFO)) 
			{
				currNumofUFO++;
				initializeUFO(&u[currNumofUFO - 1]);
				totUFO++;
			}

			if (u[i].pos.x < -winWidth / 2 + 7 * unit) //if the UFO's reach the block's leg, they disappear
			{
				// Reset UFO position if it hits the block's leg
				initializeUFO(&u[i]);
				totUFO++;
			}
			
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}

void Init() 
{
	srand(time(NULL));

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Protect the World - Yaðmur Göçmen");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// keyboard registration
	
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	// mouse registration
	
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}