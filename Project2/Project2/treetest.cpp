
//#include <windows.h> 
#include <loadTGA.h>
#include <GL\glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14156
#define RAIN	0
#define SNOW	1
#define	HAIL	2
#define MAX_PARTICLES 10000

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.02;
int loop;
int fall;
static int page = 0;

typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
	// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;
particles par_sys[MAX_PARTICLES];
GLfloat v[5][5];
void drawHalfSphere(int scaley,int scalex, GLfloat r) {
	int i, j;
	GLfloat v[10000][3];
	glColor4f(0.0, 0.0, 1.0, 0.1);
	for (i = 0; i < scalex; ++i) {
		for (j = 0; j < scaley; ++j) {
			v[i*scaley + j][0] = r * cos(j * 2 * PI / scaley)*cos(i*PI / (2 * scalex));
			v[i*scaley + j][1] = r * sin(i*PI / (2 * scalex));
			v[i*scaley + j][2] = r * sin(j * 2 * PI / scaley)*cos(i*PI / (2 * scalex));
		}
	}

	glBegin(GL_QUADS);
	for (i = 0; i < scalex - 1; ++i) {
		for (j = 0; j < scaley; ++j) {
			glVertex3fv(v[i*scaley + j]);
			glVertex3fv(v[i*scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1)*scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1)*scaley + j]);
		}
	}
	glEnd();
}

void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 51) - 20;
	par_sys[i].ypos = 10.0;
	par_sys[i].zpos = (float)(rand() % 51) + 5;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}
void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glPushMatrix();
			glColor3f(0.33, 0.71, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();
			glPopMatrix();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 100);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;


			if (par_sys[loop].ypos <= -1) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
			
		}
	}
}
void drawSnow() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.05, 16, 16);
			glPopMatrix();

			// Update values
			//Move
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}
void drawHail() {
	float x, y, z;

	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.8, 0.8, 0.9);
			glBegin(GL_QUADS);
			// Front
			glVertex3f(x - hailsize, y - hailsize, z + hailsize); // lower left
			glVertex3f(x - hailsize, y + hailsize, z + hailsize); // upper left
			glVertex3f(x + hailsize, y + hailsize, z + hailsize); // upper right
			glVertex3f(x + hailsize, y - hailsize, z + hailsize); // lower left
			//Left
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			// Back
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			//Right
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			//Top
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			//Bottom
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			glEnd();

			// Update values
			//Move
			if (par_sys[loop].ypos <= 0.1) {
				par_sys[loop].vel = par_sys[loop].vel * -0.2;
			}
			
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 100); // * 1000
			par_sys[loop].vel += par_sys[loop].gravity;
			
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}


typedef enum { TREE, STEM, LEAF, WHOLETREE, GROUND } DisplayLists;

float lightness = 30.0, deltaLight = 0.0;
float angle = 1.5, deltaAngle = 0.0, height = 1.25, deltaHeight = 0.0, ratio;
float x = 0.0f, y = 1.5f, z = 6.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;
GLint tree_display_list;
int deltaMove = 0;
float red = 1.0, blue = 1.0, green = 1.0;
float a = 0.0, b = 0.0, r = 6.0;
float tmp;
int Level = 0;
float Scale = 0.5;
int currLevel;
unsigned long k;
float input_rand = (float)rand() / (float)25000; //used for input angle (0-180) degrees

// Variables in the tree
float li; //length of branches
float Ri, ri; // the thickness of the beginning / end of the branches
float vi; //volume of branches
float Mi, mi; // mass held by the giants / mass of branches
float ei; // weight limit
float Gi, gi; //speed of growth for the tree and suckers
float Ji; //child index
unsigned TreeSeed;   //We store the seed so that each level belongs to the same tree
unsigned oneSeed;

float random()
{
	
	return input_rand;
}

void make_seed()
{
	oneSeed = 3361152;
}



void changeSize(int w, int h)
{

	if (h == 0)
		h = 1;

	ratio = 1.0f * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45, ratio, 1, 110);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z,
		0.0f, 1.25f, 0.0f,
		0.0f, 1.0f, 0.0f);
}


/*
 *
Recursive drawing of a tree based on the Oppenheier algorithm (1)
  */

