#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOWED
#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32

float last = 0.0;

float x  = 0.5;
float y  = 0.5;
float vx = 1.0;
float vy = 0.0;

/* Set up a window, or go into fullscreen. */
static int init(int argc, char** argv, int w, int h, int depth)
{
#ifndef WINDOWED
    char modeString[16];
#endif

    glutInit(&argc, argv);

#ifdef WINDOWED
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow(argv[0]);
#else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    if (snprintf(modeString, sizeof(modeString), "%dx%d:%d@85", w, h, depth) < 0)
        return -1;

    glutGameModeString(modeString);
    glutEnterGameMode();
    glutSetCursor(GLUT_CURSOR_NONE);
#endif

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel (GL_FLAT);
    glClearColor(0, 0, 0, 0);

    return 0;
}

static void drawLink(void)
{
    glBegin(GL_QUADS);
        glVertex3f( 0,  0, 0);
        glVertex3f( 1,  0, 0);
        glVertex3f( 1,  1, 0);
        glVertex3f( 0,  1, 0);
    glEnd();
}

static void drawSnake(float x, float y)
{
    glPushMatrix();
    glColor4f(0.2, 0.7, 1.0, 0.8);
    glScalef(0.025, 0.025, 1.0);

    glTranslatef(x, y, 0);
    drawLink();

    glTranslatef(1, 0, 0);
    drawLink();

    glTranslatef(0, 1, 0);
    drawLink();

    glTranslatef(0, 1, 0);
    drawLink();

    glTranslatef(-1, 0, 0);
    drawLink();

    glTranslatef(0, 1, 0);
    drawLink();

    glPopMatrix();
}

static void display(void)
{
    float t  = glutGet(GLUT_ELAPSED_TIME);
    float dt = (t - last)/100;

    last = t;

    x += vx * dt;
    y += vy * dt;

    glClear(GL_COLOR_BUFFER_BIT);

    drawSnake(x, y);

    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      gluOrtho2D (0.0, 1.0, 0.0, 1.0*(GLfloat)h/(GLfloat)w);
   else 
      gluOrtho2D (0.0, 1.0*(GLfloat)w/(GLfloat)h, 0.0, 1.0);

}

static void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
#ifndef WINDOWED
        glutLeaveGameMode();
#endif
        exit(0);
    }
}

static void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
    {
        vx = -1.0;
        vy =  0.0;
    }

    if (key == GLUT_KEY_UP)
    {
        vx =  0.0;
        vy =  1.0;
    }

    if (key == GLUT_KEY_RIGHT)
    {
        vx =  1.0;
        vy =  0.0;
    }

    if (key == GLUT_KEY_DOWN)
    {
        vx =  0.0;
        vy = -1.0;
    }
}

int main(int argc, char** argv)
{
    if (init(argc, argv, WIDTH, HEIGHT, DEPTH))
    {
        fprintf(stderr, "Couldn't initialize.\n");
        return 1;
    }

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}
