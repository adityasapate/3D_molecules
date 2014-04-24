// main.cpp
// Aditya Sapate
// 23 Octobre 2014

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>
#include <GL/glut.h>

using namespace std;

int bond_limit;
int data_limit;
typedef struct Bonds{
	string id1;
	string id2;
	int no_bond;
}Bonds;

typedef struct Elements{
string id;
char type;
GLfloat x;
GLfloat y;
GLfloat z;
}Elements;

Bonds* bonds = new Bonds[1000];
Elements* data = new Elements[1000];


/* Global Variables (Configs) */
//Init options
int selectedObject = 3;
bool drawThatAxis = 0;
bool lightEffect = 1;
GLdouble sphereRadius = 0.4;
GLdouble cylinderRadius = 0.2;
GLint resolution = 100;
GLint slices = resolution, stacks = resolution;


//Viewer options (GluLookAt)
float fovy = 60.0, aspect = 1.0, zNear = 1.0, zFar = 100.0;

//Mouse modifiers
float depth = 8;
float phi=0, theta=0;
float downX, downY;
bool leftButton = false, middleButton = false;

//colors
GLfloat oxygen[3] =		{1.0, 0.0, 0.0};	// (O - Red)
GLfloat nitrogen[3] =	{0.0, 0.0, 1.0};	// (N - Blue)
GLfloat phosphate[3] =	{1.0, 0.5, 0.0};	// (P - Orange)
GLfloat carbon[3] =		{0.5, 0.5, 0.5};	// (C - Grey)
GLfloat hydrogen[3] =	{0.0, 1.0, 0.0};	// (H - Green)
GLfloat white[3] =		{1.0, 1.0, 1.0};
GLfloat black[3] =		{1.0, 1.0, 1.0};
GLfloat col1[3] =		{1.0, 1.0, 0.0};
GLfloat col2[3] =		{1.0, 0.0, 1.0};
GLfloat col3[3] =		{0.0, 1.0, 1.0};

//atomic volume
GLdouble oxy_wt = 0.85714285714;
GLdouble nit_wt = 0.92857142857;
GLdouble phosp_wt = 1.42857142857;
GLdouble carb_wt = 1;
GLdouble hydro_wt = 0.35714285714;
GLdouble multiplicative_factor = 1;

/* Prototypes */
void liaison(GLfloat color[3], GLfloat height);
void atome(GLfloat color[3]);
void setLightColor(GLfloat light_color[3]);
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius, GLUquadricObj *quadrilc);
void drawAxis();
void reshape(int w, int h);
void mouseCallback(int button, int state, int x, int y);
void motionCallback(int x, int y);
void keyboardCallback(unsigned char ch, int x, int y);
void displayCallback(void);
void buildDisplayList();
void options_menu(int input);
void initMenu();
int cml_parser(string infile);
int find_location(string id);

GLdouble width, height;   /* window width and height */
int wd;                   /* GLUT window handle */

/* Program initialization NOT OpenGL/GLUT dependent,
 as we haven't created a GLUT window yet */
void init(void)
{
	width  = 1280.0;                 /* initial window width and height, */
	height = 800.0;                  /* within which we draw. */
}

// Called when window is resized,
// also when window is first created,
// before the first call to display().
void reshape(int w, int h)
{
	/* save new screen dimensions */
	width = (GLdouble) w;
	height = (GLdouble) h;
	
  	/* tell OpenGL to use the whole window for drawing */
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	
  	/* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
	
	aspect = width/height;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	/* perform initialization NOT OpenGL/GLUT dependent,
	 as we haven't created a GLUT window yet */
	 string temp = string(argv[1]);
	 cml_parser(temp);
	init();
	
	/* initialize GLUT, let it extract command-line 
	 GLUT options that you may provide 
	 - NOTE THE '&' BEFORE argc */
	glutInit(&argc, argv);
	
	/* specify the display to be single 
     buffered and color as RGBA values */
	
	glutInitDisplayMode(GLUT_DEPTH);
	
	/* set the initial window size */
	glutInitWindowSize((int) width, (int) height);
	
	/* create the window and store the handle to it */
	wd = glutCreateWindow("Super 3D Molecules - Funny OpenGL App by Aditya Sapate" /* title */ );
	
	/* --- register callbacks with GLUT --- */
	
	/* register function to handle window resizes */
	glutReshapeFunc(reshape);
	
	setLightColor(white);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	buildDisplayList();
	
	/* register function that draws in the window */
	glutDisplayFunc(displayCallback);
	
	// motion
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	
	// exit with [esc] keyboard button
	glutKeyboardFunc(keyboardCallback);
	
	initMenu();
	
	/* start the GLUT main loop */
	glutMainLoop();
	
	return 0;
}