void genTree(int level)
{
	
	currLevel = level;
	long savedseed;
	if ((level >= Level)) {
		glPushMatrix();
		glRotatef(random() * 180, 0, 1, 0);
		glCallList(WHOLETREE);
		glPopMatrix();
		
	}
	else {
		
		glCallList(STEM);
		glPushMatrix();
		glRotatef(random() * 180, 0, 1, 0);
		glTranslatef(0, 0.9, 0);
		glScalef(0.8, 0.8, 0.8);
		
	
		make_seed();
		glPushMatrix();
		glRotatef(110 + random() * 40, 0, 1, 0);
		glRotatef(30 + random() * 20, 0, 0, 1);
		genTree(level + 1);
		glPopMatrix();
		
		make_seed();
		savedseed = (long)random()*ULONG_MAX;
		glPushMatrix();
		glRotatef(-130 + random() * 40, 0, 1, 0);
		glRotatef(30 + random() * 20, 0, 0, 1);
		genTree(level + 1);
		glPopMatrix();
		
		make_seed();
		glPushMatrix();
		glRotatef(-20 + random() * 40, 0, 1, 0);
		glRotatef(30 + random() * 20, 0, 0, 1);
		genTree(level + 1);
		glPopMatrix();
		
		
		glPopMatrix();
	
	}
}


