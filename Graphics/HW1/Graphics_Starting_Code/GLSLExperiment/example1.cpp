#include "Angel.h"  // Angel.h is homegrown include file, which also includes glew and freeglut
#include "textfile.h"
#include <sstream>
#include <vector>
#include "mat.h"

using namespace std;


const int MODE_SIERPINSKI = 0;
const int MODE_HILBERT = 1;
const int MODE_USA = 2;
const int MODE_DRAGON = 3;
const int MODE_VINCI = 4;

//Number of Sierpinski iterations
const int SierpinskiIterations = 50000;

//direction vectors used to hilbert curve
const vec2 directions[] = {
	vec2(1,0),
	vec2(0,1),
	vec2(-1,0),
	vec2(0,-1)
};

typedef struct polyline_struct {
	GLuint vbo;
	float left;
	float top;
	float right;
	float bottom;
	int lines;
	int* segments;
	int size;
	float* data;
} polyline;

//prototypes
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void loadPolyline(char* fileName, polyline* p);
void displaySierpinski();
void displayPolyline(polyline* p);
void reshape(int W, int H);
float* hilbert(int depth);
string generateHilbertLang(int iteration);
void generateHilbertCurve(int iteration);
void displayHilbert();
void setViewport(float l, float r, float b, float t);

typedef vec2 point2;


int mode = MODE_HILBERT;

int width;
int height;

mat4 ortho;
GLuint projLoc;

GLuint sierpinskiVbo;
// Array for polyline
GLuint program;

// Array for sierpinski gasket
point2 sierpinski[SierpinskiIterations];
vector<point2>* hilbertCurve;
polyline dragon;
polyline usa;
polyline vinci;

int hilbertIterations = 2;

struct {
	float left;
	float right;
	float top;
	float bottom;
	int vWidth;
	int vHeight;
	int vX;
	int vY;
} viewport;

void generateGeometry( void )
{
	sierpinski[0] = point2(-0.5, -0.5);
	sierpinski[1] = point2(0.0, 0.5);
	sierpinski[2] = point2(0.5, -0.5);

	sierpinski[3] = point2(0.0, 0.0);

	for(int i = 4; i < SierpinskiIterations; i++){
		int index = rand()%3;
		sierpinski[i] = (sierpinski[index]+sierpinski[i-1])/2;
	}

	loadPolyline("dragon.dat", &dragon);
	loadPolyline("usa.dat", &usa);
	loadPolyline("vinci.dat", &vinci);

	generateHilbertCurve(hilbertIterations);

}

string generateHilbertLang(int iteration){
	string hilbert = "L";
	string L = "+RF-LFL-FR+";
	string R = "-LF+RFR+FL-"; 
	string newHilbert = "";
	for(int i = 0; i < iteration+1; i++){
		for(int h = 0; h < hilbert.length(); h++){
			char symbol = hilbert[h];
			switch(symbol){
			case 'L':
				newHilbert += L;
				break;
			case 'R':
				newHilbert += R;
				break;
			default:
				newHilbert += symbol;
				break;
			}
		}
		cout << newHilbert << endl;
		hilbert = newHilbert;
	}
	cout << hilbert << endl;
	return hilbert;
}

void generateHilbertCurve(int iteration){
	string lang = generateHilbertLang(iteration);
	vector<point2>* curve = new vector<point2>();
	curve->push_back(point2(1,1));
	int dir = 0;
	for(int i = 0; i < lang.length(); i++){
		char symbol = lang[i];

		switch(symbol){
		case '+':
			dir = (dir+1)%4;
			break;
		case '-':
			dir = (dir+3)%4;
			break;
		case 'F':
			curve->push_back(curve->back() + directions[dir]);
			break;
		}
	}

	if(hilbertCurve != NULL){
		free(hilbertCurve);
	}

	for(int i = 0; i < curve->size(); i++){
		printf("(%f, %f) ", curve->at(i).x, curve->at(i).y);
	}
	cout << endl;

	hilbertCurve = curve;
}

