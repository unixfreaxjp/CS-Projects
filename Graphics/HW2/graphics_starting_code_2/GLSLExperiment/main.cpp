// Draws colored cube  

#include "Angel.h"
#include "PlyModel.h"
#include <sstream>
#include "CTM.h"
#include <random>
#include <time.h>
//----------------------------------------------------------------------------

using namespace std;

int width = 0;
int height = 0;
const float speed = 0.05f;
const float rotateSpeed = 45.f;
const int fileCount = 43;
const string file[] = {
	"ply_files/airplane.ply",
	"ply_files/ant.ply",
	"ply_files/apple.ply",
	"ply_files/balance.ply",
	"ply_files/beethoven.ply",
	"ply_files/big_atc.ply",
	"ply_files/big_dodge.ply",
	"ply_files/big_porsche.ply",
	"ply_files/big_spider.ply",
	"ply_files/canstick.ply",
	"ply_files/chopper.ply",
	"ply_files/cow.ply",
	"ply_files/dolphins.ply",
	"ply_files/egret.ply",
	"ply_files/f16.ply",
	"ply_files/footbones.ply",
	"ply_files/fracttree.ply",
	"ply_files/galleon.ply",
	"ply_files/hammerhead.ply",
	"ply_files/helix.ply",
	"ply_files/hind.ply",
	"ply_files/kerolamp.ply",
	"ply_files/ketchup.ply",
	"ply_files/mug.ply",
	"ply_files/part.ply",
	"ply_files/pickup_big.ply",
	"ply_files/pump.ply",
	"ply_files/pumpa_tb.ply",
	"ply_files/sandal.ply",
	"ply_files/saratoga.ply",
	"ply_files/scissors.ply",
	"ply_files/shark.ply",
	"ply_files/steeringweel.ply",
	"ply_files/stratocaster.ply",
	"ply_files/street_lamp.ply",
	"ply_files/teapot.ply",
	"ply_files/tennis_shoe.ply",
	"ply_files/tommygun.ply",
	"ply_files/trashcan.ply",
	"ply_files/turbine.ply",
	"ply_files/urn2.ply",
	"ply_files/walkman.ply",
	"ply_files/weathervane.ply",
};

void init(void);
void loadModels(void);
void display( void );
void keyboard( unsigned char key, int x, int y );
void bufferPly(PlyModel* model);
void resize(int w, int h);
void idle();
void nextPly(int amount);


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;
GLuint modelMatrix;
GLuint viewMatrix;
GLuint vPosition;
GLuint vColor;
GLuint vUseColor;
GLuint vShear;
GLuint vTwist;

CTM modelView;
mat4 proj;
bool useColor = true;
float shear = 0.0f;
float twist = 0.0f;

bool showRoom = false;
int rotateDirection = 1;
float rotation = 0.0f;

PlyModel* models[fileCount];
int cur;

int lastIdle;

vec4 translation = vec4(0.0, 0.0, 0.0, 1.0);
vec4 dTranslation = vec4(0,0,0,0);

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

	// sets the default color to clear screen
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

}

void loadModels(){

	for(int i = 0; i < fileCount; i++){
		models[i] = loadPly(file[i]);
	}

	cur = 0;

	bufferPly(models[cur]);
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
	
	glBindVertexArray(models[cur]->vao);

	modelView.pushMatrix();
	modelView.rotateY(rotation*-1);
	modelView.translate(translation);

	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, Angel::transpose(modelView.model) );
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, Angel::transpose(proj));
	glUniform1i(vUseColor, useColor);
	glUniform1f(vShear, shear);	
	glUniform1f(vTwist, twist);
	
	modelView.popMatrix();

	glEnable( GL_DEPTH_TEST );

	glDrawArrays(GL_TRIANGLES, 0, models[cur]->vertexCount);

	glDisable( GL_DEPTH_TEST ); 

    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	bool newPly = false;
	int oldCur = cur;

    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'N':
		nextPly(1);
		break;
	case 'P':
		nextPly(-1);
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
	case 'W':
		translation = vec4(0,0,0,1);
		dTranslation = vec4(0,0,0,0);
		break;
	case 'X':
		if(dTranslation.x > 0.0f){
			dTranslation.x = 0.0f;
		} else {
			dTranslation.x = speed;
		}
		break;

	case 'x':
		if(dTranslation.x < 0.0f){
			dTranslation.x = 0.0f;
		} else {
			dTranslation.x = -speed;
		}
		break;
	case 'Y':
		if(dTranslation.y > 0.0f){
			dTranslation.y = 0.0f;
		} else {
			dTranslation.y = speed;
		}
		break;

	case 'y':
		if(dTranslation.y < 0.0f){
			dTranslation.y = 0.0f;
		} else {
			dTranslation.y = -speed;
		}
		break;
	case 'Z':
		if(dTranslation.z > 0.0f){
			dTranslation.z = 0.0f;
		} else {
			dTranslation.z = speed;
		}
		break;

	case 'z':
		if(dTranslation.z < 0.0f){
			dTranslation.z = 0.0f;
		} else {
			dTranslation.z = -speed;
		}
		break;
	case 'R':
		showRoom = !showRoom;
		dTranslation = vec4(0,0,0,0);
		break;

    }

	display();
}

void nextPly(int amount){
	int oldCur = cur;
	cur += amount;
	if(cur < 0)cur = fileCount-1;
	cur %= fileCount;
	glDeleteVertexArrays(1, &models[oldCur]->vao);
	bufferPly(models[cur]);
	modelView.loadIdentity();
	modelView.translate(-models[cur]->center);
	modelView.scale(models[cur]->scaleFactor);
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

	translation += dTranslation*delta;

	if(showRoom){
		rotation += rotateSpeed*delta;

		if(rotation >= 360.0f){
			rotation = 0;			
			rotateDirection *= -1;
			nextPly(1);
		}
	}

	glutPostRedisplay();
	
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
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
	modelView.loadIdentity();
	modelView.translate(-models[cur]->center);
	modelView.scale(models[cur]->scaleFactor);

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutReshapeFunc(resize);
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
