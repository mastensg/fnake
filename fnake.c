#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <magick/ImageMagick.h>

#define WINDOWED
#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32

#define FRAME_LENGTH 10

#define STARTING_LENGTH 3

/* How much the snake grows when eating a cookie. */
#define COOKIE_GOODNESS 1

/* Number of milliseconds to wait between interactions. */
#define WAIT 100

/* Number of clouds. */
#define CLOUDS 16

int direction;
int last;
int wait;

struct point
{
    int x;
    int y;
    struct point* next;
};

struct cloud
{
    int w;
    int h;
    float vx;
    float vy;
    float period;
    struct cloud* next;
};

struct point* snake;
struct point* cookie;
struct cloud* cloud;

struct image {
    int width, height, stride;
    unsigned char *data;
};

struct image* world;
#define getworld(x,y) (world->data + (y) * world->stride + (x) * 4)

struct image *loadWorld(const char *filename)
{
    struct image* img;
    const PixelPacket* pixels;
    int i;

    Image* magickimg;
    ImageInfo* imageinfo;
    ExceptionInfo exception;

    InitializeMagick(0);
    GetExceptionInfo(&exception);
    imageinfo = CloneImageInfo(0);

    strcpy(imageinfo->filename, filename);
    magickimg = ReadImage(imageinfo, &exception);

    if (!magickimg) {
        DestroyMagick();
        return 0;
    }

    img = malloc(sizeof(struct image));
    img->width = magickimg->columns;
    img->height = magickimg->rows;
    img->stride = img->width * 4;
    img->data = malloc(img->stride * img->height);

    pixels = AcquireImagePixels(magickimg, 0, 0,
                                img->width, img->height, &exception);

    if (!pixels) {
        free(img);
        img = 0;
        goto out;
    }

    /* Imagemagick may use > 8 bits per channel */
    for (i=0; i < img->width * img->height; ++i) {
        img->data[i*4 + 0] = pixels[i].red >> (QuantumDepth - 8);
        img->data[i*4 + 1] = pixels[i].green >> (QuantumDepth - 8);
        img->data[i*4 + 2] = pixels[i].blue >> (QuantumDepth - 8);
        img->data[i*4 + 3] = pixels[i].opacity >> (QuantumDepth - 8);
    }

out:
    DestroyImageInfo(imageinfo);
    DestroyImage(magickimg);
    DestroyMagick();

    return img;
}

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

static struct cloud* addCloud(struct cloud* head)
{
    struct cloud* currentCloud;
    struct cloud* newCloud;

    /* Go through the list. Stop at the tail. */
    for (currentCloud = head; currentCloud->next != NULL; currentCloud = currentCloud->next);

    /* Make a new tail. */
    newCloud = malloc(sizeof(struct cloud));
    *newCloud = *currentCloud;
    currentCloud->next = newCloud;

    newCloud->w = 1 + rand() % 12;
    newCloud->h = 1 + rand() % 12;
    newCloud->vx = -0.05 + 100.0/(1 + rand() % 1000);
    newCloud->vy = -0.05 + 100.0/(1 + rand() % 1000);
    newCloud->period = rand();

    return newCloud;
}

static void initWorld(const char* filename)
{
    int i;
    int x;
    int y;
    struct point* currentPoint;

    /* Read the world. */
    world = loadWorld(filename);

    /* Spawn a brand new snake. It's only a head for now. */
    snake->x = world->width / 2;
    snake->y = world->height / 2;
    snake->next = NULL;

    /* Let the snake grow for a while. */
    for (i = 1; i < STARTING_LENGTH; ++i)
        addPoint(snake);

    /* Drop some cookies in there. */
    cookie->x = -1;
    cookie->y = -1;
    cookie->next = NULL;

    for (y = 0; y < world->height; ++y)
    {
        for (x = 0; x < world->width; ++x)
            if (getworld(x,y)[0] == 255 &&
                getworld(x,y)[1] == 0 &&
                getworld(x,y)[2] == 0)
            {
                currentPoint = addPoint(cookie);
                currentPoint->x = x;
                currentPoint->y = y;
            }
    }

    srand(world->width*world->height);

    /* Spawn a brand new cloud. */
    cloud->w = 1 + rand() % 12;
    cloud->h = 1 + rand() % 12;
    cloud->vx = -0.05 + 100.0/(1 + rand() % 1000);
    cloud->vy = -0.05 + 100.0/(1 + rand() % 1000);
    cloud->period = rand();
    cloud->next = NULL;

    /* Spawn some more clouds. */
    for (i = 1; i < CLOUDS; ++i)
        addCloud(cloud);

    direction = 0;
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

    if (snake->x >= world->width)
        snake->x = 0;
    else if (snake->x < 0)
        snake->x = world->width - 1;
    else if (snake->y >= world->height)
        snake->y = 0;
    else if (snake->y < 0)
        snake->y = world->height - 1;
}

