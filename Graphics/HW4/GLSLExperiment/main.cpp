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
GLuint shadow;
GLuint vCube;
GLuint vMode;

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

vec4 lightPos(1,1,1,1);
mat4 lightProjX = Angel::identity();
mat4 lightProjY = Angel::identity();
mat4 lightProjZ = Angel::identity();

GLuint cubeMap;
GLuint cubeTex;

bool shadowsOn = true;
bool texturesOn = true;
int mode = 1;

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
	shadow = glGetUniformLocation(program, "shadow");
	vCube = glGetUniformLocation(program, "cube");
	vMode = glGetUniformLocation(program, "mode");

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

	//vec4 lightPos = vec4(-0.5, 1.0, -1.0, 0);
	//set up lighting
	
	glUniform4fv(lightPosition, 1, lightPos);
	glUniform4fv(ambient, 1, vec4(0.2f, 0.2f, 0.2f, 1));
	glUniform4fv(diffuse, 1, vec4(0.6f, 0.2f, 0.2f, 1));
	glUniform4fv(specular, 1, vec4(0.9f, 0.9f, 0.9f, 1));
	glUniform1f(shininess, 60);
	glUniform1i(vMode, 1);

	lightProjX[3][1] = -1.0/lightPos.x;
	lightProjY[3][1] = -1.0/lightPos.y;
	lightProjZ[3][1] = -1.0/lightPos.z;

	color(1, 0, 0, 1);
	// sets the default color to clear screen
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//draw lines
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void loadModels(){

	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &cubeTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

	bmpread_t px;
	if(!bmpread("nvposx.bmp", 0, &px)){
		cout << "couldn't read px\n";
		exit(1);
	}
	bmpread_t nx;
	if(!bmpread("nvnegx.bmp", 0, &nx)){
		
		cout << "couldn't read nx\n";
		exit(1);
	}
	bmpread_t py;
	if(!bmpread("nvposy.bmp", 0, &py)){
		cout << "couldn't read py\n";
		exit(1);
	}
	bmpread_t ny;
	if(!bmpread("nvnegy.bmp", 0, &ny)){
		cout << "couldn't read ny\n";
		exit(1);
	}
	bmpread_t pz;
	if(!bmpread("nvposz.bmp", 0, &pz)){
		cout << "couldn't read pz\n";
		exit(1);
	}
	bmpread_t nz;
	if(!bmpread("nvnegz.bmp", 0, &nz)){
		cout << "couldn't read nz\n";
		exit(1);
	}
	
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, px.width, px.height, 0, GL_RGB, GL_UNSIGNED_BYTE, px.rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, nx.width, nx.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nx.rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, py.width, py.height, 0, GL_RGB, GL_UNSIGNED_BYTE, py.rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, ny.width, ny.height, 0, GL_RGB, GL_UNSIGNED_BYTE, ny.rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, pz.width, pz.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pz.rgb_data );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, nz.width, nz.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nz.rgb_data );
	
	bmpread_free(&px);
	bmpread_free(&nx);
	bmpread_free(&py);
	bmpread_free(&ny);
	bmpread_free(&pz);
	bmpread_free(&nz);
	
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUseProgram(program);
	glUniform1i(vCube, cubeTex);

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

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &grassTex);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	 glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, grassimg.width, grassimg.height, 0, GL_RGB, GL_UNSIGNED_BYTE, grassimg.rgb_data );
	bmpread_free(&grassimg);

	glActiveTexture(GL_TEXTURE2);
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
		4,0,
		4,4,
		0,4,
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
	
	glUniform1i(wallUseTex, texturesOn);
	glUniform4fv(wallColor, 1,  vec4(0.8, 0.8, 0.8, 1));
	glUniformMatrix4fv(wallProj, 1, GL_TRUE, proj);
	
	glUniform1i(wTexture, 1);
	glBindVertexArray(ground);
	glDrawArrays(GL_QUADS, 0, 4);
	
	glUniform1i(wTexture, 2);
	glUniform4fv(wallColor, 1,  vec4(0.4, 0.4, 1.0, 1));
	glBindVertexArray(walls);
	glDrawArrays(GL_QUADS, 0, 8);
	
	glUseProgram(program);
	glUniform1i(vCube, 0);
	glUniform1i(vMode, mode);
	glUniformMatrix4fv( viewMatrix, 1, GL_TRUE, proj);
	glBindVertexArray(cow->vao);
	color(1,1,1,1);

	modelView.pushMatrix();
	
		modelView.translate(-cow->center + vec4(0,-1,0,1));
		modelView.scale(cow->scaleFactor*1.5f);
		//rotation = 0;
		modelView.rotateY(rotation*rotateDirection);
		glUniform4fv(lightPosition, 1, vec4(cos(Angel::DegreesToRadians*rotation), 1.0, sin(DegreesToRadians*rotation), 0));
		glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);

		if(shadowsOn){
			color(0,0,0,1);
			glUniform1i(shadow, true);
			
			lightPos = vec4(-1.0,-1,-1.0,0);
			mat4 lx = Angel::identity();
			mat4 ly = Angel::identity();
			mat4 lz = Angel::identity();
			lx[3][0] = -1.0f/lightPos.x;
			ly[3][1] = -1.0f/lightPos.y;
			lz[3][2] = -1.0f/lightPos.z;

			modelView.pushMatrix();
				modelView.model = Angel::Translate(lightPos)*lx*Angel::Translate(-lightPos)*modelView.model;
				modelView.scale(0,2,2);
				modelView.translate(-0.99,0,0);
				glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);
			modelView.popMatrix();

			modelView.pushMatrix();
				modelView.model = Angel::Translate(lightPos)*ly*Angel::Translate(-lightPos)*modelView.model;
				modelView.scale(2,0,2);
				modelView.translate(0,-0.99,0);
				glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);
			modelView.popMatrix();

			modelView.pushMatrix();
				modelView.model = Angel::Translate(lightPos)*lz*Angel::Translate(-lightPos)*modelView.model;
				modelView.scale(2,2,0);
				modelView.translate(0,0,-0.99);
				glDrawArrays(GL_TRIANGLES, 0, cow->vertexCount);
			modelView.popMatrix();


			glUniform1i(shadow, false);
		}

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
	int cToMode[] = {2, 2, 1, 2, 3};
	int dToMode[] = {3, 3, 3, 1, 2};
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'a':
	case 'A':
		shadowsOn = !shadowsOn;
		break;
	case 'b':
	case 'B':
		texturesOn = !texturesOn;
		break;
	case 'c':
	case 'C':
		mode = cToMode[mode];
		break;
	case 'd':
	case 'D':
		mode = dToMode[mode];
		break;
    }

	display();
}

void color(float r, float g, float b, float a){	
	glUniform4fv(ambient, 1, vec4(r, g, b, a)*0.3f);
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
    glutInitWindowSize( 600, 512 );
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
