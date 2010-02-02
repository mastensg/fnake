#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOWED
#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32

/* Set up a window, or go into fullscreen. */
static void init(int argc, char** argv, int w, int h, int depth)
{
    glutInit(&argc, argv);

#ifdef WINDOWED
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow(argv[0]);
#else
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutGameModeString(sprintf("%dx%d:%d@85", w, h, depth));
    glutEnterGameMode();
    glutSetCursor(GLUT_CURSOR_NONE);
#endif
}

static void display(void)
{
    float t = glutGet(GLUT_ELAPSED_TIME);

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    float ratio;

    if (h == 0)
        h = 1;

    ratio = 1.0 * w/h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        glutLeaveGameMode();
        exit(0);
    }
}

int main(int argc, char** argv) {
    init(argc, argv, WIDTH, HEIGHT, DEPTH);

    glClearColor(1, 1, 1, 0);

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}
