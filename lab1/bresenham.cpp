//*********************************************************************************
//	Iscrtavanje linije 0-45, usporedba s glLine naredbom.
//	Pocetna i krajnja koordinata se zadaju lijevom tiplom misa, desnom se brise.
//	r, g, b, k s tastature mijenja boju.
//
//	Zadatak: Treba preraditi void myLine(GLint xa, GLint ya, GLint xb, GLint yb)
//	tako da radi ispravno za sve kutove
//
//  Potreban je glut - http://freeglut.sourceforge.net/
//  Za MSVC skinuti: 
//  http://files.transmissionzero.co.uk/software/development/GLUT/freeglut-MSVC.zip
//  postaviti - Properties - Configuration properties VC++Directories - Include Dir
//                                                                    - Library Dir
//*********************************************************************************

#include <stdio.h>
#include <GL/freeglut.h>

//*********************************************************************************
//	Pocetna tocka Lx[1], Ly[1] Krajnja tocke Lx[2], Ly[2] linije.
//	Ix - stanje (zadana 1 ili 2 tocka)
//*********************************************************************************

GLdouble Lx[2], Ly[2];
GLint Ix;

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window;
GLuint width = 300, height = 300;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay();
void myReshape(int width, int height);
void myMouse(int button, int state, int x, int y);
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myLine(GLint xa, GLint ya, GLint xb, GLint yb);

//*********************************************************************************
//	Glavni program.
//*********************************************************************************

int main(int argc, char **argv) {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Line");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	printf("Lijevom tipkom misa zadaj dvije tocke - algoritam Bresenham-a\n");
	printf("Tipke r, g, b, k mijenjaju boju.\n");

	glutMainLoop();
	return 0;
}


//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay() {
	//printf("Pozvan myDisplay()\n");
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //  boja pozadine
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //brisanje nakon svake iscrtane linije
	glFlush();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int w, int h) {
	//printf("Pozvan myReshape()\n");
	width = w; height = h;               //promjena sirine i visine prozora
	Ix = 0;								//	indeks tocke 0-prva 1-druga tocka
	glViewport(0, 0, width, height);	//  otvor u prozoru

	glMatrixMode(GL_PROJECTION);		//	matrica projekcije
	glLoadIdentity();					//	jedinicna matrica
	gluOrtho2D(0, width, 0, height); 	//	okomita projekcija
	glMatrixMode(GL_MODELVIEW);			//	matrica pogleda
	glLoadIdentity();					//	jedinicna matrica

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // boja pozadine
	glClear(GL_COLOR_BUFFER_BIT);		//	brisanje pozadine
	glPointSize(1.0);					//	postavi velicinu tocke za liniju
	glColor3f(0.0f, 0.0f, 0.0f);		//	postavi boju linije
}

//*********************************************************************************
//	Crtaj moju liniju.
//*********************************************************************************

//crtanje linije s kutom u intervalu [0, 90]
void line0_90(GLint x1, GLint y1, GLint x2, GLint y2) {
	GLint x, y; // x, y - trenutni x i y u iscrtavanju linije
	GLint dx, dy; // dx, dy - pomak po x odnosno y osi
	GLint d;

	//interval [0, 45]
	if (y2 - y1 <= x2 - x1) {
		GLint x = x1;
		GLint y = y1;

		GLint dx = x2 - x1;
		GLint dy = y2 - y1;

		GLdouble d = dy / (double)dx - 0.5;

		glBegin(GL_POINTS);
		for (int i = 0; i <= dx; i++) {
			glVertex2i(x, y);
			if (d > 0) {
				y++;
				d = d - 1.0;
			}
			x++;
			d += dy / (double) dx;
		}
		glEnd();
	}
	else {
		//interval [45, 90]
		GLint x = y1;
		GLint y = x1;

		GLint dx = y2 - y1;
		GLint dy = x2 - x1;

		GLdouble d = dy / (double)dx - 0.5;

		glBegin(GL_POINTS);
		for (int i = 0; i <= dx; i++) {
			glVertex2i(y, x);
			if (d > 0) {
				y++;
				d = d - 1.0;
			}
			x++;
			d += dy / (double) dx;
		}
		glEnd();
	}
}

//crtanje pravca s kutom u intervalu [0, -90]
void line0_neg90(GLint x1, GLint y1, GLint x2, GLint y2) {
	GLint x, y, dx, dy, d;

	//interval [0, -45]
	if (-(y2 - y1) <= x2 - x1) {
		GLint x = x1;
		GLint y = y1;

		GLint dx = x2 - x1;
		GLint dy = -(y2 - y1);

		GLdouble d = dy / (double)dx - 0.5;

		glBegin(GL_POINTS);
		for (int i = 0; i <= dx; i++) {
			glVertex2i(x, y);
			if (d > 0) {
				y--;
				d = d - 1.0;
			}
			x++;
			d += dy / (double)dx;
		}
		glEnd();
	}
	else {
		//interval [-45, -90]
		GLint x = -y1;
		GLint y = -x1;

		GLint dy = x2 - x1;
		GLint dx = -(y2 - y1);

		GLdouble d = dy / (double)dx - 0.5;

		glBegin(GL_POINTS);
		for (int i = 0; i <= dx; i++) {
			glVertex2i(-y, -x);
			if (d > 0) {
				y--;
				d = d - 1.0;
			}
			x++;
			d += dy / (double)dx;
		}
		glEnd();
	}
}

void myLine(GLint x1, GLint y1, GLint x2, GLint y2) {
	//razdvojeni slucaji za prikaz linija -> crtamo li nadesno ili nalijevo te intervali nagiba pravca: [0, 90] i [0, -90]
	if (x1 <= x2) {
		if (y1 <= y2) line0_90(x1, y1, x2, y2);
		else line0_neg90(x1, y1, x2, y2);
	} else {
		if (y1 >= y2) line0_90(x2, y2, x1, y1);
		else line0_neg90(x2, y2, x1, y1);
	}

	//crtanje gotove linije
	int offset = 20; //offset paralalenog prikazivanja s Bresenham-ovim algoritmom
	glBegin(GL_LINES);
	{
		glVertex2i(x1, y1 + offset);
		glVertex2i(x2, y2 + offset);
	}
	glEnd();
}

//*********************************************************************************
//	Mis.
//*********************************************************************************

void myMouse(int button, int state, int x, int y) {
	//	Lijeva tipka - crta pocetnu tocku ili liniju.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

	{
		//	Pamti krajnju tocke linije.
		Lx[Ix] = x;							//	upisi tocku
		Ly[Ix] = height - y;
		Ix = Ix ^ 1;						//	flip - druga tocka

		//	Crta prvu tocku ili liniju do druge tocke.
		if (Ix == 0)	myLine((int)Lx[0], (int)Ly[0], (int)Lx[1], (int)Ly[1]);
		else			glVertex2i(x, height - y);

		printf("Koordinate tocke %d: %d %d \n", Ix ^ 1, x, y);

		glFlush();
	}

	//	Desna tipka - brise canvas. 
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		myReshape(width, height);
	}
}

//*********************************************************************************
//	Tastatura tipke - r, g, b, k - mijenjaju boju.
//*********************************************************************************

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
	switch (theKey)
	{
	case 'r':
		glColor3f(1, 0, 0);
		break;

	case 'g':
		glColor3f(0, 1, 0);
		break;

	case 'b':
		glColor3f(0, 0, 1);
		break;

	case 'k':
		glColor3f(0, 0, 0);

	}
	glRecti(width - 15, height - 15, width, height); // crta mali kvadrat u boji
	glFlush();
}