void genLists(void)
{

	GLUquadricObj *cylquad = gluNewQuadric();
	int i;
	
	glNewList(STEM, GL_COMPILE);
	glColor3f(0.2, 0.1, 0.05f);

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	//glNormal3f(1,2,3);
	gluCylinder(cylquad, 0.1, 0.07, 1, 10, 2);
	glPopMatrix();
	glEndList();
	
	glNewList(LEAF, GL_COMPILE);
	glPushMatrix();
	glColor3f(0.0, 0.7, 0.0f);
	for (int i = 0; i < currLevel; i++) glScalef(1.25, 1.25, 1.25);
	glScalef(0.5, 0.5, 0.5);
	glBegin(GL_TRIANGLES);
	// glNormal3f(-0.1, 0, 0.25);  
	glVertex3f(0, 0, 0);
	glVertex3f(0.25, 0.25, 0.1);
	glVertex3f(0, 0.5, 0);
	
	//glNormal3f(0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(-0.25, 0.25, 0.1);
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(WHOLETREE, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(STEM);
	if (currLevel > 0) {
		for (i = 0; i < 3; i++) {
			glTranslatef(0, 0.333, 0);
			glRotatef(90, 0, 1, 0);
			glPushMatrix();
			glRotatef(0, 0, 1, 0);
			glRotatef(50, 1, 0, 0);
			glCallList(LEAF);
			glPopMatrix();
			glPushMatrix();
			glRotatef(180, 0, 1, 0);
			glRotatef(60, 1, 0, 0);
			glCallList(LEAF);
			glPopMatrix();
		}
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();
}

/*
 * List if we wanted to do the whole forest
 */
void CreateTree(float x,float y,float z)
{
	//make_seed();
	genLists();
	glNewList(TREE, GL_COMPILE);
	glPushMatrix();
	glTranslatef(x, y, z);
	
	glScalef(Scale*1.1, Scale*1.2, Scale*1.1);
	genTree(0);
	glPopMatrix();
	glEndList();
}

void bench() {
	glPushMatrix();
	glColor3f(0.0,0.0,0.0);
	glTranslatef(2.0, 0.5, 0.0);
	glScalef(2.0, 0.2, 8.0);
	glutSolidCube(0.2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.83, 0.2, 0.75);
	glScalef(0.2,3.0,0.5);
	glutSolidCube(0.2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.83, 0.2, -0.75);
	glScalef(0.2, 3.0, 0.5);
	glutSolidCube(0.2);
	glPopMatrix();	
	glPushMatrix();
	glTranslatef(2.18, 0.2, -0.75);
	glScalef(0.2, 3.0, 0.5);
	glutSolidCube(0.2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.18, 0.2, 0.75);
	glScalef(0.2, 3.0, 0.5);
	glutSolidCube(0.2);
	glPopMatrix();
}

// Rotation around the axis and height adjustment(we use a co - cylindrical system)
void orientMe(float ang, float height) {

	x = r * cos(ang);
	z = r * sin(ang);
	y = height;
	glLoadIdentity();
	gluLookAt(x, y, z,
		0.0f, 1.25f, 0.0f,
		0.0f, 1.0f, 0.0f);
}


// Navigating the stage (radius change - we use a cylindrical joint)

void moveMeFlat(float ang) {

	x = r * cos(ang);
	z = r * sin(ang);

	glLoadIdentity();
	gluLookAt(x, y, z,
		0.0f, 1.25f, 0.0f,
		0.0f, 1.0f, 0.0f);
}



void ground_wall() {

	int subdiv = 1;
	float back = 1;
	float dn = (2.0 / 3) / subdiv;
	float nx = -1.0 / 3;
	float ny = -1.0 / 3;
	float dst = 1.0 / subdiv;
	float s = 0;
	float t = 0;
	float x = -back;
	float y = -back;
	float dp = (float)2 * back / subdiv;
	glEnable(GL_NORMALIZE);
	glColor3f(0.13f, 0.1f, 0.01f);
	glBegin(GL_QUADS);
	for (int i1 = 0; i1 < subdiv; i1++) {
		for (int i2 = 0; i2 < subdiv; i2++) {
			glNormal3f(nx, ny, -1.0 / 3);
			glTexCoord2f(s, t);
			glVertex3f(x, y, -back);
			glNormal3f(nx + dn, ny, -1.0 / 3);
			glTexCoord2f(s + dst, t);
			glVertex3f(x + dp, y, -back);
			glNormal3f(nx + dn, ny + dn, -1.0 / 3);
			glTexCoord2f(s + dst, t + dst);
			glVertex3f(x + dp, y + dp, -back);
			glNormal3f(nx, ny + dn, -1.0 / 3);
			glTexCoord2f(s, t + dst);
			glVertex3f(x, y + dp, -back);
			nx += dn; x += dp; s += dst;
		}
		nx = -1.0 / 3; x = -back; s = 0;
		ny += dn; y += dp; t += dst;
	}
	glEnd();
}

void CreateGround(void) {
	glNewList(GROUND, GL_COMPILE);
	glScalef(1.0, 0.1, 1.0);
	glPushMatrix();
	ground_wall();
	glRotatef(90, 1, 0, 0);
	ground_wall();
	glRotatef(90, 1, 0, 0);
	ground_wall();
	glRotatef(90, 1, 0, 0);
	ground_wall();
	glRotatef(90, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	ground_wall();
	glRotatef(180, 0, 1, 0);
	ground_wall();
	glPopMatrix();
	glScalef(1.0, 10.0, 1.0);
	glEndList();
}

void skybox(){
	glPushMatrix();
	glColor4f(0.9, 1.0, 0.0,0.1);
	glTranslated(-7.0, -0.0, 0.0);
	glScaled(0.01, 7.0, 7.0);
	glutSolidCube(2);
	glPopMatrix();
}



void drawBitmap(float x, float y, float z, void *f, const unsigned char* s) {
	int i;
	glRasterPos2f(x, y);
	glutBitmapString(f, s);
	printf("Bitmap drawing at %f,%f\n %s\r\n", x, y, s);
}

void drawStroke(const unsigned char* s, float tx, float ty, float tz, float sx, float sy, float sz) {
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(sx, sy, sz);
	glColor3f(1.0, 1.0, 1.0);
	glutStrokeString(GLUT_STROKE_ROMAN, s);
	printf("im in stroke\n %f %f",tx,ty);
	glPopMatrix();
}

void intropage() {
	//drawStroke(reinterpret_cast<const unsigned char *>("Hello!!"), 0, 0, 0,0.5,0.5, 0.08);
	drawBitmap(-1.0,3.5,10, GLUT_BITMAP_HELVETICA_18,
		reinterpret_cast<const unsigned char *>("BANGALORE INSTITUTE OF TECHNOLOGY"));
	drawBitmap(-1.45, 3.25, 10, GLUT_BITMAP_HELVETICA_18,
		reinterpret_cast<const unsigned char *>("DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING"));
	drawBitmap(-1.45, 2.75, 10, GLUT_BITMAP_HELVETICA_18,
		reinterpret_cast<const unsigned char *>("COMPUTER GRAPHICS AND VISUALISATION LAB(15CSL68)"));
	drawBitmap(-1.15, 1.75, 10, GLUT_BITMAP_HELVETICA_18,
	reinterpret_cast<const unsigned char *>("WEATHER SIMULATION AND FRACTAL TREE\n\t             PRESS SPACE TO CONTINUE"));
	drawBitmap(-4.25, -0.5, 10, GLUT_BITMAP_HELVETICA_18,
		reinterpret_cast<const unsigned char *>(" SUBMITTED BY\n\nASHRAY A SRIPATHI-  1BI16CS026\nDARSHAN N-\t                1BI16CS039"));
	drawBitmap(2.250, -0.5, 10, GLUT_BITMAP_HELVETICA_18,
		reinterpret_cast<const unsigned char *>("UNDER THE GUIDANCE OF\nProf. K N Prashanth Kumar\nProf.Chaitra L\nProf.Prathima M G"));

	printf("In intro\n");
}

void renderScene(void) {
	if (page == 0) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glPushMatrix();
		intropage();
		glFlush();
		glPopMatrix();
		glutSwapBuffers();
	}
	else if (page == 1) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (deltaMove == 1) {
			if (r > 4) {
				r = r - 0.08;
				moveMeFlat(angle);
			}
		}
		else if (deltaMove == -1) {
			if (r < 75) {
				r = r + 0.08;
				moveMeFlat(angle);
			}
		}
		if (deltaAngle) {
			angle += deltaAngle;
			orientMe(angle, height);
		}

		if (deltaHeight) {
			height += deltaHeight;
			orientMe(angle, height);
		}

		if (deltaLight) {
			lightness += deltaLight;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		float LightDiffuse[] = { lightness, lightness, lightness, 1.0 };	//diffuse lighting
		float LightPosition[] = { 0.0, 50.0,200.0, 1.0 };		//light position
		float LightDirection[] = { -0.0, 0.0,-100.0 };      // direction
		float Material[] = { 0.5, 0.5, 0.5, -1.0 }; // type of object's material

		glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 100.0);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, LightDirection);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Material);
		

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);


		// We create a tree stand (we abandon the list here)
		glColor3f(0.545f, 0.27f, 0.07f);
		glPushMatrix();
		glRotatef(0.0, 0.0, 1.0, 0.0);
		glTranslatef(0, 0, 0);
		glScalef(30.0, -1.0, 30.0);
		glutSolidSphere(0.2, 100, 100);
		glPopMatrix();

		CreateGround();
		//which particle?
		if (fall == RAIN) {
			drawRain();
		}
		else if (fall == HAIL) {
			drawHail();
		}
		else if (fall == SNOW) {
			drawSnow();
		}

		CreateTree(0, 0, 0);
		CreateTree(1, 0, 1);
		bench();
		//Grid around the tree
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_LINE);
		glColor3f(0.2f, 0.2f, 0.2f);
		glPushMatrix();
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 0.28, 0.33, 0.5f, 15, 15);
		glPopMatrix();
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric, GLU_LINE);
		glColor3f(0.2f, 0.2f, 0.2f);
		glPushMatrix();
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 0.28, 0.33, 0.5f, 15, 15);
		glPopMatrix();
		//skybox();
		drawHalfSphere(100, 100, 6);//draw glass after everything is complete
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glutSwapBuffers();
		gluDeleteQuadric(quadric);
	}

}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
	if (key == 32)
	{
		page = 1;
		glutPostRedisplay();
	}

}

void pressKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		deltaAngle = -0.01f; break;
	case GLUT_KEY_RIGHT:
		deltaAngle = 0.01f; break;
	case GLUT_KEY_UP:
		deltaMove = 1; break;
	case GLUT_KEY_DOWN:
		deltaMove = -1; break;
	case GLUT_KEY_HOME:
		deltaHeight = 0.05f;
		break;
	case GLUT_KEY_END:
		deltaHeight = -0.05f;
		break;
	case GLUT_KEY_PAGE_UP:
		Scale = Scale + 0.1;
		Level++; break;
	case GLUT_KEY_PAGE_DOWN:
		Scale = Scale - 0.1;
		Level--; break;
	case GLUT_KEY_F1:
		lightness += 10;
		break;
	case GLUT_KEY_F2:
		lightness -= 10;
		break;
	case GLUT_KEY_F3:
		deltaLight = -0.02f;
		break;
	}
}

// It ensures smooth movements thanks to keyrepeat

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_HOME:
	case GLUT_KEY_END:
		deltaHeight = 0.0f; break;
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		deltaAngle = 0.0f; break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		deltaMove = 0; break;
	case GLUT_KEY_F2:
	case GLUT_KEY_F3:
		deltaLight = 0.0f; break;
	}
}
int value=0; // menu var

void menu(int num) {
	value = num;
	switch (value) {
	case 1: fall = RAIN;
		glutPostRedisplay();
		break;
	case 2: fall = HAIL;
		glutPostRedisplay();
		break;
	case 3: fall = SNOW;
		glutPostRedisplay();
		break;
	case 4:input_rand = (float)rand() / (float)25000;
		glutPostRedisplay();
		break;
	}
}
void createMenu() {
	glutCreateMenu(menu);
	glutAddMenuEntry("Rain", 1);
	glutAddMenuEntry("Hail", 2);
	glutAddMenuEntry("Snow", 3); 
	glutAddMenuEntry("Randomise", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void init() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}
	
	
}


int main(int argc, char **argv)
{
	// Initial definitions 0.7, 0.08, 0.04
	li = 0.7;
	Ri = 0.08;
	ri = 0.04;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1920, 1080);
	glutCreateWindow("CGV_PROJECT");	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	init();
	createMenu();
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	Level = 1;
	glutReshapeFunc(changeSize);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glutMainLoop();

	return(0);
}
