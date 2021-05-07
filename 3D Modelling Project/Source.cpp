// Task Topic: 3D Modelling Project
// Study module: CPT 205
// Student Name: Zhao Zishen

//  Main body of code with necessary comments
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h> 
#include <iostream> 
#include <GL/glut.h>

GLubyte* pixeldata;
GLint imagewidth;
GLint imageheight;
GLint pixellength;
GLuint	texture[9];

float camera_x = 0;
float camera_y = 150;
float camera_z = -300;
float target_x = 0;
float target_y = 150;
float target_z = 0;
float farmer_x = 0;
float farmer_y = 30;
float farmer_z = 0;
float light = 0.9;
int angle = 0;
int leg = 0;
int leg_sign = 1;
int animal_x = -200;
int time_interval = 100;




void swing_leg(void) {
	if (leg_sign > 0) { // leg swings front
		if (leg < 30) {
			leg += 4;
		}
		else {
			leg_sign = -1;
		}
	}
	if (leg_sign < 0) { // leg swings back
		if (leg > -30) {
			leg -= 4;
		}
		else {
			leg_sign = 1;
		}
	}
}

void draw_tree(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	// leaves
	glPushMatrix();
	glTranslatef(0.0, 200.0, 0.0);
	glScalef(120.0, 130.0, 120.0);
	glColor3f(0.0, 0.7, 0.1);
	glutSolidIcosahedron();
	glColor3f(0.0, 0.0, 0.0);
	glutWireIcosahedron();
	glPopMatrix();
	// trunk
	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	glScalef(30.0, 100.0, 30.0);
	glColor3f(0.7, 0.3, 0.0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	// apples
	glTranslatef(0.0, 200.0, -120);
	glScalef(5.0, 7.0, 5.0);
	glColor3f(1, 0.0, 0.0);
	glutSolidSphere(1,10,10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-80, 250.0, -40);
	glScalef(5.0, 7.0, 5.0);
	glColor3f(1, 0.0, 0.0);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
	
	glPopMatrix();
}

void when_in_mainloop()
{   
	glutPostRedisplay();
}

void OnTimer(int value) {
	if (animal_x < 2100) {
		animal_x += 10;
	}
	else {
		animal_x = -2100;
	}
	glutTimerFunc(time_interval, OnTimer, 1);
}

void keyboard_input(unsigned char key, int x, int y) {
	if (key == 's' || key == 'S')
		if (camera_z > -2000) {
			camera_z -= 10; // Camera move back
			farmer_z -= 10; // famer walk back
			target_z -= 10;
			swing_leg();
		}

	if (key == 'w' || key == 'W')
		if (camera_z < 2000) {
			camera_z += 10; // Camera move front
			farmer_z += 10; // famer walk front
			target_z += 10;
			swing_leg();
		}
	if (key == 'd' || key == 'D')
		if (camera_x > -2000) {
			camera_x -= 10; // Camera move right
			farmer_x -= 10; // farmer walk right
			target_x -= 10;
			swing_leg();
		}
	
	if (key == 'a' || key == 'A')
		if (camera_x < 2000) {
			camera_x += 10; // Camera move left
			farmer_x += 10; // farmer walk left
			target_x += 10;
			swing_leg();
		}
	if (key == 'c' || key == 'C')
		if (camera_y > 35) {
			camera_y -= 10; //Camera down
		}
	if (key == ' ')
		if (camera_y < 500) {
			camera_y += 10; //Camera up
		}
	if (key == 'l' || key == 'L')
		if (light > 0.5) { 
			light = 0; // turn off the light
		}
		else {
			light = 0.9;  // turn on the light
		}
	glutPostRedisplay();
}

void makeTexture(const char* filename) {
	// read .bmp file
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FILE* pfile = NULL;
	fopen_s(&pfile, filename, "rb");
	if (pfile == 0)
		exit(0);
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);
	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0)
		pixellength++;
	pixellength *= imageheight;
	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0)
		exit(0);
	fseek(pfile, 54, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	fclose(pfile);

	// make textures
	if (filename == "1.bmp") {
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "2.bmp") {
		glGenTextures(1, &texture[1]);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "3.bmp") {
		glGenTextures(1, &texture[2]);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "4.bmp") {
		glGenTextures(1, &texture[3]);
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "5.bmp") {
		glGenTextures(1, &texture[4]);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "6.bmp") {
		glGenTextures(1, &texture[5]);
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "7.bmp") {
		glGenTextures(1, &texture[6]);
		glBindTexture(GL_TEXTURE_2D, texture[6]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "8.bmp") {
		glGenTextures(1, &texture[7]);
		glBindTexture(GL_TEXTURE_2D, texture[7]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (filename == "9.bmp") {
		glGenTextures(1, &texture[8]);
		glBindTexture(GL_TEXTURE_2D, texture[8]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);
		free(pixeldata);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void myDisplay(void) {

	// set the light
	GLfloat lightpos[] = { 0.f, 2000.f, 0.f, 1.0 };
	GLfloat light0_mat[] = { light, light, light, 1};
	GLfloat light0_diff[] = { light, light, light, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_mat);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diff);

	// set the material
	GLfloat mat_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[] = { 0.9,0.9,0.9,1.0 };
	GLfloat mat_specular[] = { 0.3,0.3,0.3,1.0 };
	GLfloat high_shininess[] = { 50.0 };
	GLfloat high_mat[] = { 0.1,0.1,0.1,0.5 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, high_mat);
	

	//////////////// ground ///////////////
	glPushMatrix();
	glTranslatef(0.0, -6.0, 0.0);
	glScalef(4000.0, 10.0, 4000.0);
	glPushMatrix();
	glColor3f(0, 1, 0.5);
	glutSolidCube(1);
	glColor3f(0.0, 0.0, 0.0);
	glutWireCube(1);
	glPopMatrix();
	glPopMatrix();

	//////////////// farmer ////////////////
	glPushMatrix();
	glTranslatef(farmer_x, farmer_y, farmer_z);

	// right leg
	glPushMatrix();
	glTranslatef(-7.5, 5.0, 0.0);
	glRotatef(leg, -1.0, 0.0, 0.0);
	glTranslatef(0.0, -15.0, 0.0);
	glScalef(5.0, 30.0, 5.0);
	glutSolidCube(1.0);
	glPopMatrix();
	// left leg
	glPushMatrix();
	glTranslatef(7.5, 5.0, 0.0);
	glRotatef(-leg, -1.0, 0.0, 0.0);
	glTranslatef(0.0, -15.0, 0.0);
	glScalef(5.0, 30.0, 5.0);
	glutSolidCube(1.0);
	glPopMatrix();
	// right arm
	glPushMatrix();
	glTranslatef(-12.5, 35.0, 0.0);
	glRotatef(20, 0.0, 0.0, -1.0);
	glTranslatef(0.0, -15.0, 0.0);
	glScalef(5.0, 30.0, 5.0);
	glutSolidCube(1.0);
	glPopMatrix();
	// left arm
	glPushMatrix();
	glTranslatef(12.5, 35.0, 0.0);
	glRotatef(-20, 0.0, 0.0, -1.0);
	glTranslatef(0.0, -15.0, 0.0);
	glScalef(5.0, 30.0, 5.0);
	glutSolidCube(1.0);
	glPopMatrix();
	// head
	glTranslatef(0.0, 50.0, 0.0);
	glColor3f(0, 0, 0);
	glutSolidSphere(10.0, 60, 60);
	// neck
	glTranslatef(0.0, -10.0, 0.0);
	glScalef(8.0, 10.0, 10.0);
	glutSolidCube(1.0);
	// body
	glColor3f(1, 0, 0);
	glScalef(0.125, 0.1, 0.1);
	glTranslatef(0.0, -18.0, 0.0);
	glScalef(25.0, 35.0, 15.0);
	glutSolidCube(1.0);
	glPopMatrix();
	

	//////////////// trees ////////////////
	
	for (float i = -1900; i < 2000; i += 250) {
		draw_tree(i, 0, 1900);
	}
	for (float i = -1900; i < 2000; i += 250) {
		draw_tree(-1900, 0, i);
	}
	for (float i = -1900; i < 2000; i += 250) {
		draw_tree(1900, 0, i);
	}
	draw_tree(1000, 0, 1000);
	draw_tree(400, 0, 500);
	draw_tree(-800, 0, 400);
	
	//////////////// animal ////////////////
	glPushMatrix();
	glTranslatef(animal_x, 0, 700);
	// body
	glPushMatrix();
	glTranslatef(0, 50.0, 0);
	glScalef(70, 70, 30);
	glColor3f(0.3, 0.1, 0.6);
	glutSolidSphere(1, 6, 6);
	glPopMatrix();
	// tail
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(-80, 30.0, 0.0);
	glRotatef(40, 0.0, 0.0, -1.0);
	glScalef(5.0, 60.0, 5.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();

	//////////////// house ////////////////

	glPushMatrix();
	glTranslatef(-300, 120, 1300);
	// front wall
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(201, -121, -121);
	glTexCoord2f(0.0, 1.0); glVertex3f(201, 121, -121);
	glTexCoord2f(1.0, 1.0); glVertex3f(-201, 121, -121);
	glTexCoord2f(1.0, 0.0); glVertex3f(-201, -121, -121);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glBegin(GL_QUADS);
	// left wall
	glTexCoord2f(0.0, 0.0); glVertex3f(201, -121, 121);
	glTexCoord2f(0.0, 1.0); glVertex3f(201, 121, 121);
	glTexCoord2f(1.0, 1.0); glVertex3f(201, 121, -121);
	glTexCoord2f(1.0, 0.0); glVertex3f(201, -121, -121);
	// right wall
	glTexCoord2f(0.0, 0.0); glVertex3f(-201, -121, 121);
	glTexCoord2f(0.0, 1.0); glVertex3f(-201, 121, 121);
	glTexCoord2f(1.0, 1.0); glVertex3f(-201, 121, -121);
	glTexCoord2f(1.0, 0.0); glVertex3f(-201, -121, -121);
	// back wall
	glTexCoord2f(0.0, 0.0); glVertex3f(201, -121, 121);
	glTexCoord2f(0.0, 1.0); glVertex3f(201, 121, 121);
	glTexCoord2f(1.0, 1.0); glVertex3f(-201, 121, 121);
	glTexCoord2f(1.0, 0.0); glVertex3f(-201, -121, 121);
	glEnd();
	// top wall
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(201, 121, -121);
	glTexCoord2f(0.0, 1.0); glVertex3f(201, 121, 121);
	glTexCoord2f(1.0, 1.0); glVertex3f(-201, 121, 121);
	glTexCoord2f(1.0, 0.0); glVertex3f(-201, 121, -121);
	glEnd();
	glPopMatrix();
	

	//////////////// sky box ////////////////
	glColor3f(1, 1, 1);
	// front
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(2000, 0, 2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(2000, 3500, 2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(-2000, 3500, 2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2000, 0, 2000);
	glEnd();
	// right
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2000, 0, 2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(-2000, 3500, 2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(-2000, 3500, -2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2000, 0, -2000);
	glEnd();
	// back
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2000, 0, -2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(-2000, 3500, -2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(2000, 3500, -2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(2000, 0, -2000);
	glEnd();
	// left
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(2000, 0, -2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(2000, 3500, -2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(2000, 3500, 2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(2000, 0, 2000);
	glEnd();
	// top
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(2000, 3500, -2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(2000, 3500, 2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(-2000, 3500, 2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2000, 3500, -2000);
	glEnd();
	// down
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(2000, 0, -2000);
	glTexCoord2f(0.0, 1.0); glVertex3f(2000, 0, 2000);
	glTexCoord2f(1.0, 1.0); glVertex3f(-2000, 0, 2000);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2000, 0, -2000);
	glEnd();

	
}

void myinit() {

	// make the textures
	makeTexture("1.bmp");
	makeTexture("2.bmp");
	makeTexture("3.bmp");
	makeTexture("4.bmp");
	makeTexture("5.bmp");
	makeTexture("6.bmp");
	makeTexture("7.bmp");
	makeTexture("8.bmp");
	makeTexture("9.bmp");

	// enable the features
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glFrontFace(GL_CCW);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);
}

void displayObject() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1, 0.1, 5000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, camera_z, target_x, target_y, target_z, 0, 1, 0);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0, 0, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myDisplay();
	glFlush();
	glutSwapBuffers();
}

void reshapeWindow(GLint w, GLint h) {
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000,650);
	glutInitWindowPosition(400, 200);
	glutCreateWindow("farm");
	myinit();
	glutDisplayFunc(displayObject);
	glutReshapeFunc(reshapeWindow);
	glutIdleFunc(when_in_mainloop);
	glutTimerFunc(time_interval, OnTimer, 1);
	glutKeyboardFunc(keyboard_input);
	glutMainLoop();
}