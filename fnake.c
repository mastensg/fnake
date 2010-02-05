#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

/*#define WINDOWED*/
#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32

/* World size */
#define HOR_SIZE 24
#define VER_SIZE 24

#define FRAME_LENGTH 100

float last = 0.0;

int direction = 0;

struct link
{
    float x;
    float y;
    struct link* next;
};

struct link* snake;

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

static void drawSnake()
{
    struct link* currentLink;

    for (currentLink = snake; currentLink != NULL; currentLink = currentLink->next)
    {
        glPushMatrix();
        glColor4f(0.2, 0.7, 1.0, 0.8);
        glScalef(1/(float)HOR_SIZE, 1/(float)VER_SIZE, 1.0);
        glTranslatef(currentLink->x, currentLink->y, 0);
        glScalef(0.9, 0.9, 1.0);
        glBegin(GL_QUADS);
            glVertex3f( 0,  0, 0);
            glVertex3f( 1,  0, 0);
            glVertex3f( 1,  1, 0);
            glVertex3f( 0,  1, 0);
        glEnd();
        glPopMatrix();
    }
}

static int placeLink(struct link* newLink, float x, float y)
{
    newLink->x = x;
    newLink->y = y;

    return 0;
}

static void moveSnake(void)
{
    struct link* currentLink;

    for (currentLink = snake; currentLink->next->next != NULL; currentLink = currentLink->next);

    currentLink->next->next = snake;
    snake = currentLink->next;
    currentLink->next = NULL;

    snake->x = snake->next->x;
    snake->y = snake->next->y;

    switch (direction)
    {
        case 0:
            snake->x += 1.0;
            break;

        case 1:
            snake->x -= 1.0;
            break;

        case 2:
            snake->y += 1.0;
            break;

        case 3:
            snake->y -= 1.0;
            break;
    }
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawSnake();
    glutSwapBuffers();
}

static void animate(int value)
{
    glutTimerFunc(FRAME_LENGTH, animate, 0);
    moveSnake();
    display();
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
        free(snake);
        exit(0);
    }
}

static void special(int key, int x, int y)
{
    int newDirection = direction;

    if (key == GLUT_KEY_RIGHT && direction > 1)
        newDirection = 0;

    if (key == GLUT_KEY_LEFT  && direction > 1)
        newDirection = 1;

    if (key == GLUT_KEY_UP    && direction <= 1)
        newDirection = 2;

    if (key == GLUT_KEY_DOWN  && direction <= 1)
        newDirection = 3;

    if (newDirection != direction)
    {
        direction = newDirection;
        moveSnake();
        display();
    }
}

int main(int argc, char** argv)
{
    struct link link2;
    struct link link3;
    struct link link4;
    struct link link5;

    snake = malloc(sizeof(struct link));

    if (init(argc, argv, WIDTH, HEIGHT, DEPTH))
    {
        fprintf(stderr, "Couldn't initialize.\n");
        return 1;
    }

    placeLink(snake, 3.0, 2.0);
    snake->next = &link2;
    placeLink(&link2, 4.0, 2.0);
    link2.next = &link3;
    placeLink(&link3, 5.0, 2.0);
    link3.next = &link4;
    placeLink(&link4, 6.0, 2.0);
    link4.next = &link5;
    placeLink(&link5, 7.0, 2.0);
    link5.next = NULL;

    glutDisplayFunc(display);
    /*glutIdleFunc(idle);*/
    animate(0);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}
