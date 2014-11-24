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
static float *_points;
static uint32_t *_indices;
static float *_colors;
static unsigned int _numPoints;
static unsigned int _numIndices;
static int height = 30;
static int width = 50;
static float *political;

void init(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);
	char directory[] = "/Users/Robby/Code/DEM/w100n90/";
    char filename[] = "W100N90";


    char d2[] = "/Users/Robby/Code/3DEarth/";
    char f2[] = "oregon.data";
    loadData(d2, f2, &political);


    // elevationPointCloud(directory, filename, 41.3110871, -72.8074902, width, height, &_points, &_colors, &_numPoints);
	elevationTriangles(directory, filename, 41.3110871, -72.8074902, width, height, &_points, &_indices, &_colors, &_numPoints, &_numIndices);

    // elevationPointCloud(directory, filename, 44.0, -120.5, width, height, &_points, &_colors, &_numPoints);
	// elevationTriangles(directory, filename, 44.0, -120.5, width, height, &_points, &_indices, &_colors, &_numPoints, &_numIndices);

	// for(int i = 0; i < _numIndices/3; i++){
	// 	printf("v %.2f %.2f %.2f\n", _points[_indices[i*3+0]], _points[_indices[i*3+1]], _points[_indices[i*3+2]] );
	// }
	for(int i = 0; i < _numPoints; i++){
		printf("v %.2f %.2f %.2f\n", _points[i*3+0], _points[i*3+1], _points[i*3+2] );
	}
	for(int i = 0; i < _numIndices/3; i++){
		printf("f %d %d %d\n", _indices[i*3+0] + 1, _indices[i*3+1] + 1, _indices[i*3+2] + 1);
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);

	// glPushMatrix();
	// glRotatef(spin, 0.0f, 0.0f, 1.0f);
	// 	glColor3f(1.0, 1.0, 1.0);
	// 	glRectf(-25.0, -25.0, 25.0, 25.0);
	// glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0, 0, -5);
		glScalef(.1, .1, -.1);
		// glRotatef(180, -1, 0, 0);  // ORTHO 1
		glRotatef(120+cos(spin*.0015)*30, -1, 0, 0);  // PERSPECTIVE 1
		glPushMatrix();
		glRotatef(sin(spin*.004)*90, 0.0f, 0.0f, 1.0f);  // PERSPECTIVE 2
		glScalef(1.0f, 1.0f, .10f);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColor3f(0.5f, 1.0f, 0.5f);
		glColorPointer(3, GL_FLOAT, 0, _colors);
		glVertexPointer(3, GL_FLOAT, 0, _points);
		// glDrawArrays(GL_POINTS, 0, _numPoints);
		glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, _indices);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);


		// glPushMatrix();
		// glTranslatef(0.0f, 0.0f, 0.0f);
		// glLineWidth(2);
		// glColor3f(1.0f, 1.0f, 1.0f);
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glVertexPointer(2, GL_FLOAT, 0, political);
		// glDrawArrays(GL_LINE_LOOP, 0, 128);
		// glDisableClientState(GL_VERTEX_ARRAY);
		// glPopMatrix();

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
	float a = (float)width/height;
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho(-50.0, 50.0, -50/a, 50/a, -100.0, 100.0);
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