void loadPolyline(char* fileName, polyline* p){
	char* text = textFileRead(fileName);

	if(text == NULL){
		cout << "file not found\n";
		return;
	}
	
	stringstream ss(text);
	string line;

	//skip comments
	while(getline(ss, line)){
		if(line[0] == '*'){
			break;
		}
	}

	//get maximum bounds
	ss >> p->left;
	ss >> p->top;
	ss >> p->right;
	ss >> p->bottom;

	//number of polylines
	int lines;
	ss >> lines;
	p->lines = lines;
	//read polylines
	p->segments = new int[lines];
	float val;
	//cout << lines << endl;
	vector<float>* linePoints = new vector<float>();
	for(int s = 0; s < lines; s++){
		int len;
		ss >> len;
		p->segments[s] = len;
		//cout << len << endl;
		for(int l = 0; l < len; l++){

			ss >> val;
			//cout << " " << val;
			linePoints->push_back(val);
			ss >> val;
			//cout << " " << val << endl;
			linePoints->push_back(val);
		}
	}

	p->size = linePoints->size()*4;
	p->data = linePoints->data();

	//make GPU Buffer
/*	GLuint vao;
	glGenVertexArrays(1, &vao);
	cout << "vao " << vao << endl;
	glBindVertexArray(vao);
	p->vao = vao;
	dragonVao = vao;
	sierpinskiVao = vao;
	cout << "vao2 " << p->vao << endl;
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	cout << linePoints->size() << endl;
	glBufferData(GL_ARRAY_BUFFER, linePoints->size()*4,linePoints->data(), GL_STATIC_DRAW);
	*/
//	p->buffer = buffer;
	
}


void initGPUBuffers( void )
{
	
    GLuint buffer;

	// Create a vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );

    glBindVertexArray( vao);
    glGenBuffers( 1, &buffer);
    glBindBuffer( GL_ARRAY_BUFFER, buffer);
	setViewport(0,15,0,15);
		glBufferData(GL_ARRAY_BUFFER, hilbertCurve->size()*8, hilbertCurve->data(), GL_STATIC_DRAW);

	/*glBindVertexArray( vao[1] );
    glGenBuffers( 1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, dragon.size, dragon.data, GL_STATIC_DRAW);
	
	glBindVertexArray( vao[2] );
    glGenBuffers( 1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, usa.size, usa.data, GL_STATIC_DRAW);

	glBindVertexArray( vao[3] );
    glGenBuffers( 1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vinci.size, vinci.data, GL_STATIC_DRAW);

	sierpinskiVbo = vao[0];
	dragon.vbo = vao[1];
	usa.vbo = vao[2];
	vinci.vbo = vao[3];*/

}


void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	projLoc = glGetUniformLocation(program, "Proj");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        // sets white as color used to clear screen
}


void display(){
	
	glClear(GL_COLOR_BUFFER_BIT);
	switch(mode){
	case MODE_SIERPINSKI:
		displaySierpinski();
		break;
	case MODE_DRAGON:
		displayPolyline(&dragon);
		break;
	case MODE_USA:
		displayPolyline(&usa);
		break;
	case MODE_VINCI:
		displayPolyline(&vinci);
		break;
	case MODE_HILBERT:
		displayHilbert();
		break;
	}
}

void displaySierpinski(){
	glViewport(viewport.vX,viewport.vY,viewport.vWidth, viewport.vHeight);
	ortho = Ortho2D(-1,1,-1,1);
	
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);
	//glBindBuffer(GL_ARRAY_BUFFER,sierpinskiVbo);
	glDrawArrays(GL_POINTS, 0, SierpinskiIterations);
	glFlush();
}

void displayPolyline(polyline* p){
	for(int h = 0; h < 4; h++){
		for(int v = 0; v < 4; v++){
			glViewport(viewport.vX+h*width/4, viewport.vY+v*height/4, viewport.vWidth/4, viewport.vHeight/4);
			ortho = Ortho2D(p->left, p->right, p->bottom, p->top);	
			printf("%f, %f, %f, %f\n", p->left, p->right, p->bottom, p->top);
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);

			int first = 0;
			//glBindVertexArray(p->vbo);
			for(int i = 0; i < p->lines; i++){
				glDrawArrays(GL_LINE_STRIP, first, p->segments[i]);
				first += p->segments[i];
			}
			glFlush();
		}
	}
}

