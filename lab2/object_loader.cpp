#include <cstdio>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <string>

#define MAX_VERTICES 26000
#define MAX_POLYGONS 50000
#define ONE_UNIT 100

using namespace glm;
using namespace std;

// data structures
typedef struct
{
    GLdouble x;
    GLdouble y;
    GLdouble z;
} vertex;
vertex *vertices = (vertex *)malloc(MAX_VERTICES * sizeof(vertex));
GLint number_of_vertices = 0;

typedef struct
{
    int first_vertex;
    int second_vertex;
    int third_vertex;
} polygon;
polygon *polygons = (polygon *)malloc(MAX_POLYGONS * sizeof(polygon));
GLint number_of_polygons = 0;

GLdouble xmin, xmax, ymin, ymax, zmin, zmax;

vec3 center_vector = vec3(0.0f, 0.0f, 0.0f);

// testing vertex
struct VertexV
{
    GLdouble x;
    GLdouble y;
    GLdouble z;
} V;

typedef struct
{
    GLfloat A;
    GLfloat B;
    GLfloat C;
    GLfloat D;
} Plane;
Plane *planes = (Plane *)malloc(MAX_POLYGONS * sizeof(Plane));

// main window pointer and initial size
GLuint window;
GLuint width = 1200, height = 700;

// function prototypes
void myDisplay();

void myReshape(int width, int height);

void printVector(vec3 v);

void renderObject();

/**
 * This program takes vertices and polygons and draws object defined by them.
 *
 * @author ftodoric
 * @date 07/04/2020
 */
int main(int argc, char **argv)
{
    V.x = stof(argv[1]);
    V.y = stof(argv[2]);
    V.z = stof(argv[3]);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);
    glutInit(&argc, argv);

    window = glutCreateWindow("Loaded Object");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);

    glutMainLoop();

    free(vertices);
    free(polygons);
    free(planes);
}

void myDisplay()
{
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    // background color
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clearing the line after every drawn line

    ifstream objectFile("C:/VSC-Workspace/IRG-labs/objects/kocka.obj");
    std::string line;
    std::string delimiter = " ";
    int pos = 0;

    // parsing the file
    if (objectFile.is_open())
    {
        while (getline(objectFile, line))
        {
            if (line.length() == 0)
                continue;
            switch (line.at(0))
            {
            case '#':
                break;
            case ' ':
                break;
            case 'v':
                pos = 0;
                line.replace(0, 2, "");

                pos = line.find(delimiter);
                vertices[number_of_vertices].x = stof(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                vertices[number_of_vertices].y = stof(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                vertices[number_of_vertices].z = stof(line.substr(0, pos));

                number_of_vertices++;
                break;
            case 'f':
                pos = 0;
                line.replace(0, 2, "");

                pos = line.find(delimiter);
                polygons[number_of_polygons].first_vertex = stoi(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                polygons[number_of_polygons].second_vertex = stoi(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                polygons[number_of_polygons].third_vertex = stoi(line.substr(0, pos));

                number_of_polygons++;
                break;
            default:
                break;
            }
        }
        objectFile.close();
    }
    else
        cout << "Unable to open file";

    xmin = xmax = vertices[0].x;
    ymin = ymax = vertices[0].y;
    zmin = zmax = vertices[0].z;
    for (int i = 1; i < number_of_vertices; i++)
    {
        if (vertices[i].x < xmin)
            xmin = vertices[i].x;
        if (vertices[i].x > xmax)
            xmax = vertices[i].x;
        if (vertices[i].y < ymin)
            ymin = vertices[i].y;
        if (vertices[i].y > ymax)
            ymax = vertices[i].y;
        if (vertices[i].z < zmin)
            zmin = vertices[i].z;
        if (vertices[i].z > zmax)
            zmax = vertices[i].z;
    }

    // calculating the center of the object
    vec3 vertex;
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertex = vec3(vertices[i].x, vertices[i].y, vertices[i].z);
        center_vector += vertex;
    }
    center_vector = (1.0f / (float)number_of_vertices) * center_vector;

    //scaling and translating object
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertices[i].x = (vertices[i].x - center_vector.x) / (abs(xmax) + abs(xmin)) * ONE_UNIT + width / 2.0f;
        vertices[i].y = (vertices[i].y - center_vector.y) / (abs(ymax) + abs(ymin)) * ONE_UNIT + height / 2.0f;
        vertices[i].z = vertices[i].z / (abs(zmax) + abs(zmin)) * ONE_UNIT;
    }
    center_vector.x = width / 2.0f;
    center_vector.y = height / 2.0f;
    center_vector.z = center_vector.z / (abs(zmax) + abs(zmin)) * ONE_UNIT;

    //calculating all plane equations of object's polygons
    GLdouble x1, x2, x3, y1, y2, y3, z1, z2, z3;
    for (int i = 0; i < number_of_polygons; i++)
    {
        x1 = vertices[polygons[i].first_vertex].x;
        x2 = vertices[polygons[i].second_vertex].x;
        x3 = vertices[polygons[i].third_vertex].x;
        y1 = vertices[polygons[i].first_vertex].y;
        y2 = vertices[polygons[i].second_vertex].y;
        y3 = vertices[polygons[i].third_vertex].y;
        z1 = vertices[polygons[i].first_vertex].z;
        z2 = vertices[polygons[i].second_vertex].z;
        z3 = vertices[polygons[i].third_vertex].z;
        planes[i].A = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
        planes[i].B = -(x2 - x1) * (z3 - z1) - (z2 - z1) * (x3 - x1);
        planes[i].C = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
        planes[i].D = -x1 * planes[i].A - y1 * planes[i].B - z1 * planes[i].C;
    }

    //testing vertex V
    bool vertex_is_inside = true;
    vec4 R;
    vec4 vec_V = vec4(V.x, V.y, V.z, 1.0f);
    for (int i = 0; i < number_of_polygons; i++)
    {
        R = vec4(planes[i].A, planes[i].B, planes[i].C, planes[i].D);
        //printf("%f %f %f %f\n", planes[i].A, planes[i].B, planes[i].C, planes[i].D);
        //printf("%f\n", dot(vec_V, R));
        if (dot(vec_V, R) < 0)
        {
            vertex_is_inside = false;
            break;
        }
    }
    printf("%s\n", vertex_is_inside ? "Vertex V is inside the object." : "Vertex V is outside the object.");

    renderObject();
    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    width = w;
    height = h;                      // new window width and height
    glViewport(0, 0, width, height); // opening space in window

    glMatrixMode(GL_PROJECTION); // projection matrix
    glLoadIdentity();            // identity matrix
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW); // view matrix
    glLoadIdentity();           // identity matrix

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT);         // clearing background
    glPointSize(1.0);                     // set point of the line size
    glColor3f(0.0f, 0.0f, 0.0f);          // line color
}

void printVector(vec3 v)
{
    printf("[%.2f, %.2f, %.2f]\n", v.x, v.y, v.z);
}

void renderObject()
{
    for (int i = 0; i < number_of_polygons; i++)
    {
        glBegin(GL_LINE_LOOP);
        glVertex2f(vertices[polygons[i].first_vertex - 1].x,
                   vertices[polygons[i].first_vertex - 1].y);
        glVertex2f(vertices[polygons[i].second_vertex - 1].x,
                   vertices[polygons[i].second_vertex - 1].y);
        glVertex2f(vertices[polygons[i].third_vertex - 1].x,
                   vertices[polygons[i].third_vertex - 1].y);
        glEnd();
    }
}