void liaison(GLfloat color[3], GLfloat height)
{
	//sans lumière:
	glColor3fv(color);
	//avec lumière
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création du cylindre
	gluCylinder(myQuad, cylinderRadius, cylinderRadius, height, slices, stacks);
}

//Sphere
void atome(GLfloat color[3])
{
	//sans lumière:
	glColor3fv(color);
	//avec lumière
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création de la sphere
	gluSphere(myQuad , sphereRadius , slices , stacks);
}

void space_atom(GLfloat color[3], GLdouble radius){
	
	//sans lumière:
	glColor3fv(color);
	//avec lumière
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création de la sphere
	gluSphere(myQuad , radius , slices , stacks);
}

void setLightColor(GLfloat light_color[3])
{
	//Light Options
	GLfloat mat_specular[]={ 1.0, 1.0, 1.0, 1.0 };
	GLfloat shine[] = {100.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color );
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);
}

void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius, GLUquadricObj *quadric)
{
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;
	float ax,rx,ry,rz;
	float len = sqrt( vx*vx + vy*vy + vz*vz );
	
	glPushMatrix();
	glTranslatef( x1,y1,z1 );
	if (fabs(vz) < 0.0001)
	{
		glRotatef(90, 0,1,0);
		ax = 57.2957795*-atan( vy / vx );
		if (vx < 0)
		{
			
		}
		rx = 1;
		ry = 0;
		rz = 0;
	}
	else
	{
		ax = 57.2957795*acos( vz/ len );
		if (vz < 0.0)
			ax = -ax;
		rx = -vy*vz;
		ry = vx*vz;
		rz = 0;
	}
	glRotatef(ax, rx, ry, rz);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, radius, radius, len, slices, stacks);
	glPopMatrix();
}

