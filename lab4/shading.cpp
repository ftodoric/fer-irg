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
    GLfloat gouraud;
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
int number_of_polygons = 0;

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

void renderScene();

void miniMax();

void transformAndProject();

void findConstantIntensity(int polygon_index);

double findGouraudIntensity(double x, double y, double z);

void setColor(double I, char shading_color);

bool equalVector3(vec3 v1, vec3 v2);

bool getMax(double a, double b);

void printVector3(vec3 v);

void printVector4(vec4 v);

void printMatrix4(mat4 m);

/**
 * This program loads an object from a file, renders it from different perspectives
 * and applies different methods of shading.
 *
 * @author ftodoric
 * @date 16/05/2020
 */
int main(int argc, char **argv)
{
    parseFile("C:/VSC-Workspace/IRG-labs/objects/kocka.obj");

    // viewpoint input
    cout << "Set viewpoint O:" << endl;
    cin >> O.x >> O.y >> O.z;
    O.w = 1.0f;

    // shading method input
    cout << "Set shading method: (const - constant shading, gouraud - gouraud shading)" << endl;
    cin >> shading_method;

    if (strcmp(shading_method, "const") != 0 && strcmp(shading_method, "gouraud") != 0 &&
        strcmp(shading_method, "none") != 0)
    {
        printf("[ERR] Wrong shading method!\nTerminating program.");
        exit(0);
    }

    // shading color input
    if (strcmp(shading_method, "none") != 0)
    {
        // light source input
        cout << "Set light source coordinates:" << endl;
        cin >> L.x >> L.y >> L.z;
    }

    // shading color input
    if (strcmp(shading_method, "none") != 0)
    {
        cout << "Set shading color: r - red, g - green, b - blue" << endl;
        cin >> shading_color;
    }

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
    case 27:
        exit(0);
    default:
        break;
    }

    glutPostRedisplay();
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
        vertices[i].gouraud = findGouraudIntensity(originals[i].x, originals[i].y, originals[i].z);
    }

    miniMax();

    transformAndProject();

    glPushMatrix();
    glTranslatef((float)width / 2, (float)height / 2, 0);
    glScalef(50.0f, 50.0f, 1.0f);

    // drawing object
    for (int i = 0; i < number_of_polygons; i++)
    {
        // check if polygon is visible
        if (dot(O, vec4(planes[i].A, planes[i].B, planes[i].C, planes[i].D)) <= 0)
            continue;

        if (strcmp(shading_method, "const") == 0)
        {
            findConstantIntensity(i);
            setColor(I_const, shading_color);

            glBegin(GL_POLYGON);
            glVertex2f(vertices[polygons[i].first_vertex - 1].x,
                       vertices[polygons[i].first_vertex - 1].y);
            glVertex2f(vertices[polygons[i].second_vertex - 1].x,
                       vertices[polygons[i].second_vertex - 1].y);
            glVertex2f(vertices[polygons[i].third_vertex - 1].x,
                       vertices[polygons[i].third_vertex - 1].y);
            glEnd();
        }
        else if (strcmp(shading_method, "gouraud") == 0)
        {
            glBegin(GL_POLYGON);
            setColor(vertices[polygons[i].first_vertex - 1].gouraud, shading_color);
            glVertex2f(vertices[polygons[i].first_vertex - 1].x,
                       vertices[polygons[i].first_vertex - 1].y);
            setColor(vertices[polygons[i].second_vertex - 1].gouraud, shading_color);
            glVertex2f(vertices[polygons[i].second_vertex - 1].x,
                       vertices[polygons[i].second_vertex - 1].y);
            setColor(vertices[polygons[i].third_vertex - 1].gouraud, shading_color);
            glVertex2f(vertices[polygons[i].third_vertex - 1].x,
                       vertices[polygons[i].third_vertex - 1].y);
            glEnd();
        }
        else if (strcmp(shading_method, "none") == 0)
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
}

