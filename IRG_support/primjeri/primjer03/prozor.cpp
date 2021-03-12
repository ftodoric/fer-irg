// Kompajliranje:
// g++ -o prozor prozor.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

// Ukljuci potporu za osnovne tipove glm-a: vektore i matrice
#include <glm/glm.hpp>

// Ukljuci funkcije za projekcije, transformacije pogleda i slicno
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

// Ako ste na Ubuntu-u i program se odmah po pokretanju srusi sa segmentation fault,
// radi se o poznatom bugu:
// https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
// potreban je sljedeci define i program treba linkati s -lpthread:
#ifndef _WIN32
#define LINUX_UBUNTU_SEGFAULT
#endif

#ifdef LINUX_UBUNTU_SEGFAULT
//ss1
#include <pthread.h>
#endif

// Nasa pomocna biblioteka za ucitavanje, prevodenje i linkanje programa shadera
#include "util.hpp"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 256, sub_height = 256;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboard		(unsigned char theKey, int mouseX, int mouseY);

GLuint vertexArrayID;
GLuint programID;
GLuint MVPMatrixID;

bool init_data(); // nasa funkcija za inicijalizaciju podataka

//*********************************************************************************
//	Glavni program.
//*********************************************************************************

int main(int argc, char ** argv)
{
	// Sljedeci blok sluzi kao bugfix koji je opisan gore
	#ifdef LINUX_UBUNTU_SEGFAULT
        //ss2
        int i=pthread_getconcurrency();        
	#endif

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Prozor");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	glutMainLoop();
    	return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
	   -1.0f, -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
	   0.0f,  1.0f, 0.0f,
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = loadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our "MVP" uniform for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
 	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)sub_width / (float)sub_height, 0.1f, 100.0f);
  
 	// Or, for an ortho camera :
 	//glm::mat4 projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
  
 	// Camera matrix
 	glm::mat4 view = glm::lookAt(
	    glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
	    glm::vec3(0,0,0), // and looks at the origin
	    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
 	);
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelViewProjection : multiplication of our 3 matrices
 	glm::mat4 mvp = projection * view * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID);

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Zatraži crtanje toka vrhova pri čemu ih se interpretira kao trokute
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
	sub_width = width;                      	// zapamti novu sirinu prozora
    	sub_height = height;				// zapamti novu visinu prozora
}

//*********************************************************************************
//	Mis.
//*********************************************************************************

void myMouse(int button, int state, int x, int y)
{
	//	Lijeva tipka - crta pocetnu tocku ili liniju.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glutPostRedisplay();
	}
	//	Desna tipka - brise canvas. 
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glutPostRedisplay();
	}
}


//*********************************************************************************
//	Tastatura tipke - r, g, b, k - mijenjaju boju.
//*********************************************************************************

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey)
	{
		case 'r':
			break;
	}
	glutPostRedisplay();
}

