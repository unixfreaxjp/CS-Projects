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
#include "Turtle.h"
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
void drawIFS(Grammer* g, vec4 loc);
void color(float r, float g, float b, float a);
void color(vec4 c);
void drawSegmentLZ();
void addtree(char c);
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct treeInstance {
	float x;
	float y;
	float rz;
	Angel::vec4 color;
	char grammer;
};

vec4 COLOR[] = {
	vec4(1.0, 0.0, 0.0, 1.0),//red
	vec4(0.0, 1.0, 0.0, 1.0),//green
	vec4(0.0, 0.0, 1.0, 1.0),//blue
	vec4(1.0, 1.0, 0.0, 1.0),//yellow
	vec4(1.0, 0.0, 1.0, 1.0),//magenta
	vec4(0.0, 1.0, 1.0, 1.0),//cyan
	vec4(1.0, 0.5, 0.0, 1.0),//orange
	vec4(0.5, 0.5, 0.5, 1.0),//grey
};

map <char, Grammer*> trees;

// handle to program
GLuint program;
GLuint modelTransform;
GLuint modelMatrix;
GLuint viewMatrix;
GLuint vPosition;
GLuint vColor;
GLuint vUseColor;
GLuint uColor;

CTM modelView;
mat4 proj;
bool useColor = false;
float shear = 0.0f;
float twist = 0.0f;

vector<treeInstance> treeVec;

PlyModel* plane;
PlyModel* sphere;
PlyModel* cylinder;

PlyModel* car;

int lastIdle;
float cameraAngle = 0;

void init(void)
{	
	//init shaders
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	modelTransform = glGetUniformLocationARB(program, "model_transform");
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");
	vUseColor = glGetUniformLocationARB(program, "useColor");
	uColor = glGetUniformLocationARB(program, "color");
	// sets the default color to clear screen

	modelView.loc = modelMatrix;
		addtree('a');
		addtree('c');
		addtree('b');

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

}

void loadModels(){
	plane = loadPly("ply_files/plane.ply");
	sphere = loadPly("ply_files/sphere.ply");
	cylinder = loadPly("ply_files/cylinder.ply");
	car = loadPly("ply_files/big_porsche.ply");

	plane->modelTransform = Angel::Scale(100)*plane->modelTransform;
	sphere->modelTransform = Angel::Scale(0.1f)*sphere->modelTransform;
	cylinder->modelTransform =  Angel::Translate(0,0,0.5f)*Angel::Scale(0.7f*0.1f, 0.7f*0.1f, 0.5f)*cylinder->modelTransform;
	
	car->modelTransform = Angel::Translate(30,100,0)*Angel::RotateX(90)*Angel::Scale(4) * car->modelTransform;

	bufferPly(plane);
	bufferPly(sphere);
	bufferPly(cylinder);
	bufferPly(car);
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
	glUniformMatrix4fv( modelTransform, 1, GL_FALSE, Angel::transpose(Angel::identity()));
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelView.model);
	glUniformMatrix4fv( viewMatrix, 1, GL_TRUE, proj);
	glUniform1i(vUseColor, useColor);
	
	color(0.0,0.7,0.0,1.0);
	drawPly(plane);

	color(0.3,0.7,0.3,1);

	for(int i = 0; i < treeVec.size(); i++){
		treeInstance ti = treeVec.at(i);
		color(ti.color);
		modelView.pushMatrix();
		modelView.rotateX(ti.rz);
		drawIFS(trees[ti.grammer], vec4(ti.x, ti.y, 0,1));
		modelView.popMatrix();
	}

	color(0.5f, 0.3f, 0.8f, 1.0f);
	drawPly(car);

	glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
}


void drawPly(PlyModel* model){
	glBindVertexArray(model->vao);

	glUniformMatrix4fv(modelTransform, 1, GL_FALSE, Angel::transpose(model->modelTransform));

	glEnable( GL_DEPTH_TEST );
	modelView.pushMatrix();
	//modelView.translate(-model->center);
	//modelView.scale(model->scaleFactor);
	//glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.model) );
	
	
	glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
	modelView.popMatrix();

	glDisable( GL_DEPTH_TEST );

	glUniformMatrix4fv(modelTransform, 1, GL_FALSE, Angel::transpose(Angel::identity()));

}

void drawSegmentLZ(){
	vec4 loc = modelView.model*vec4(0,0,0,1);
}

void color(float r, float g, float b, float a){
	color(vec4(r,g,b,a));
}

void color(vec4 c){
	
	glUniform4fv(uColor, 1, c);
}

void drawIFS(Grammer* g, vec4 loc){	
	Turtle t;
	for(int i = 0; i < g->pattern.length(); i++){
		char c = g->pattern[i];
		switch(c){
		case 'F':
			modelView.pushMatrix();
			
			modelView.model =  t.rot* modelView.model;
			
			modelView.translate(t.loc + loc);
			drawPly(sphere);
			drawPly(cylinder);

			modelView.popMatrix();
			t.forward();
			break;
		case 'f':
			t.forward();
			break;
		case '+':
			t.rot = Angel::RotateX(g->rot.x) * t.rot;
			break;
		case '-':
			t.rot = Angel::RotateX(-g->rot.x) * t.rot;
			break;
		case '&':
			t.rot = Angel::RotateY(g->rot.y) * t.rot;
			break;
		case '^':
			t.rot = Angel::RotateY(-g->rot.y) * t.rot;
			break;
		case '\\':
			t.rot = Angel::RotateZ(g->rot.z) * t.rot;
			break;
		case '/':
			t.rot = Angel::RotateZ(-g->rot.z) * t.rot;
			break;
		case '|':
			t.rot = Angel::RotateX(180) * t.rot;
			break;
		case '[':
			t.push();
			break;
		case ']':
			t.pop();
			break;
		}
	}

}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{

	addtree(key);
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'f':
		treeVec.clear();
		addtree('a');
		addtree('c');
		addtree('b');
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
	case '8':
		proj = proj* Angel::RotateY(5);
		break;
	case '2':
		proj = proj* Angel::RotateY(-5);
		break;	
	case '1':
		proj = proj* Angel::RotateX(5);
		break;
	case '3':
		proj = proj* Angel::RotateX(-5);
		break;	
	case '4':
		proj = proj* Angel::RotateZ(5);
		break;
	case '6':
		proj = proj* Angel::RotateZ(-5);
		break;
	case '5':
		resize(width, height);
		break;



    }

	display();
}

void addtree(char key){
		if(trees.find(key) != trees.end()){
		treeInstance tree;
		tree.x = ((float)rand()/(float)(RAND_MAX))*200 - 100;
		tree.y = ((float)rand()/(float)(RAND_MAX))*200 - 100;
		tree.rz = ((float)rand()/(float)(RAND_MAX))*360.0f;
		tree.color = COLOR[rand()%8];
		tree.grammer = key;	
		cout << tree.rz << endl;
		treeVec.push_back(tree);
	}	
}

//resize handler
void resize(int w, int h){
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	proj = Angel::Perspective((GLfloat)70.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)100.0);
	//proj = proj * Angel::Translate(0.0f, 0.5f, -1.0f);
	proj = proj * Angel::LookAt(vec4(100.0f, 200.0f, 100, 1.0f), vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f,0.0f,1.0f,1.0f));
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
	
	//cout << a->pattern << endl;

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