void miniMax()
{
    // calculating min and max values
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
    double size_x = xmax - xmin;
    double center_x = (xmax + xmin) / 2;
    double size_y = ymax - ymin;
    double center_y = (ymax + ymin) / 2;
    double size_z = zmax - zmin;
    double center_z = (zmax + zmin) / 2;

    //scaling and translating object
    for (int i = 0; i < number_of_vertices; i++)
    {
        vertices[i].x = (vertices[i].x - center_x) * (2 / getMax(size_x, getMax(size_y, size_z)));
        vertices[i].y = (vertices[i].y - center_y) * (2 / getMax(size_x, getMax(size_y, size_z)));
        vertices[i].z = (vertices[i].z - center_z) * (2 / getMax(size_x, getMax(size_y, size_z)));
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

void findConstantIntensity(int polygon_index)
{
    // determining polygon center
    vec3 center = vec3(0.0f, 0.0f, 0.0f);
    vec3 vertex;
    vertex = vec3(originals[polygons[polygon_index].first_vertex].x,
                  originals[polygons[polygon_index].first_vertex].y,
                  originals[polygons[polygon_index].first_vertex].z);
    center += vertex;
    vertex = vec3(originals[polygons[polygon_index].second_vertex].x,
                  originals[polygons[polygon_index].second_vertex].y,
                  originals[polygons[polygon_index].second_vertex].z);
    center += vertex;
    vertex = vec3(originals[polygons[polygon_index].third_vertex].x,
                  originals[polygons[polygon_index].third_vertex].y,
                  originals[polygons[polygon_index].third_vertex].z);
    center += vertex;
    center = (1.0f / (float)3) * center;

    vec3 vecL = L - center;
    vec3 N = vec3(planes[polygon_index].A, planes[polygon_index].B, planes[polygon_index].C);

    vecL = normalize(vecL);
    N = normalize(N);

    float scalar = dot(vecL, N);
    if (scalar < 0)
        scalar = 0;

    I_const = Ia * ka + Ii * kd * scalar; // 100 + 155 * scalar
}

double findGouraudIntensity(double x, double y, double z)
{
    // find adjacent polygons to the vertex
    vec3 vertex = vec3(x, y, z);
    bool adjacent_polygons[MAX_POLYGONS];
    vec3 poly_v1, poly_v2, poly_v3;
    for (int i = 0; i < number_of_polygons; i++)
    {
        poly_v1 = vec3(originals[polygons[i].first_vertex - 1].x, originals[polygons[i].first_vertex - 1].y,
                       originals[polygons[i].first_vertex - 1].z);
        poly_v2 = vec3(originals[polygons[i].second_vertex - 1].x, originals[polygons[i].second_vertex - 1].y,
                       originals[polygons[i].second_vertex - 1].z);
        poly_v3 = vec3(originals[polygons[i].third_vertex - 1].x, originals[polygons[i].third_vertex - 1].y,
                       originals[polygons[i].third_vertex - 1].z);
        adjacent_polygons[i] = equalVector3(poly_v1, vertex) || equalVector3(poly_v2, vertex) ||
                               equalVector3(poly_v3, vertex);
    }

    // find vertex normal
    vec3 N;
    vec3 vertexN = vec3(0.0f, 0.0f, 0.0f);
    int num_of_adjacents = 0;
    for (int i = 0; i < number_of_polygons; i++)
    {
        if (adjacent_polygons[i])
        {
            N = vec3(planes[i].A, planes[i].B, planes[i].C);
            N = normalize(N);
            vertexN += N;
            num_of_adjacents++;
        }
    }
    vertexN = (1.0f / (float)num_of_adjacents) * vertexN;
    vertexN = normalize(vertexN);

    // find scalar product of vector pointing to light source and vertex normal
    vec3 vecL = L - vertex;
    vecL = normalize(vecL);

    double scalar = dot(vecL, vertexN);
    if (scalar < 0)
        scalar = 0;

    // Phong intensity formula
    double I_gouraud = Ia * ka + Ii * kd * scalar;

    return I_gouraud;
}

void setColor(double I, char color)
{
    switch (color)
    {
    case 'r':
        glColor3ub(I, 0, 0);
        break;
    case 'g':
        glColor3ub(0, I, 0);
        break;
    case 'b':
        glColor3ub(0, 0, I);
        break;
    default:
        break;
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

bool getMax(double a, double b)
{
    if (a >= b)
        return a;
    else
        return b;
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
