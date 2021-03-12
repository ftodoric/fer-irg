#include <cstdio>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

#define MAX_N 20 // maximum number of polygon vertices

using namespace glm;

// polygon structures
GLint vertices_x[MAX_N], vertices_y[MAX_N];
GLint number_of_vertices = 0;
GLboolean is_drawn = FALSE;
vec3 edgeVec[MAX_N]; // edge vectors

GLint T_x, T_y; // custom vertex in relation polygon

int rgb[3] = {0, 0, 0}; // current polygon color

// main window pointer and initial size
GLuint window;
GLuint width = 800, height = 500;

// function prototypes
void myDisplay();

void myReshape(int width, int height);

void convexPolygonMouse(int button, int state, int x, int y);

void convexPolygonKeyboard(unsigned char theKey, int mouseX, int mouseY);

void convexPolygon(GLint v_x[], GLint v_y[]);

void printVector(vec3 v);

void is_vertex_in_polygon(GLint x, GLint y);

void color_the_polygon();

/**
 * This program takes keyboard and mouse inputs then draws and colors the polygon.
 * Given polygon is checked if it's convex.
 * Coordinates of the polygon are taken by left mouse clicks, and by
 * pressing button 'f' coordinates are then processed and polygon is drawn.
 * Buttons 'r','g','b' and 'k' change current color for polygon filling.
 *
 * Button 'c' or right mouse click clears the screen when another
 * polygon is drawn.
 *
 * @author ftodoric
 * @date 05/04/2020
 */
int main(int argc, char **argv)
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(200, 100);
    glutInit(&argc, argv);

    window = glutCreateWindow("Convex Polygon");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);
    glutMouseFunc(convexPolygonMouse);
    glutKeyboardFunc(convexPolygonKeyboard);
    printf("Choose whenever the color of the polygon filling:\n");
    printf("\tr - red\n\tg - green\n\tb - blue\n\tk - black\n");
    printf("Set vertex coordinates CLOCKWISE with left mouse click.\n");
    printf("Button 'd' ends the input and draws the polygon.\n");
    printf("Button 'c' or right mouse click clears the screen.\n");

    glutMainLoop();
    return 0;
}

void myDisplay()
{
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    // background color
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clearing the line after every drawn line

    glFlush();
}

void myReshape(int w, int h)
{
    width = w;
    height = h;                      // new window width and height
    glViewport(0, 0, width, height); // opening space in window

    glMatrixMode(GL_PROJECTION);     // projection matrix
    glLoadIdentity();                // identity matrix
    gluOrtho2D(0, width, 0, height); // perpendicular projection
    glMatrixMode(GL_MODELVIEW);      // view matrix
    glLoadIdentity();                // identity matrix

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT);         // clearing background
    glPointSize(1.0);                     // set point of the line size
    glColor3f(0.0f, 0.0f, 0.0f);          // line color
}

void convexPolygon(GLint v_x[], GLint v_y[])
{
    for (int i = 0; i < number_of_vertices - 1; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex2i(v_x[i], v_y[i]);
            glVertex2i(v_x[i + 1], v_y[i + 1]);
        }
        glEnd();
        glBegin(GL_LINES);
        {
            glVertex2i(v_x[number_of_vertices - 1], v_y[number_of_vertices - 1]);
            glVertex2i(v_x[0], v_y[0]);
        }
        glEnd();
    }
    printf("Input vertex T: ");
}

void convexPolygonMouse(int button, int state, int x, int y)
{
    vec3 v1, v2, B;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (is_drawn)
        {
            T_x = x;
            T_y = height - y;
            printf("(%d, %d)\n", T_x, T_y);
            glBegin(GL_POINT);
            glVertex2i(T_x, T_y);
            glEnd();

            is_vertex_in_polygon(T_x, T_y);

            color_the_polygon();

            is_drawn = FALSE;
            number_of_vertices = 0;
        }
        else
        {
            vertices_x[number_of_vertices] = x;
            vertices_y[number_of_vertices] = height - y;
            number_of_vertices++;

            if (number_of_vertices >= 2)
            {
                v1 = vec3(vertices_x[number_of_vertices - 2], vertices_y[number_of_vertices - 2], 1);
                v2 = vec3(vertices_x[number_of_vertices - 1], vertices_y[number_of_vertices - 1], 1);
                B = cross(v1, v2);
                edgeVec[number_of_vertices - 2] = B;
            }

            printf("Coordinates V%d: (%d, %d)\n", number_of_vertices, x, height - y);
        }

        glFlush();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        myReshape(width, height);
    }
}

/**
 * KEYS:    d - draws the polygon restarting the coordinates input
 *          c - clears the screen when new polygon is drawn
 *          colors: r - red
 *                  g - green
 *                  b - blue
 *                  k - black
 */
void convexPolygonKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    vec3 v1, v2, B, v3;

    switch (theKey)
    {
    case 'd':
        if (number_of_vertices < 3)
        {
            printf("ERROR: 3 vertices are required for a polygon.\n");
            break;
        }

        // insert the last edge vector
        v1 = vec3(vertices_x[number_of_vertices - 1], vertices_y[number_of_vertices - 1], 1);
        v2 = vec3(vertices_x[0], vertices_y[0], 1);
        B = cross(v1, v2);
        edgeVec[number_of_vertices - 1] = B;

        // check if convex
        for (int i = 0; i < number_of_vertices; i++)
        {
            if (i <= number_of_vertices - 3)
            {
                v3 = vec3(vertices_x[i + 2], vertices_y[i + 2], 1);
                if (dot(edgeVec[i], v3) >= 0)
                {
                    printf("ERROR: Polygon not convex.\n");
                    number_of_vertices = 0;
                    return;
                }
            }
            else
            {
                v3 = vec3(vertices_x[i + 2 - number_of_vertices], vertices_y[i + 2 - number_of_vertices], 1);
                if (dot(edgeVec[i], v3) >= 0)
                {
                    printf("ERROR: Polygon not convex.\n");
                    number_of_vertices = 0;
                    return;
                }
            }
        }

        convexPolygon(vertices_x, vertices_y);
        is_drawn = TRUE;
        break;
    case 'c':
        myReshape(width, height);
        break;

    case 'r':
        rgb[0] = 1;
        rgb[1] = 0;
        rgb[2] = 0;
        break;
    case 'g':
        rgb[0] = 0;
        rgb[1] = 1;
        rgb[2] = 0;
        break;
    case 'b':
        rgb[0] = 0;
        rgb[1] = 0;
        rgb[2] = 1;
        break;
    case 'k':
        rgb[0] = 0;
        rgb[1] = 0;
        rgb[2] = 0;
        break;
    }
    glFlush();
}

void printVector(vec3 v)
{
    printf("[%.2f, %.2f, %.2f]\n", v.x, v.y, v.z);
}

/**
 * Checks if the given vertex
 * is inside or outside of the polygon.
 */
void is_vertex_in_polygon(GLint x, GLint y)
{
    vec3 T_vec = vec3(x, y, 1);
    bool is_inside = TRUE;

    for (int i = 0; i < number_of_vertices; i++)
    {
        if (dot(T_vec, edgeVec[i]) > 0)
        {
            is_inside = FALSE;
            break;
        }
    }

    printf("%s\n", is_inside ? "Vertex T is inside the polygon!" : "Vertex T is outside the polygon!");
}

/**
 * Method for coloring the polygon with current color.
 */
void color_the_polygon()
{
    GLint xmin = vertices_x[0], xmax = vertices_x[0];
    GLint ymin = vertices_y[0], ymax = vertices_y[0];
    vec3 left[MAX_N], right[MAX_N];
    int l_count = 0;
    int r_count = 0;
    GLint xl, xr, L, D;

    printf("Coloring the polygon...\nPolygon colored.\n");
    glColor3f(rgb[0], rgb[1], rgb[2]); // line color

    // find minimums and maximums
    for (int i = 1; i < number_of_vertices; i++)
    {
        if (vertices_x[i] < xmin)
            xmin = vertices_x[i];
        if (vertices_x[i] > xmax)
            xmax = vertices_x[i];
        if (vertices_y[i] < ymin)
            ymin = vertices_y[i];
        if (vertices_y[i] > ymax)
            ymax = vertices_y[i];
    }

    // find right and left edges
    for (int i = 0; i < number_of_vertices - 1; i++)
    {
        if (vertices_y[i] < vertices_y[i + 1])
            left[l_count++] = edgeVec[i];
        else
            right[r_count++] = edgeVec[i];
    }
    if (vertices_y[number_of_vertices - 1] < vertices_y[0])
        left[l_count++] = edgeVec[number_of_vertices - 1];
    else
        right[r_count++] = edgeVec[number_of_vertices - 1];

    // draw horizontal lines in color
    for (int yp = ymin; yp <= ymax; yp++)
    {
        L = xmin;
        D = xmax;
        for (int i = 0; i < l_count; i++)
        {
            if (left[i].x == 0)
                continue;
            xl = (-left[i].y * yp - left[i].z) / left[i].x;
            if (xl >= L)
                L = xl;
        }
        for (int i = 0; i < r_count; i++)
        {
            if (right[i].x == 0)
                continue;
            xr = (-right[i].y * yp - right[i].z) / right[i].x;
            if (xr <= D)
                D = xr;
        }
        // draw the colored line
        glBegin(GL_LINES);
        {
            glVertex2i((int)L, yp);
            glVertex2i((int)D, yp);
        }
        glEnd();
    }

    glColor3f(0, 0, 0); // back to black color
}
