#include <iostream>
#include <GL/freeglut.h>

using namespace std;

// main window pointer and initial size
GLuint window;
GLuint width = 800, height = 500;

double xmin = 0, xmax = width, ymin = 0, ymax = height;
double umin = -2.5, umax = 1, vmin = -2, vmax = 2;

double eps = 2;
int limit = 16;

typedef struct
{
    double re;
    double im;
} complex;

void display();

void reshape(int width, int height);

void keyboard(unsigned char key, int mouseX, int mouseY);

void renderFractal();

int divergence_test(complex c);

/**
 * This program draws Mandelbrot's fractal with given parameters.
 *
 * @author ftodoric
 * @date 16/05/2020
 */
int main(int argc, char **argv)
{
    printf("Set epsilon:\n");
    cin >> eps;
    printf("Set iteration limit:\n");
    cin >> limit;
    printf("Set umin, umax, vmin, vmax:\n");
    cin >> umin >> umax >> vmin >> vmax;

    // initializing window and settings
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);

    window = glutCreateWindow("Mandelbrot Fractal");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
}

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    renderFractal();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    width = w;
    height = h; // new window width and height

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // opening space in window
    glMatrixMode(GL_PROJECTION);                       // projection matrix
    glLoadIdentity();                                  // identity matrix
    gluOrtho2D(0, width - 1, 0, height - 1);
    glMatrixMode(GL_MODELVIEW); // view matrix
    glLoadIdentity();           // identity matrix

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT);         // clearing background
    glPointSize(1.0);                     // set point of the line size
    glColor3f(0.0f, 0.0f, 0.0f);          // line color
}

void keyboard(unsigned char key, int mouseX, int mouseY)
{
    //glutPostRedisplay();
}

void renderFractal()
{
    glPointSize(1);
    glBegin(GL_POINTS);
    for (int y = (int)ymin; y <= ymax; y++)
    {
        for (int x = (int)xmin; x <= xmax; x++)
        {
            complex c;
            c.re = (x - xmin) / (double)(xmax - xmin) * (umax - umin) + umin;
            c.im = (y - ymin) / (double)(ymax - ymin) * (vmax - vmin) + vmin;
            int n = divergence_test(c);
            if (n == -1)
                glColor3f(0.0f, 0.0f, 0.0f);
            else
                glColor3f((double)n / limit, 1.0 - (double)n / limit, 255);

            glVertex2i(x, y);
        }
    }
    glEnd();
}

int divergence_test(complex c)
{
    complex z;
    z.re = 0, z.im = 0;
    for (int i = 0; i < limit; i++)
    {
        double next_re = z.re * z.re - z.im * z.im + c.re;
        double next_im = 2 * z.re * z.im + c.im;
        z.re = next_re;
        z.im = next_im;
        double modul2 = z.re * z.re + z.im * z.im;
        if (modul2 > eps * eps)
            return i;
    }
    return -1;
}
