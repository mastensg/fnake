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

int direction = 0;

struct link
{
    float x;
    float y;
    struct link* next;
};

struct link snake;

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

static int placeLink(struct link* newLink, float x, float y)
{
    newLink->x = x;
    newLink->y = y;

    return 0;
}

static void drawLink(void)
{
    glScalef(0.9, 0.9, 1.0);
    glBegin(GL_QUADS);
        glVertex3f( 0,  0, 0);
        glVertex3f( 1,  0, 0);
        glVertex3f( 1,  1, 0);
        glVertex3f( 0,  1, 0);
    glEnd();
}

static void drawSnake(struct link* invisibleSnake)
{
    struct link* currentLink;

    for (currentLink = invisibleSnake; currentLink != NULL; currentLink = currentLink->next)
    {
        glPushMatrix();
        glColor4f(0.2, 0.7, 1.0, 0.8);
        glScalef(0.025, 0.025, 1.0);
        glTranslatef(currentLink->x, currentLink->y, 0);
        drawLink();
        glPopMatrix();
    }
}

static void moveSnake(struct link* invisibleSnake, int invisibleDirection)
{
    struct link* currentLink;

    switch invisibleSnake
    {
        case 0:
            invisibleSnake->x += 1.0;
            break;

        case 1:
            invisibleSnake->x += 1.0;
            break;

        case 2:
            invisibleSnake->x += 1.0;
            break;

        case 3:
            invisibleSnake->x += 1.0;
            break;
    }

    for (currentLink = invisibleSnake; currentLink->next != NULL; currentLink = currentLink->next)
    {
        placeLink(invisibleSnake, invisibleSnake->next->x, invisibleSnake->next->y);
    }
}

static void display(void)
{
    float t  = glutGet(GLUT_ELAPSED_TIME);
    float dt = (t - last)/100;

    last = t;

    glClear(GL_COLOR_BUFFER_BIT);

    drawSnake(&snake);

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
        direction = 2;

    if (key == GLUT_KEY_UP)
        direction = 1;

    if (key == GLUT_KEY_RIGHT)
        direction = 0;

    if (key == GLUT_KEY_DOWN)
        direction = 3;
}

int main(int argc, char** argv)
{
    struct link link2;
    struct link link3;

    if (init(argc, argv, WIDTH, HEIGHT, DEPTH))
    {
        fprintf(stderr, "Couldn't initialize.\n");
        return 1;
    }

    placeLink(&snake, 3.0, 2.0);
    snake.next = &link2;
    placeLink(&link2, 4.0, 2.0);
    link2.next = &link3;
    placeLink(&link3, 5.0, 2.0);
    link3.next = NULL;

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}
