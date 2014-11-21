#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

static GLfloat spin = 0.0f;

void init(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_FLAT);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		glRotatef(spin, 0.0f, 0.0f, 1.0f);
		glColor3f(1.0, 1.0, 1.0);
		glRectf(-25.0, -25.0, 25.0, 25.0);
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
	glOrtho(-40.0, 40.0, -40.0, 40.0, -1.0, 1.0);
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
	glutInitWindowSize(480,480);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}