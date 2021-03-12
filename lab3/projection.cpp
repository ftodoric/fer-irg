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
} vertex;
vertex *originals = (vertex *)malloc(MAX_VERTICES * sizeof(vertex));
vertex *vertices = (vertex *)malloc(MAX_VERTICES * sizeof(vertex));
int number_of_vertices = 0;

typedef struct
{
    int first_vertex;
    int second_vertex;
    int third_vertex;
} polygon;
polygon *polygons = (polygon *)malloc(MAX_POLYGONS * sizeof(polygon));
GLint number_of_polygons = 0;

vec3 center_vector;

vec4 O; // viewpoint O

// main window pointer and initial size
GLuint window;
GLuint width = 800, height = 500;

// function prototypes
void parseFile(const string &path);

void display();

void reshape(int width, int height);

void renderScene();

void transformAndProject();

void renderObject();

void keyboard(unsigned char key, int mouseX, int mouseY);

void printVector3(vec3 v);

void printVector4(vec4 v);

void printMatrix4(mat4 m);

/**
 * This program loads an object from file descriptor and
 * renders it from different perspectives.
 *
 * @author ftodoric
 * @date 03/05/2020
 */
int main(int argc, char **argv)
{
    parseFile("C:/CLionWorkspace/IRG/vjezba5/objects/kocka.obj");

    // viewpoints input
    cout << "Viewpoint O:\n";
    cin >> O.x >> O.y >> O.z;
    O.w = 1;

    // calculating the center of the object
    vec3 vertex;
    center_vector = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertex = vec3(originals[i].x, originals[i].y, originals[i].z);
        center_vector += vertex;
    }
    center_vector = (1.0f / (float)number_of_vertices) * center_vector;
    //printVector3(center_vector);

    // initializing window and settings
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);

    window = glutCreateWindow("Loaded Object");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    cout << "Keys:\n"
            "q/a - increase/decrease O.x\tr/f - increase/decrease G.x\n"
            "w/s - increase/decrease O.y\tt/g - increase/decrease G.y\n"
            "e/d - increase/decrease O.z\tz/h - increase/decrease G.z"
         << endl;

    glutMainLoop();

    free(originals);
    free(vertices);
    free(polygons);
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
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
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

void renderScene()
{
    // copy original vertices
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertices[i].x = originals[i].x;
        vertices[i].y = originals[i].y;
        vertices[i].z = originals[i].z;
    }

    transformAndProject();

    glPushMatrix();
    glTranslatef((float)width / 2, (float)height / 2, 0);
    glScalef(100.0f, 100.0f, 1.0f);

    // drawing the object
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

    glPopMatrix();
}

void transformAndProject()
{
    // calculate transformation matrices
    mat4 T1 = mat4(1, 0, 0, -O[0],
                   0, 1, 0, -O[1],
                   0, 0, 1, -O[2],
                   0, 0, 0, 1);

    vec4 G1 = vec4(center_vector, 1.0f) * T1;

    // z-axis rotation
    double sin_alfa = G1[1] / sqrt(G1[0] * G1[0] + G1[1] * G1[1]);
    double cos_alfa = G1[0] / sqrt(G1[0] * G1[0] + G1[1] * G1[1]);
    mat4 T2 = mat4(cos_alfa, sin_alfa, 0, 0,
                   -sin_alfa, cos_alfa, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1);

    vec4 G2 = G1 * T2;

    // y-axis rotation
    double sin_beta = G2[0] / sqrt(G2[0] * G2[0] + G2[2] * G2[2]);
    double cos_beta = G2[2] / sqrt(G2[0] * G2[0] + G2[2] * G2[2]);
    mat4 T3 = mat4(cos_beta, 0, -sin_beta, 0,
                   0, 1, 0, 0,
                   sin_beta, 0, cos_beta, 0,
                   0, 0, 0, 1);

    vec4 G3 = G2 * T3;

    // matching the display coordinate system
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

        vertices[i].x = temp[0] / temp[2] * H;
        vertices[i].y = temp[1] / temp[2] * H;
        vertices[i].z = 0;
    }
}

void keyboard(unsigned char key, int mouseX, int mouseY)
{
    double d = 0.5;

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
    case 27:
        exit(0);
    default:
        break;
    }

    glutPostRedisplay();
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
