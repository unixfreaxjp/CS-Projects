// Draws colored cube  

#include "Angel.h"
#include "PlyModel.h"
#include <sstream>
#include "CTM.h"
#include <random>
#include <iostream>;
#include <time.h>
#include "textfile.h"
#include "Grammer.h"
//----------------------------------------------------------------------------

using namespace std;

int width = 0;
int height = 0;
const float speed = 0.05f;
const float rotateSpeed = 45.f;

void init(void);
void loadModels(void);
void display( void );
void keyboard( unsigned char key, int x, int y );
void bufferPly(PlyModel* model);
void resize(int w, int h);
void idle();
void drawPly(PlyModel* model);
void drawIFS(Grammer* g);
void color(float r, float g, float b, float a);


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

map <char, Grammer*> trees;

// handle to program
GLuint program;
GLuint modelMatrix;
GLuint viewMatrix;
GLuint vPosition;
GLuint vColor;
GLuint vUseColor;
GLuint vShear;
GLuint vTwist;
GLuint uColor;

CTM modelView;
mat4 proj;
bool useColor = true;
float shear = 0.0f;
float twist = 0.0f;


PlyModel* sphere;
PlyModel* cylinder;

int lastIdle;


void init(void)
{	
	//init shaders
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");
	vUseColor = glGetUniformLocationARB(program, "useColor");
	vShear = glGetUniformLocationARB(program, "shear");
	vTwist = glGetUniformLocationARB(program, "twist");
	uColor = glGetUniformLocationARB(program, "color");
	// sets the default color to clear screen

	modelView.loc = modelMatrix;

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

}

void loadModels(){

	sphere = loadPly("ply_files/sphere.ply");
	cylinder = loadPly("ply_files/cylinder.ply");

	bufferPly(sphere);
	bufferPly(cylinder);
}

void bufferPly(PlyModel* model){
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	

	GLuint buffer;
	glGenBuffers(1, &buffer);

	model->vao = vao;
	model->vbo = buffer;
	
	glUseProgram(program);
	int vSize = model->vertexCount*sizeof(vec4);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, vSize*2, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, model->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vSize, vSize, model->colors);
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	
	//glBindVertexArray(models[cur]->vao);

	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.model) );
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, Angel::transpose(proj));
	glUniform1i(vUseColor, useColor);
	glUniform1f(vShear, shear);	
	glUniform1f(vTwist, twist);
	
	color(0.3,0.7,0.3,1);
	/*drawPly(cylinder);
	modelView.pushMatrix();
	modelView.translate(0,0,1);
	drawPly(sphere);
	modelView.popMatrix();*/
	drawIFS(trees['a']);
	glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
}

void drawPly(PlyModel* model){
	glBindVertexArray(model->vao);

	glEnable( GL_DEPTH_TEST );
	modelView.pushMatrix();
	modelView.translate(-model->center);
	modelView.scale(model->scaleFactor);
	//glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.model) );
	
	
	glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
	modelView.popMatrix();

	glDisable( GL_DEPTH_TEST );
}

void color(float r, float g, float b, float a){
	glUniform4fv(uColor, 1, vec4(r,g,b,a));
}

void drawIFS(Grammer* g){
	vec4 turtle = (0,0,0,1);
	modelView.pushMatrix();
	for(int i = 0; i < g->pattern.length(); i++){
		char c = g->pattern[i];
		switch(c){
		case 'F':
			modelView.pushMatrix();
			modelView.scale(0.5f, 0.5f, 1.0f);
			drawPly(cylinder);
			modelView.popMatrix();
			modelView.translate(0,0,g->len);
			break;
		case 'f':
			modelView.translate(0,0,g->len);
			break;
		case '+':
			turtle.x += g->rot.x;
			//modelView.rotateX(g->rot.x);
			break;
		case '-':
			turtle.x -= g->rot.y;
			//modelView.rotateX(-g->rot.x);
			break;
		case '&':
			//modelView.rotateY(g->rot.y);
			break;
		case '^':
			//modelView.rotateY(-g->rot.y);
			break;
		case '\\':
			//modelView.rotateZ(g->rot.z);
			break;
		case '/':
			//modelView.rotateZ(-g->rot.z);
			break;
		case '|':
			//modelView.rotateX(180);
			break;
		case '[':
			//modelView.pushMatrix();
			break;
		case ']':
			//modelView.popMatrix();
			break;
		}
	}

	modelView.popMatrix();
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{

    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'h':
		shear += 0.05f;
		break;
	case 'H':
		shear -= 0.05f;
		if(shear < 0.0f)shear = 0.0f;
		break;
	case 't':
		twist += 0.05f;
		break;
	case 'T':
		twist -= 0.05f;
		if(twist < 0.0f) twist = 0.0f;
		break;
	case 'c':
		useColor = !useColor;
		break;


    }

	display();
}

//resize handler
void resize(int w, int h){
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	proj = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)100.0);
	//proj = proj * Angel::Translate(0.0f, 0.5f, -1.0f);
	proj = proj * Angel::LookAt(vec4(0.0f, 0.5f, -1.0f, 1.0f), vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f,0.0f,1.0f,1.0f));
}

void idle(){
	int time = glutGet(GLUT_ELAPSED_TIME);

	float delta = (float)(time-lastIdle)/1000.0f;
	lastIdle = time;
	
	//glutPostRedisplay();
	
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	
	Grammer* a = new Grammer("IFS/lsys1.txt");
	Grammer* b = new Grammer("IFS/lsys2.txt");
	Grammer* c = new Grammer("IFS/lsys3.txt");
	Grammer* d = new Grammer("IFS/lsys4.txt");
	Grammer* e = new Grammer("IFS/lsysSier.txt");
	
	trees['a'] = a;
	trees['b'] = b;
	trees['c'] = c;
	trees['d'] = d;
	trees['e'] = e;
	
	cout << a->pattern << endl;

	srand(time(NULL));
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Homework 2" );

	// init glew
    glewInit();

	init();
    loadModels();

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutReshapeFunc(resize);
	int i = 5;
	glutIdleFunc(idle);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	lastIdle = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

    return 0;
}
