// Draws colored cube  

#include "Angel.h"
#include "PlyModel.h"
#include <sstream>
#include "CTM.h"
#include <random>
#include <iostream>;
#include <time.h>
#include "textfile.h"
#include "bmpread.h"
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
void color(float r, float g, float b, float a);


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;
GLuint viewMatrix;
GLuint vPosition;
GLuint vNormal;
GLuint vColor;

GLuint lightPosition;
GLuint ambient, diffuse, specular;
GLuint shininess;

GLuint wallProgram;
GLuint wPosition;
GLuint wNormal;
GLuint wTex;
GLuint wallProj;
GLuint wallModel;
GLuint wallUseTex;
GLuint wallColor;
GLuint wTexture;

CTM modelView;
mat4 proj;

int rotateDirection = 1;
float rotation = 0.0f;

PlyModel* cow;
GLuint ground;
GLuint walls;

GLuint grassTex;
GLuint stoneTex;

int lastIdle;

vec4 translation = vec4(0.0, 0.0, 0.0, 1.0);
vec4 dTranslation = vec4(0,0,0,0);

void init(void)
{	
	//init shaders
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	modelView.loc = glGetUniformLocationARB(program, "model_matrix");
	viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	vPosition = glGetAttribLocation(program, "vPosition");
	vNormal = glGetAttribLocation(program, "vNormal");
	vColor = glGetUniformLocation(program, "color");
	lightPosition = glGetUniformLocation(program, "LightPosition");
	ambient = glGetUniformLocation(program, "ambient");
	diffuse = glGetUniformLocation(program, "diffuse");
	specular = glGetUniformLocation(program, "specular");
	shininess = glGetUniformLocation(program, "shininess");

	wallProgram = InitShader("vshaderwall.glsl", "fshaderwall.glsl");
	wallProj = glGetUniformLocationARB(wallProgram, "projection_matrix");
	wallUseTex = glGetUniformLocation(wallProgram, "useTex");
	wallColor = glGetUniformLocation(wallProgram, "color");
	wPosition = glGetAttribLocation(wallProgram, "vPosition");
	wNormal = glGetAttribLocation(wallProgram, "normal");
	wTex = glGetAttribLocation(wallProgram, "vTexCoord");
	wTexture = glGetUniformLocation(wallProgram, "texture");

	glUseProgram(program);
	modelView.loadIdentity();

	//set up lighting
	glUniform4fv(lightPosition, 1, vec4(-0.5, 1.0, 1.0, 0));
	glUniform4fv(ambient, 1, vec4(0.2f, 0.2f, 0.2f, 1));
	glUniform4fv(diffuse, 1, vec4(0.6f, 0.2f, 0.2f, 1));
	glUniform4fv(specular, 1, vec4(0.9f, 0.9f, 0.9f, 1));
	glUniform1f(shininess, 50);

	color(1, 0, 0, 1);
	// sets the default color to clear screen
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void loadModels(){
	cow = loadPly("ply_files/cow.ply");
	bufferPly(cow);

	bmpread_t grassimg;
	if(!bmpread("grass.bmp", 0, &grassimg)){
		cout << "Couldn't load grass tex!\n";
		exit(1);
	}

	bmpread_t stoneimg;
	if(!bmpread("stones.bmp", 0, &stoneimg)){
		cout << "Couldn't load stone tex!\n";
		exit(1);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &grassTex);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	 glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, grassimg.width, grassimg.height, 0, GL_RGB, GL_UNSIGNED_BYTE, grassimg.rgb_data );
	bmpread_free(&grassimg);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &stoneTex);
	glBindTexture(GL_TEXTURE_2D, stoneTex);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	 glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, stoneimg.width, stoneimg.height, 0, GL_RGB, GL_UNSIGNED_BYTE, stoneimg.rgb_data );
	bmpread_free(&stoneimg);

	float groundVerts[] = {
		-1.0, -1.0f,  1.0f, 1, //vertices
		-1.0, -1.0f, -1.0f, 1,
		 1.0, -1.0f, -1.0f, 1,
		 1.0, -1.0f,  1.0f, 1,

		 0,1,0,0,//normals
		 0,1,0,0,
		 0,1,0,0,
		 0,1,0,0,

		 0,4,//tex coords
		 0,0,
		 4,0,
		 4,4,

	};

	float wallVerts[] = {
		-1.0f, -1.0f, -1.0f, 1,
		 1.0f, -1.0f, -1.0f, 1,
		 1.0f,  1.0f, -1.0f, 1,
		-1.0f,  1.0f, -1.0f, 1,
		
		-1.0f, -1.0f,  1.0f, 1,
		-1.0f, -1.0f, -1.0f, 1,
		-1.0f,  1.0f, -1.0f, 1,
		-1.0f,  1.0f,  1.0f, 1,

		0,0,1,1,//normals
		0,0,1,1,
		0,0,1,1,
		0,0,1,1,
		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
				
		0,0,
		4,0,
		4,4,
		0,4,
		0,0,
		5,0,
		5,5,
		0,5,
	};
	
	glUseProgram(wallProgram);
	glGenVertexArrays(1, &ground);
	glBindVertexArray(ground);
	GLuint groundBuf;
	glGenBuffers(1, &groundBuf);
	glBindBuffer(GL_ARRAY_BUFFER, groundBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerts), groundVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(wPosition);
	glVertexAttribPointer(wPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(wNormal);
	glVertexAttribPointer(wNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(64));
	glEnableVertexAttribArray(wTex);
	glVertexAttribPointer(wTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(128));
	
	glGenVertexArrays(1, &walls);
	glBindVertexArray(walls);
	GLuint wallBuf;
	glGenBuffers(1, &wallBuf);
	glBindBuffer(GL_ARRAY_BUFFER, wallBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVerts), wallVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(wPosition);
	glVertexAttribPointer(wPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(wNormal);
	glVertexAttribPointer(wNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(128));
	glEnableVertexAttribArray(wTex);
	glVertexAttribPointer(wTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(256));
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
	glBufferSubData(GL_ARRAY_BUFFER, vSize, vSize, model->normals);
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vSize));

}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
		
	glEnable( GL_DEPTH_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	
	glUseProgram(wallProgram);
	glUniform1i(wTexture, 0);
	glUniformMatrix4fv(wallProj, 1, GL_TRUE, proj);
	glBindVertexArray(ground);
	glUniform1i(wallUseTex, true);
	glUniform4fv(wallColor, 1,  vec4(0.6, 0.6, 0.6, 1));
	
	glDrawArrays(GL_QUADS, 0, 4);
	
	
	glBindVertexArray(walls);
	glUniform1i(wTexture, 1);
	glDrawArrays(GL_QUADS, 0, 8);

	
	glUseProgram(program);
	
	glUniformMatrix4fv( viewMatrix, 1, GL_TRUE, proj);
	glBindVertexArray(cow->vao);
	color(1,0,0,1);
	modelView.pushMatrix();
	
	modelView.translate(-cow->center + vec3(0.0f, -1.5f, 0.0f));
	modelView.scale(cow->scaleFactor*1.5f);
	modelView.rotateY(rotation*rotateDirection);
		
	
	glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);
	modelView.popMatrix();


    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
	
	glDisable( GL_DEPTH_TEST ); 
	
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	bool newPly = false;

    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'W':
		translation = vec4(0,0,0,1);
		dTranslation = vec4(0,0,0,0);
		rotation = 0.0f;
		break;
    }

	display();
}

void color(float r, float g, float b, float a){	
	glUniform4fv(ambient, 1, vec4(r, g, b, a)*0.2f);
	glUniform4fv(diffuse, 1, vec4(r, g, b, a)*0.6f);
	//glUniform4fv(wallColor,1,  vec4(r, g, b, a));
}

//resize handler
void resize(int w, int h){
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	proj = Angel::Perspective((GLfloat)60.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat)100.0);
	//proj = proj * Angel::Translate(0.0f, 0.5f, -1.0f);
	proj = proj * Angel::LookAt(vec4(1.0f, 0.5f, 2.5f, 1.0f), vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f,1.0f,0.0f,1.0f));
}

void idle(){
	int time = glutGet(GLUT_ELAPSED_TIME);
	float delta = (float)(time-lastIdle)/1000.0f;
	lastIdle = time;

	translation += dTranslation*delta;

	rotation += rotateSpeed*delta;
	
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
    glutCreateWindow( "Homework 4" );

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