static int occupied(int x, int y)
{
    struct point* currentPoint;

    /* Check if point is in the world. */
    if (getworld(x,y)[0] == 255)
        return 3;

    /* Check if point is in the body of the snake. */
    for (currentPoint = snake->next; currentPoint != NULL; currentPoint = currentPoint->next)
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
        cookie->x = rand() % world->width;
        cookie->y = rand() % world->height;
    }
}

static void interact(void)
{
    int i;
    struct point* currentCookie;

    moveSnake();

    if (occupied(snake->x, snake->y) > 1)
    {
        /* Spawn a brand new snake. It's only a head for now. */
        snake->x = world->width / 2;
        snake->y = world->height / 2;
        snake->next = NULL;

        /* Let the snake grow for a while. */
        for (i = 1; i < STARTING_LENGTH; ++i)
            addPoint(snake);
    }

    for (currentCookie = cookie; currentCookie != NULL; currentCookie = currentCookie->next)
    {
        if (currentCookie->x == snake->x && currentCookie->y == snake->y)
        {
            /* Convert cookie to snake. */
            for (i = 0; i < COOKIE_GOODNESS; ++i)
                addPoint(snake);

            moveCookie(currentCookie);
        }
    }
}

static void display(void)
{
    int x;
    int y;
    float z;
    float t = glutGet(GLUT_ELAPSED_TIME);
    struct point* currentPoint;
    struct cloud* currentCloud;
    float diffuse[] = {0, 0, 0, 1};
    float dx;
    float dy;

    switch (direction)
    {
        case 0:
            dx = (float)wait/WAIT;
            dy = 0.0;
            break;

        case 1:
            dx = -(float)wait/WAIT;
            dy = 0.0;
            break;

        case 2:
            dx = 0.0;
            dy = -(float)wait/WAIT;
            break;

        case 3:
            dx = 0.0;
            dy = (float)wait/WAIT;
            break;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glScalef(1/(float)world->width, 1.0/world->width, 1/(float)world->height);
    gluLookAt(snake->x + dx,
              48.0,
              snake->y + dy + 64,
              snake->x + dx,
              0.0,
              snake->y + dy,
              0.0, 1.0, 0.0);

#if 0
    gluLookAt((float)snake->x / world->width,
              1.50,
              (float)snake->y / world->height + 1.25,
              (float)snake->x / world->width,
              1.0,
              (float)snake->y / world->height,
              0.0, 1.0, 0.0);
    /* Draw clouds. */
    diffuse[0] = 1;
    diffuse[1] = 1;
    diffuse[2] = 1;
    diffuse[3] = 0.7;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    glTranslatef(0, -0.25, 0);
    for (currentCloud = cloud; currentCloud != NULL; currentCloud = currentCloud->next)
    {
        glPushMatrix();
        glTranslatef(2 * cos(currentCloud->vx * t/10000 + currentCloud->period/1000.0),
                     0,
                     2 * sin(currentCloud->vy * t/10000 + currentCloud->period/1000.0));
        glScalef(    currentCloud->w/10.0, 1/10.0, currentCloud->h/10.0);
        glutSolidSphere(1.0, 64, 64);
        glPopMatrix();
    }
#endif

    /* Draw snake. */
    diffuse[0] = 0.0;
    diffuse[1] = 0.25 /*+ 0.0625 * sin(z*64)*/;
    diffuse[2] = 0.0;
    diffuse[3] = 1.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    for (currentPoint = snake, z = 0; currentPoint != NULL; currentPoint = currentPoint->next, ++z)
    {
        glTranslatef( currentPoint->x, 0,  currentPoint->y);
        glutSolidCube(1.0);
        glTranslatef(-currentPoint->x, 0, -currentPoint->y);
    }

    /* Draw cookies. */
    diffuse[0] = 1;
    diffuse[1] = 0;
    diffuse[2] = 0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    for (currentPoint = cookie->next; currentPoint != NULL; currentPoint = currentPoint->next)
    {
        glTranslatef( currentPoint->x, 0,  currentPoint->y);
        glTranslatef(0, 0.25, 0);
        glRotatef( t / 30, 0, 1, 0);
        glutSolidTeapot(0.75);
        glRotatef(-t / 30, 0, 1, 0);
        glTranslatef(0, -0.25, 0);
        glTranslatef(-currentPoint->x, 0, -currentPoint->y);
    }

    glTranslatef(0, -1, 0);
    /* Draw world. */
    for (y = 0; y < world->height; ++y)
    {
        for (x = 0; x < world->width; ++x)
        {
            if (getworld(x,y)[0] == 255)
            {
                z = 5 + 0.25 * sin((float)x*32/world->width + t/1000) + 0.25 * cos((float)y*32/world->height + t/1000);
                diffuse[3] = 0.5;
            }
            else
            {
                z = 1;
                diffuse[3] = 0.75;
            }

            diffuse[0] = 0.5 + 0.3 * sin(sin(x * 6.0/world->width + 340 + y*16.0/world->height) + sin(y * 16.0/world->height + 220 + x*12.0/world->width) * 3.0 + (float)x*64/world->width + 600);
            diffuse[1] = 0.5 + 0.3 * sin(sin(x * 6.0/world->width + 340 + y*16.0/world->height) + sin(y * 16.0/world->height + 220 + x*12.0/world->width) * 3.0 + (float)x*64/world->width + 600);
            diffuse[2] = 0.5 + 0.3 * sin(sin(x * 6.0/world->width + 340 + y*16.0/world->height) + sin(y * 16.0/world->height + 220 + x*12.0/world->width) * 3.0 + (float)x*64/world->width + 600);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);

            glTranslatef(x, -0.5 + z/2, y);
            glScalef(1, z, 1);
            glutSolidCube(1.0);
            glScalef(1, 1/z, 1);
            glTranslatef(-x, 0.5 - z/2, -y);
        }
    }

    glPopMatrix();
    glutSwapBuffers();
}

