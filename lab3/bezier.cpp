#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cmath>

#include <GL/freeglut.h>
#include <glm/glm.hpp>

#define MAX_VERTICES 1000000
#define MAX_POLYGONS MAX_VERTICES / 3
#define ONE_UNIT 100

using namespace glm;
using namespace std;

// data structures
typedef struct
{
    GLdouble x;
    GLdouble y;
    GLdouble z;
} Vertex;
Vertex *originals = (Vertex *)malloc(MAX_VERTICES * sizeof(Vertex));
Vertex *vertices = (Vertex *)malloc(MAX_VERTICES * sizeof(Vertex));
GLint number_of_vertices = 0;

typedef struct
{
    int first_vertex;
    int second_vertex;
    int third_vertex;
} Poly;
Poly *polygons = (Poly *)malloc(MAX_POLYGONS * sizeof(Poly));
GLint number_of_polygons = 0;

int control_polygon[3];

GLdouble xmin, xmax, ymin, ymax, zmin, zmax;

vec3 center_vector;

vec4 O; // viewpoint

typedef struct
{
    GLfloat A;
    GLfloat B;
    GLfloat C;
    GLfloat D;
} Plane;
Plane *planes = (Plane *)malloc(MAX_POLYGONS * sizeof(Plane));

// shading data
char shading_method[10];
char shading_color;

vec3 L;
GLdouble I_const;
GLdouble Ia = 50, ka = 1.0;
GLdouble Ii = 255 - Ia, kd = 1.0;

// main window pointer and initial size
GLuint window;
GLuint width = 800, height = 500;

// function prototypes
void parseFile(const string &path);

void display();

void reshape(int width, int height);

void keyboard(unsigned char key, int mouseX, int mouseY);

void renderBezier();

void renderScene();

void animation();

void transformAndProject();

bool equalVector3(vec3 v1, vec3 v2);

void printVector3(vec3 v);

void printVector4(vec4 v);

void printMatrix4(mat4 m);

int factorial(int num);

/**
 * This program loads an object from a file, renders it from different perspectives
 * and constructs Bézier path.
 *
 * @author ftodoric
 * @date 04/05/2020
 */
int main(int argc, char **argv)
{
    parseFile("C:/CLionWorkspace/IRG/vjezba5/objects/kocka.obj");

    // viewpoint input
    cout << "Set viewpoint O:" << endl;
    cin >> O.x >> O.y >> O.z;
    O.w = 1.0f;

    // calculating the center of the object
    vec3 vertex;
    center_vector = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertex = vec3(originals[i].x, originals[i].y, originals[i].z);
        center_vector += vertex;
    }
    center_vector = (1.0f / (float)number_of_vertices) * center_vector;

    // initializing window and settings
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);

    window = glutCreateWindow("Animation by Bezier curve");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    cout << "Press b to begin animation." << endl;

    glutMainLoop();

    free(originals);
    free(vertices);
    free(polygons);
    free(planes);
}