void drawAxis()
{
	
	float originAxis[3] = {0,0,0};	//Origine
	float xAxis[3] = {1, 0, 0};		// L'axe des x
	float yAxis[3] = {0, 1, 0};		// L'axe des y
	float zAxis[3] = {0, 0, 1};		// L'axe des z
	
	//Temp: Désactivation de la lumière
	glDisable(GL_LIGHTING);  
	glPushMatrix ();
	glLineWidth (10.0);
	
	//x = rouge, y = vert, z = bleu
	
	glBegin (GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3fv(originAxis);
	glVertex3fv(xAxis);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3fv(originAxis);
	glVertex3fv(yAxis);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3fv(originAxis);
	glVertex3fv(zAxis);
	glEnd();
	
	glPopMatrix ();
	//Réactivation de la lumière
	glEnable(GL_LIGHTING);
}

/* Callbacks */
void mouseCallback(int button, int state, int x, int y)
{
	downX = x; downY = y;
	leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
	middleButton = ((button == GLUT_MIDDLE_BUTTON) &&  (state == GLUT_DOWN));
}

void motionCallback(int x, int y)
{
	if (leftButton) //Rotate
	{
		phi += (x-downX)/4.0;
		theta += (downY-y)/4.0;
	}
	if (middleButton) //Scale
	{
		if (depth + (downY - y)/10.0 < zFar-10 && depth + (downY - y)/10.0 > zNear+3)
			depth += (downY - y)/10.0;
	}
	downX = x;
	downY = y;
	
	glutPostRedisplay();
}

void keyboardCallback(unsigned char ch, int x, int y)
{
	switch (ch)
	{
			//Esc button will exit app
		case 27:	
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void displayCallback(void)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//gluLookAt(eyex, eyey, eyez, atx, aty, atz, upx, upy, upz);
	gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
	
	
	//Motion Options
	glTranslatef(0.0, 0.0, -depth);
	glRotatef(-theta, 1.0, 0.0, 0.0);
	glRotatef(phi, 0.0, 1.0, 0.0);
	
	//Axis x, y and z Toggle
	if (drawThatAxis)
	{
		drawAxis();
	}
	
	//Light Effect Toggle :)
	if (lightEffect) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	
	switch (selectedObject)
	{
		//Molecule 1
		case (1):
			glCallList(1);
			break;
		//Molecule 2
		case (2):
			glCallList(2);
			break;
		//Molecule 3
		case (3):
			glCallList(3);
		default:
			break;
	}
	
	glFlush(); 
}

/* Our display items to prepare */
void buildDisplayList()
{
	GLuint id;
	
	// 3 elements
	id = glGenLists( 3 );
	
	// 1st molecule (H2O)
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	// Begin drawing
	
	// molecules
	glPushMatrix();
	glTranslatef(-1, 0, 0);
	atome(oxygen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	atome(hydrogen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1, 0, 0);
	atome(oxygen);
	glPopMatrix();
	
	// links
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	glRotatef(-25, 0, 0, 1);
	glRotatef(-90, 0, 1, 0);
	liaison(white, 1.1180);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	glRotatef(25, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	liaison(white, 1.1180);
	glPopMatrix();
	
	// End drawing
	
	glPopMatrix();
	glEndList();
	
	// 2nd molecule (CH4 tethra)
	id++;
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	
	// Begin drawing
	char atomeType_space;
	GLfloat x_space;
	GLfloat y_space;
	GLfloat z_space;

	int nbElements = data_limit;

	//for every elements, push, draw, pop
	for (int i = 0; i < nbElements; i++)
	{
		atomeType_space = data[i].type;
		x_space = data[i].x;
		y_space = data[i].y;
		z_space = data[i].z;
		
		//cout << atomeType << " " << x << " "  << y << " " << z << endl ;
		//cout << nbElements;
		
		glPushMatrix();

		glTranslatef(x_space, y_space, z_space);
		GLdouble temp;
		switch (atomeType_space)
		{
		case 'O':
			temp = oxy_wt * multiplicative_factor;
			space_atom(oxygen, temp);
			break;
		case 'N':
			temp = nit_wt * multiplicative_factor;
			space_atom(nitrogen, temp);
			break;
		case 'P':
			temp = phosp_wt * multiplicative_factor;
			space_atom(phosphate, temp);
			break;
		case 'C':
			temp = carb_wt * multiplicative_factor;
			space_atom(carbon, temp);
			break;
		case 'H':
			temp = hydro_wt * multiplicative_factor;
			space_atom(hydrogen, temp);
			break;
		default:
			break;
		}
		glPopMatrix();
	}
	// End drawing
	
	glPopMatrix();
	glEndList();
	
	// 3rd molecule
	id++;
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	
	// Begin drawing
	
	char atomeType;
	GLfloat x;
	GLfloat y;
	GLfloat z;

	//for every elements, push, draw, pop
	for (int i = 0; i < nbElements; i++)
	{
		atomeType = data[i].type;
		x = data[i].x;
		y = data[i].y;
		z = data[i].z;
		
		//cout << atomeType << " " << x << " "  << y << " " << z << endl ;
		//cout << nbElements;
		
		glPushMatrix();

		glTranslatef(x, y, z);
		switch (atomeType)
		{
		case 'O':
			atome(oxygen);
			break;
		case 'N':
			atome(nitrogen);
			break;
		case 'P':
			atome(phosphate);
			break;
		case 'C':
			atome(carbon);
			break;
		case 'H':
			atome(hydrogen);
			break;
		default:
			break;
		}
		glPopMatrix();
	}
	
	GLfloat tempx1 = 0.0, tempx2 = 0.0, tempy1 = 0.0, tempy2 = 0.0, tempz1 = 0.0, tempz2 = 0.0; 
	
	nbElements = bond_limit;
	
	for (int i = 0; i < nbElements; i++)
	{
		int loc1 = find_location(bonds[i].id1);
		int loc2 = find_location(bonds[i].id2);
		
		tempx1 = data[loc1].x;
		tempy1 = data[loc1].y;
		tempz1 = data[loc1].z;
		
		tempx2 = data[loc2].x;
		tempy2 = data[loc2].y;
		tempz2 = data[loc2].z;
		
		cout << bonds[i].no_bond << endl;
		if(bonds[i].no_bond == 1){
			setLightColor(col1);
		}else if(bonds[i].no_bond == 2){
			setLightColor(col2);
		}else if(bonds[i].no_bond == 3){
			setLightColor(col3);
		}
	
		renderCylinder(tempx1, tempy1, tempz1, tempx2, tempy2, tempz2, cylinderRadius, myQuad);
	}
	glPopMatrix();
	glEndList();
	
	glutSwapBuffers();
}

void options_menu(int input)
{
	if (input == 1 || input == 2 || input == 3)
	{
		selectedObject = input;
	}
	else if (input == 4)
	{
		if (lightEffect == 1)
		{
			lightEffect = 0;
		}
		else
		{
			lightEffect = 1;
		}
	}
	else if (input == 5)
	{
		if (drawThatAxis == 1)
		{
			drawThatAxis = 0;
		}
		else
		{
			drawThatAxis = 1;
		}
	}
	glutPostRedisplay();	
}

void initMenu()
{
	//Création des menus et appel des callback
	glutCreateMenu(options_menu);
	glutAddMenuEntry("Molecule H2O", 1);
	glutAddMenuEntry("Space Filling Model", 2);	
	glutAddMenuEntry("Ball and Stick Model~", 3);
	glutAddMenuEntry("Toggle LightEffect", 4);
	glutAddMenuEntry("Toggle Axis", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int cml_parser(string infile)
{
    string line;
    ifstream in(infile.c_str());

	int iter = 0 , iter2 = 0;
	int check = 0, check2 = 0;
	bool flag = false, flag2 = false;
    bool begin_tag = false, check_tag = false;
    bool begin_tag2 = false, check_tag2 = false;
    while (getline(in,line))
    {
        std::string tmp; // strip whitespaces from the beginning
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ' ' && tmp.size() == 0)
            {
            }
            else
            {
                tmp += line[i];
            }
        }

        //cout << "-->" << tmp << "<--" << endl;

        if (tmp == "<atomArray>")
        {
            //cout << "Found <Package>" << endl;
            begin_tag = true;
            continue;
        }
        else if (tmp == "</atomArray>")
        {
            begin_tag = false;
            //cout << "Found </Package>" << endl;
        }
        

        if (begin_tag)
        {
		    
			std::string delimiter = "\"";

			size_t pos = 0;
			std::string token;
			while ((pos = tmp.find(delimiter)) != std::string::npos) {
				token = tmp.substr(0, pos);
				if(flag){
					switch(check){
						case 0:
							data[iter].id = token;
							break;
							
						case 1:
							data[iter].type = token[0];
							break;
						case 2:
							data[iter].x = atof(token.c_str());
							break;
						case 3:
							data[iter].y = atof(token.c_str());
							break;
						case 4:
							data[iter].z = atof(token.c_str());
						}
						check++;
					//std::cout << token << std::endl;
					
				}
				flag = !flag;
				tmp.erase(0, pos + delimiter.length());
				
			}
			flag = false;
			check = 0;
			iter++;
			
			
			
		
        }
        
        
        
        if (tmp == "<bondArray>")
        {
            //cout << "Found <Package>" << endl;
            begin_tag2 = true;
            continue;
        }
        else if (tmp == "</bondArray>")
        {
            begin_tag2 = false;
            //cout << "Found </Package>" << endl;
        }
        

        if (begin_tag2)
        {
		    
			std::string delimiter = "\"";

			size_t pos = 0;
			std::string token;
			while ((pos = tmp.find(delimiter)) != std::string::npos) {
				token = tmp.substr(0, pos);
				if(flag2){
					
					if(check2 == 0){
						std::string split = " ";
						
						size_t sub_pos = 0;
						std::string sub_token;
						

						while ((sub_pos = token.find(split)) != std::string::npos) {
							sub_token = token.substr(0, sub_pos);
							
							bonds[iter2].id1 = sub_token;
								
							token.erase(0, sub_pos + split.length());
						}
						bonds[iter2].id2 = token;
						//cout << bonds[iter2].id1 << " " << bonds[iter2].id2 << endl;
						
						check2++;
					}else{
							
						bonds[iter2].no_bond = atoi(token.c_str());
					}
					
					
				}
				flag2 = !flag2;
				tmp.erase(0, pos + delimiter.length());
				
			}
			flag2 = false;
			iter2++;
			check2= 0;
		
    }
        
        
	}
	
	//cout << iter << " " << iter2;
	bond_limit = iter2;
	data_limit = iter;
	return 0;	
}


int find_location(string id){
	
	int capacity = data_limit;
	
	for(int i=0; i<= capacity; i++){
		if (id.compare(data[i].id) == 0)
			return i;
	}
	return -1;
	
	
}
