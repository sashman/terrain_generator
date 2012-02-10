/*
 * 3d_view.cpp
 *
 * Module use to view the generated terrain in 3D using GLUT
 *
 *  Created on: 7 Feb 2012
 *      Author: sash
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "terrain_generator.hpp"

/* region size */
#define X_MAX 1.0
#define Y_MAX 0.3
#define Z_MAX 1.0
#define X_MIN -1.0
#define Y_MIN -.3
#define Z_MIN -1.0

float eyex = 1.5;
float eyey = 1.0;
float eyez = 1.5;

//eye pre defined location
float predef_eyex[] = { 1.0, 0.5, 1.0 };
float predef_eyey[] = { 1.0, 0.0, -1.0 };
float predef_eyez[] = { 0.5, 0.9, 0.5 };

//center
float centrex =  0.0;
float centrey =  0.0;
float centrez =  0.0;

float predef_centrex[] = { -0.8, -1, 0, -1.0 };
float predef_centrey[] = { -0.7, -0.5, -0.2 };
float predef_centrez[] = { -0.5, -0.7, -0.6 };
//up
float upx = 0.0;
float upy = 1.0;
float upz = 0.0;


extern int crop_height;
extern int crop_width;
int max_value = 0;
extern int** tmap;

GLuint dlist;
bool changed = true;

float map_x(int x) {
	return ((float)x/(float)crop_width)*(X_MAX-X_MIN)+X_MIN;
}

float map_y(int y) {
	return ((float)y/(float)max_value)*(Y_MAX-Y_MIN)+Y_MIN;
}

float map_z(int z) {
	return ((float)z/(float)crop_height)*(Z_MAX-Z_MIN)+Z_MIN;
}

void display() {
	if (changed) {
		changed = false;
		dlist = glGenLists(1);

		glNewList(dlist, GL_COMPILE);


		glClear(GL_COLOR_BUFFER_BIT);


		for (int i = 0; i < crop_height; i++) {
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < crop_width; j++) {

				float x = map_x(j);
				float z = map_z(i);
				float height = map_y(tmap[i][j]);


				glColor3f(height + 0.2,height + 0.2,height + 0.2);
				glVertex3d(x, height + 0.2, z);

			}
			glEnd();
		}





	}
	glEndList();

	glCallList(dlist);
	glutSwapBuffers();

}

void reshape(int w, int h) {

}

void idle() {

	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez, centrex, centrey, centrez, upx, upy, upz);

	glutPostRedisplay();

}

void run_view() {
	int argc = 0;
	char** argv;

	glutInit(&argc, argv);


	std::cout<<"\n\n3D...."<<std::endl;
	max_value = tmap[0][0];
	for (int i = 0; i < crop_height; i++) {
		for (int j = 0; j < crop_width; j++) {
			if (tmap[i][j] > max_value)
				max_value = tmap[i][j];
		}
	}
	std::cout<<"Max value "<<max_value<<std::endl;
	dlist = glGenLists(1);

	glutInitWindowSize(500, 500);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutCreateWindow("3D View");
	//glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);

	/*
	 glutKeyboardFunc(keyListen);
	 glutSpecialFunc(keySpecialListen);
	 */

	glutMainLoop();

}

