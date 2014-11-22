#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

#include <stdio.h>
#include "dem.c"

static GLfloat spin = 0.0f;
static float *_data;
static float *_colors;
static _elevationDataSize;
static int height = 400;
static int width = 800;

void init(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);
	char directory[] = "/Users/Robby/Code/DEM/w100n90/";
    char filename[] = "W100N90";
    
    elevationPointCloud(directory, filename, 41.3110871, -72.8074902, width, height, &_data, &_colors);
    _elevationDataSize = width * height;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	// glPushMatrix();
	// glRotatef(spin, 0.0f, 0.0f, 1.0f);
	// 	glColor3f(1.0, 1.0, 1.0);
	// 	glRectf(-25.0, -25.0, 25.0, 25.0);
	// glPopMatrix();
		
	glPushMatrix();

		glTranslatef(0, 0, -50);
		glScalef(.1, .1, -.1);
		glRotatef(120+cos(spin*.0015)*30, -1, 0, 0);
		glPushMatrix();
		glRotatef(sin(spin*.004)*90, 0.0f, 0.0f, 1.0f);
		glScalef(1.0f, 1.0f, .10f);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, _colors);
		glVertexPointer(3, GL_FLOAT, 0, _data);
		glDrawArrays(GL_POINTS, 0, _elevationDataSize);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	// glFlush();
}

void spinDisplay(void){
	spin = spin - 2.0;
	if(spin > 360.0){
		spin = spin - 360.0;
	}
	glutPostRedisplay();
}

void reshape(int w, int h){
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho(-40.0, 40.0, -40.0, 40.0, -100.0, 100.0);
	glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int button, int state, int x, int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN)
				glutIdleFunc(spinDisplay);
			break;
		case GLUT_MIDDLE_BUTTON:
			if(state == GLUT_DOWN)
				glutIdleFunc(NULL);
			break;
		default:
			break;
	}
}

int main(int argc, char **argv){
	printf("WE ON!\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(10,10);
	glutInitWindowSize(width,height);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}