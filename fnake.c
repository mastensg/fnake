#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOWED
#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32

#define FRAME_LENGTH 100

/* World size */
#define HOR_SIZE 24
#define VER_SIZE 24

#define STARTING_LENGTH 3
#define COOKIE_GOODNESS 3
#define GOAL 5

int direction = 0;
int cookies = 0;
int level = 0;

struct point
{
    int x;
    int y;
    struct point* next;
};

struct point* snake;
struct point* cookie;

int levels[2][24][24] = {
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    }
};

/* Add another point to a list. */
static struct point* addPoint(struct point* head)
{
    struct point* currentPoint;
    struct point* newPoint;

    /* Go through the list. Stop at the tail. */
    for (currentPoint = head; currentPoint->next != NULL; currentPoint = currentPoint->next);

    /* Make a new tail. */
    newPoint = malloc(sizeof(struct point));
    *newPoint = *currentPoint;
    currentPoint->next = newPoint;

    return newPoint;
}

static void moveSnake(void)
{
    struct point* currentLink;

    /* Go through the snake. Stop one point before the tail. */
    for (currentLink = snake; currentLink->next->next != NULL; currentLink = currentLink->next);

    /* Move the tail on top of the head. */
    currentLink->next->next = snake;
    snake = currentLink->next;
    currentLink->next = NULL;
    snake->x = snake->next->x;
    snake->y = snake->next->y;

    /* Move the head in the direction the snake is going. */
    switch (direction)
    {
        case 0:
            ++snake->x;
            break;

        case 1:
            --snake->x;
            break;

        case 2:
            --snake->y;
            break;

        case 3:
            ++snake->y;
            break;
    }

    /* Wrap around if the snake is at the edge. */
    if (snake->x >= HOR_SIZE)
        snake->x = 0;
    else if (snake->x < 0)
        snake->x = HOR_SIZE - 1;
    else if (snake->y >= VER_SIZE)
        snake->y = 0;
    else if (snake->y < 0)
        snake->y = VER_SIZE - 1;
}

static int occupied(int x, int y)
{
    struct point* currentPoint;

    /* Check if point is in the level. */
    if (levels[level][snake->y][snake->x] == 1)
        return 3;

    /* Check if point is in the body of the snake. */
    for (currentPoint = snake->next; currentPoint->next != NULL; currentPoint = currentPoint->next)
    {
        if (x == currentPoint->x && y == currentPoint->y)
        {
            return 2;
        }
    }

    /* Check if point is the head of the snake. */
    if (snake->x == x && snake->y == y)
        return 1;

    return 0;
}

/* Move the cookie out of the way. */
static void moveCookie(struct point* cookie)
{
    while (occupied(cookie->x, cookie->y))
    {
        cookie->x = rand() % HOR_SIZE;
        cookie->y = rand() % VER_SIZE;
    }
}

static void initLevel(int level)
{
    int i;
    int x;
    int y;
    struct point* currentCookie;

    /* Spawn a brand new snake. It's only a head for now. */
    snake->x = HOR_SIZE / 2;
    snake->y = VER_SIZE / 2;
    snake->next = NULL;

    /* Let the snake grow for a while. */
    for (i = 1; i < STARTING_LENGTH; ++i)
        addPoint(snake);

    /* Drop some cookies in there. */
    cookie->x = -1;
    cookie->y = -1;
    cookie->next = NULL;

    for (y = 0; y < VER_SIZE; ++y)
    {
        for (x = 0; x < HOR_SIZE; ++x)
            if (levels[level][y][x] == 2)
            {
                currentCookie = addPoint(cookie);
                currentCookie->x = x;
                currentCookie->y = y;
            }
    }

    cookies = 0;
    srand(level);
}

static void die(void)
{
    initLevel(level);
}

static void interact(void)
{
    int i;
    struct point* currentCookie;

    moveSnake();

    if (occupied(snake->x, snake->y) > 1)
        die();

    for (currentCookie = cookie; currentCookie != NULL; currentCookie = currentCookie->next)
    {
        if (currentCookie->x == snake->x && currentCookie->y == snake->y)
        {
            /* Go to the next level when goal is reached. */
            if (++cookies >= GOAL)
                initLevel(++level);

            /* Convert cookie to snake. */
            for (i = 1; i < COOKIE_GOODNESS; ++i)
                addPoint(snake);

            moveCookie(currentCookie);
        }
    }
}

/* Set up a window, or go into fullscreen mode. */
static void drawSquare(int x, int y)
{
    glPushMatrix();
    glScalef(1/(float)HOR_SIZE, 1/(float)VER_SIZE, 1.0);
    glTranslatef(x, y, 0);
    glScalef(0.8, 0.8, 1.0);
    glBegin(GL_QUADS);
        glVertex3f( 0,  0, 0);
        glVertex3f( 1,  0, 0);
        glVertex3f( 1,  1, 0);
        glVertex3f( 0,  1, 0);
    glEnd();
    glPopMatrix();
}

static void drawLevel(level)
{
    int x;
    int y;

    for (y = 0; y < VER_SIZE; ++y)
    {
        for (x = 0; x < HOR_SIZE; ++x)
            if (levels[level][y][x] == 1)
                drawSquare(x, y);
    }
}

static void drawSquares(struct point* head)
{
    struct point* currentPoint;

    for (currentPoint = head; currentPoint != NULL; currentPoint = currentPoint->next)
    {
        drawSquare(currentPoint->x, currentPoint->y);
    }
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(0.1, 0.2, 7.0, 0.8);
    drawLevel(level);

    glColor4f(0.2, 0.7, 1.0, 0.8);
    drawSquares(snake);

    glColor4f(0.7, 0.2, 1.0, 0.8);
    drawSquares(cookie);

    glutSwapBuffers();
}

static void animate(int value)
{
    glutTimerFunc(FRAME_LENGTH, animate, 0);
    interact();
    display();
}

static void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluOrtho2D (0, 1, (GLfloat)h/(GLfloat)w, 0);
    else
        gluOrtho2D (0, (GLfloat)w/(GLfloat)h, 1, 0);
}

static void keyboard(unsigned char key, int x, int y)
{
    /* Level cheats. */
    if      (key == 110)
        initLevel(++level);
    else if (key == 112)
        initLevel(--level);

    /* Grow cheat. */
    if (key == 111)
        addPoint(snake);

    if (key == 27)
    {
#ifndef WINDOWED
        glutLeaveGameMode();
#endif
        free(snake);
        free(cookie);
        exit(0);
    }
}

static void special(int key, int x, int y)
{
    int newDirection = direction;

    if      (key == GLUT_KEY_RIGHT && direction > 1)
        newDirection = 0;
    else if (key == GLUT_KEY_LEFT  && direction > 1)
        newDirection = 1;
    else if (key == GLUT_KEY_UP    && direction <= 1)
        newDirection = 2;
    else if (key == GLUT_KEY_DOWN  && direction <= 1)
        newDirection = 3;

    /* Move immediately if the direction has changed. */
    if (newDirection != direction)
    {
        direction = newDirection;
        interact();
        display();
    }
}

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

int main(int argc, char** argv)
{
    if (init(argc, argv, WIDTH, HEIGHT, DEPTH))
    {
        fprintf(stderr, "Couldn't initialize.\n");
        return 1;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    snake = malloc(sizeof(struct point));
    cookie = malloc(sizeof(struct point));

    initLevel(level);
    animate(0);

    glutMainLoop();

    return 0;
}