static void idle(void)
{
    float t = glutGet(GLUT_ELAPSED_TIME);
    float dt = t - last;

    last = t;
    wait += dt;

    if (wait > WAIT)
    {
        interact();
        wait = 0;
    }

    display();
}

static void reshape(int w, int h)
{
    float ratio;

    if (h == 0) {
        h = 1;
    }

    ratio = 1.0 * w/h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(30, ratio, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y)
{
    /* Grow cheat. */
    if (key == 111)
        addPoint(snake);

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
        wait = 0;
        interact();
    }
}

static int init(int argc, char** argv, int w, int h, int depth)
{
    float ambient[]         = {0.2, 0.2, 0.2, 1.0};
    float diffuse[]         = {0.8, 0.8, 0.8, 1.0};
    float light_position[]  = {-1.5, 3.0, -4.0, 1.0};
    float specular[]        = {0.9, 0.9, 0.9, 1.0};

#ifndef WINDOWED
    char modeString[16];
#endif

    glutInit(&argc, argv);

#ifdef WINDOWED
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM);
    glutCreateWindow(argv[0]);
#else
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM);

    if (snprintf(modeString, sizeof(modeString), "%dx%d:%d@85", w, h, depth) < 0)
        return -1;

    glutGameModeString(modeString);
    glutEnterGameMode();
    glutSetCursor(GLUT_CURSOR_NONE);
#endif

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    /*glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);*/
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3, 0.3, 1, 0);
    glClearAccum(0, 0, 0, 0);

    return 0;
}

int main(int argc, char** argv)
{
    if (init(argc, argv, WIDTH, HEIGHT, DEPTH))
        return 1;

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);

    snake  = malloc(sizeof(struct point));
    cookie = malloc(sizeof(struct point));
    cloud  = malloc(sizeof(struct cloud));

    last  = 0;
    initWorld("world.png");

    glutMainLoop();

    return 0;
}