void displayHilbert(){
	glViewport(viewport.vX,viewport.vY,viewport.vWidth, viewport.vHeight);
	float size = pow((double)2, hilbertIterations);
	ortho = Ortho2D(0,size,0,size);
	
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);
	//glBindBuffer(GL_ARRAY_BUFFER,sierpinskiVbo);
	int vertices = pow((double)4, hilbertIterations+1);
	cout << "vertices " << vertices << endl;
	glDrawArrays(GL_LINE_STRIP, 0, vertices);
	glFlush();
}



void setViewport(float l, float r, float b, float t){
	viewport.left =l;
	viewport.right =r;
	viewport.bottom =b;
	viewport.top =t;
}

void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
    case 033:			// 033 is Escape key octal value
        exit(1);		// quit program
        break;
	case 's':
		mode = MODE_SIERPINSKI;
		setViewport(-1,1,-1,1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sierpinski), sierpinski, GL_STATIC_DRAW);

		break;
	case 'd':
		mode = MODE_DRAGON;
		setViewport(dragon.left, dragon.right, dragon.bottom, dragon.top);
		glBufferData(GL_ARRAY_BUFFER, dragon.size, dragon.data, GL_STATIC_DRAW);
		break;
	case 'u':
		mode = MODE_USA;
		setViewport(usa.left, usa.right, usa.bottom, usa.top);
		glBufferData(GL_ARRAY_BUFFER, usa.size, usa.data, GL_STATIC_DRAW);
		break;
	case 'v':
		mode = MODE_VINCI;
		setViewport(vinci.left, vinci.right, vinci.bottom, vinci.top);
		glBufferData(GL_ARRAY_BUFFER, vinci.size, vinci.data, GL_STATIC_DRAW);
		break;
	case 'h':
		mode = MODE_HILBERT;
		float size = pow((double)2, hilbertIterations);
		setViewport(0,size+1,0,size+1);
		glBufferData(GL_ARRAY_BUFFER, hilbertCurve->size()*8, hilbertCurve->data(), GL_STATIC_DRAW);
    	break;
	}

	reshape(width, height);
	display();
}

void reshape(int W, int H){
	width = W;
	height = H;

	float R = (viewport.right - viewport.left)/(viewport.top-viewport.bottom);
	float WR = (float)W/(float)H;
	if(R > WR){
		viewport.vY = (H-W/R)/2;
		viewport.vX = 0;
		viewport.vWidth = W;
		viewport.vHeight = W/R;
	} else if (R < WR){
		viewport.vY = 0;
		viewport.vX = (W-H*R)/2;
		viewport.vWidth = H*R;
		viewport.vHeight = H;
	} else {
		viewport.vX = 0;
		viewport.vY = 0;
		viewport.vWidth = W;
		viewport.vHeight = H;
	}
}


int main( int argc, char **argv )
{
	// main function: program starts here

	width = 640;
	height = 480;

    glutInit( &argc, argv );                       // intialize GLUT  
    glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB ); // single framebuffer, colors in RGB
    glutInitWindowSize( width, height);                // Window size: 640 wide X 480 high
	glutInitWindowPosition(100,150);               // Top left corner at (100, 150)
    glutCreateWindow( "Starting App" );            // Create Window

    glewInit();										// init glew

    generateGeometry( );                           // Call function that generates points to draw
    initGPUBuffers( );							   // Create GPU buffers
    shaderSetup( );                                // Connect this .cpp file to shader file

    glutDisplayFunc( display );                    // Register display callback function
    glutKeyboardFunc( keyboard );  
	glutReshapeFunc(reshape);// Register keyboard callback function

	// Can add minimalist menus here
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop

	
	setViewport(-1,1,-1,1);

    glutMainLoop();
    return 0;
}