void parseFile(const string &path)
{
    // parsing the descriptor file
    ifstream objectFile(path);
    string line;
    string delimiter = " ";
    int pos = 0;

    if (objectFile.is_open())
    {
        while (getline(objectFile, line))
        {
            if (line.length() == 0)
                continue;
            switch (line.at(0))
            {
            case '#':
            case ' ':
                break;
            case 'v':
                line.replace(0, 2, "");

                pos = line.find(delimiter);
                originals[number_of_vertices].x = stof(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                originals[number_of_vertices].y = stof(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                originals[number_of_vertices].z = stof(line.substr(0, pos));

                number_of_vertices++;
                break;
            case 'f':
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
            case 'c':
                line.replace(0, 2, "");

                pos = line.find(delimiter);
                control_polygon[0] = stoi(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                control_polygon[1] = stoi(line.substr(0, pos));
                line.erase(0, pos + delimiter.length());

                pos = line.find(delimiter);
                control_polygon[2] = stoi(line.substr(0, pos));

                break;
            default:
                break;
            }
        }
        objectFile.close();
    }
    else
        cout << "Unable to open file";
}

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    renderBezier();
    renderScene();
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
    double d = 0.8;

    switch (key)
    {
    // set O viewpoint
    case 'q':
        O = vec4(O.x + d, O.y, O.z, 1);
        break;
    case 'a':
        O = vec4(O.x - d, O.y, O.z, 1);
        break;
    case 'w':
        O = vec4(O.x, O.y + d, O.z, 1);
        break;
    case 's':
        O = vec4(O.x, O.y - d, O.z, 1);
        break;
    case 'e':
        O = vec4(O.x, O.y, O.z + d, 1);
        break;
    case 'd':
        O = vec4(O.x, O.y, O.z - d, 1);
        break;

        // set G viewpoint
    case 'r':
        center_vector = vec3(center_vector.x + d, center_vector.y, center_vector.z);
        break;
    case 'f':
        center_vector = vec3(center_vector.x - d, center_vector.y, center_vector.z);
        break;
    case 't':
        center_vector = vec3(center_vector.x, center_vector.y + d, center_vector.z);
        break;
    case 'g':
        center_vector = vec3(center_vector.x, center_vector.y - d, center_vector.z);
        break;
    case 'z':
        center_vector = vec3(center_vector.x, center_vector.y, center_vector.z + d);
        break;
    case 'h':
        center_vector = vec3(center_vector.x, center_vector.y, center_vector.z - d);
        break;
    case 'b':
        animation();
        break;
    case 27:
        exit(0);
    default:
        break;
    }

    glutPostRedisplay();
}

void renderBezier()
{
    glPushMatrix();

    glTranslatef((float)width / 2 + 200, (float)height / 2 + 100, 0);
    glScalef(100.0f, 100.0f, 1.0f);

    glBegin(GL_LINE_LOOP);
    glVertex2f(originals[control_polygon[0] - 1].x,
               originals[control_polygon[0] - 1].y);
    glVertex2f(originals[control_polygon[1] - 1].x,
               originals[control_polygon[1] - 1].y);
    glVertex2f(originals[control_polygon[2] - 1].x,
               originals[control_polygon[2] - 1].y);
    glEnd();

    double t = 0;
    double b;
    glBegin(GL_POINTS);
    while (t < 1)
    {
        vec3 p = vec3(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 3; i++)
        {
            vec3 r = vec3(originals[control_polygon[i] - 1].x, originals[control_polygon[i] - 1].y,
                          originals[control_polygon[i] - 1].z);

            b = (double)factorial(2) / (factorial(i) * factorial(2 - i)) * pow(t, i) * pow(1 - t, 2 - i);

            r[0] *= b;
            r[1] *= b;
            r[2] *= b;

            p[0] += r[0];
            p[1] += r[1];
            p[2] += r[2];
        }
        t += 0.01;

        glVertex2f(p[0], p[1]);
    }
    glEnd();

    glPopMatrix();
}

void renderScene()
{
    //calculating plane equations of object's polygons
    GLdouble x1, x2, x3, y1, y2, y3, z1, z2, z3;
    for (int i = 0; i < number_of_polygons; i++)
    {
        x1 = originals[polygons[i].first_vertex - 1].x;
        x2 = originals[polygons[i].second_vertex - 1].x;
        x3 = originals[polygons[i].third_vertex - 1].x;
        y1 = originals[polygons[i].first_vertex - 1].y;
        y2 = originals[polygons[i].second_vertex - 1].y;
        y3 = originals[polygons[i].third_vertex - 1].y;
        z1 = originals[polygons[i].first_vertex - 1].z;
        z2 = originals[polygons[i].second_vertex - 1].z;
        z3 = originals[polygons[i].third_vertex - 1].z;
        planes[i].A = (y2 - y1) * (z3 - z1) - (y3 - y1) * (z2 - z1);
        planes[i].B = (z2 - z1) * (x3 - x1) - (z3 - z1) * (x2 - x1);
        planes[i].C = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
        planes[i].D = -x1 * planes[i].A - y1 * planes[i].B - z1 * planes[i].C;
    }

    // copy original vertices
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertices[i].x = originals[i].x;
        vertices[i].y = originals[i].y;
        vertices[i].z = originals[i].z;
    }

    glPushMatrix();
    glTranslatef((float)width / 2, (float)height / 2, 0);
    glScalef(100.0f, 100.0f, 1.0f);

    transformAndProject();

    // drawing object
    for (int i = 0; i < number_of_polygons; i++)
    {

        // check if polygon is visible
        if (dot(O, vec4(planes[i].A, planes[i].B, planes[i].C, planes[i].D)) <= 0)
            continue;

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

void animation()
{
    double t = 0;
    double b;
    while (t < 1)
    {
        vec3 p = vec3(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 3; i++)
        {
            vec3 r = vec3(originals[control_polygon[i] - 1].x, originals[control_polygon[i] - 1].y,
                          originals[control_polygon[i] - 1].z);

            b = (double)factorial(2) / (factorial(i) * factorial(2 - i)) * pow(t, i) * pow(1 - t, 2 - i);

            r[0] *= b;
            r[1] *= b;
            r[2] *= b;

            p[0] += r[0];
            p[1] += r[1];
            p[2] += r[2];
        }
        t += 0.01;

        O = vec4(O.x + p[0], O.y + p[1], O.z + p[2], 1);

        Sleep(10);
        display();
    }
}

void transformAndProject()
{
    // calculate transformation matrices
    mat4 T1 = mat4(1, 0, 0, -O[0],
                   0, 1, 0, -O[1],
                   0, 0, 1, -O[2],
                   0, 0, 0, 1);

    vec4 G1 = vec4(center_vector, 1.0f) * T1;

    double sin_alfa = G1[1] / sqrt(G1[0] * G1[0] + G1[1] * G1[1]);
    double cos_alfa = G1[0] / sqrt(G1[0] * G1[0] + G1[1] * G1[1]);
    mat4 T2 = mat4(cos_alfa, sin_alfa, 0, 0,
                   -sin_alfa, cos_alfa, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);

    vec4 G2 = G1 * T2;

    double sin_beta = G2[0] / sqrt(G2[0] * G2[0] + G2[2] * G2[2]);
    double cos_beta = G2[2] / sqrt(G2[0] * G2[0] + G2[2] * G2[2]);
    mat4 T3 = mat4(cos_beta, 0, -sin_beta, 0,
                   0, 1, 0, 0,
                   sin_beta, 0, cos_beta, 0,
                   0, 0, 0, 1);

    vec4 G3 = G2 * T3;

    mat4 T4 = mat4(0, 1, 0, 0,
                   -1, 0, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);
    mat4 T5 = mat4(-1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);

    mat4 transformation = T1 * T2 * T3 * T4 * T5;

    // projection matrix
    double H = G3[2];
    mat4 projection = mat4(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 0, 0,
                           0, 0, (1.f / H), 0);

    // transform and project all vertices
    vec4 temp;
    for (int i = 0; i < number_of_vertices; i++)
    {
        temp = vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
        temp = temp * transformation;
        //temp = temp * projection;
        vertices[i].x = temp[0] / temp[2] * H;
        vertices[i].y = temp[1] / temp[2] * H;
        vertices[i].z = 0;
    }
}

bool equalVector3(vec3 v1, vec3 v2)
{
    if (v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2])
    {
        return true;
    }
    else
        return false;
}

void printVector3(vec3 v)
{
    printf("[%.2f, %.2f, %.2f]\n", v.x, v.y, v.z);
}

void printVector4(vec4 v)
{
    printf("[%.2f, %.2f, %.2f, %.2f]\n", v.x, v.y, v.z, v.w);
}

void printMatrix4(mat4 m)
{
    for (int i = 0; i < m.length(); ++i)
    {
        for (int j = 0; j < m.length(); ++j)
        {
            cout << m[j][i] << " ";
        }
        cout << endl;
    }
}

int factorial(int num)
{
    int fact = 1;
    if (num == 0)
        return fact;
    for (int i = 2; i <= num; i++)
    {
        fact *= i;
    }
    return fact;
}
