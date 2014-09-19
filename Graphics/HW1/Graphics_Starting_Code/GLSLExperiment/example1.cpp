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

GLuint defaultVao;
GLuint hilbertVao;

GLuint defaultBuffer;

mat4 ortho;
GLuint projLoc;
GLuint projLocHilbert;
//GLuint sierpinskiVbo;
// Array for polyline
GLuint program;
GLuint hilbertProgram;

// Array for sierpinski gasket
point2 sierpinski[SierpinskiIterations];
vector<point2>* hilbertCurve;
polyline dragon;
polyline usa;
polyline vinci;

int hilbertIterations = 4;

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
	//generate sierpinksi's gasket
	sierpinski[0] = point2(-0.5, -0.5);
	sierpinski[1] = point2(0.0, 0.5);
	sierpinski[2] = point2(0.5, -0.5);

	sierpinski[3] = point2(0.0, 0.0);

	for(int i = 4; i < SierpinskiIterations; i++){
		int index = rand()%3;
		sierpinski[i] = (sierpinski[index]+sierpinski[i-1])/2;
	}

	//load polyline files
	loadPolyline("dragon.dat", &dragon);
	loadPolyline("usa.dat", &usa);
	loadPolyline("vinci.dat", &vinci);

	generateHilbertCurve(hilbertIterations);

}

//creates the notation for the curve at the given iteration
string generateHilbertLang(int iteration){
	string hilbert = "L";
	string L = "+RF-LFL-FR+";
	string R = "-LF+RFR+FL-"; 
	string newHilbert = "";
	for(int i = 0; i < iteration; i++){
		for(int h = 0; h < hilbert.length(); h++){
			char symbol = hilbert[h];

			if(symbol == 'L'){
				newHilbert += L;
			} else if (symbol == 'R'){
				newHilbert += R;
			} else {
				newHilbert += symbol;
			}
		}

		hilbert = newHilbert;
		newHilbert = "";
	}

	//cout << hilbert << endl;

	newHilbert = hilbert;

	hilbert = "";
	for(int i = 0; i < newHilbert.length(); i++){
		char s = newHilbert[i];
		if(s != 'R' && s != 'L'){
			hilbert += s;
		}

	}

	//cout << hilbert << endl;

	return hilbert;
}

//generates a hilbert curve at the given iteration
void generateHilbertCurve(int iteration){
	//get the notation to use as 'instructions'
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
	/*	
	for(int i = 0; i < curve->size(); i++){
		printf("(%d, %d) ", (int)curve->at(i).x, (int)curve->at(i).y);
	}
	cout << endl;*/

	/**************************/
	
	vec4 colors[] = {
		vec4(1,0.3,0.3,1),//r
		vec4(0.3,1,0.3,1),//g
		vec4(0.3,0.3,1,1),//b
		vec4(1,1,1,1),//w
	};

	int colorIndices[] = {
		0,1,2,3,
		0,1,2,3,
		3,2,1,0,
		3,2,1,0,
	};

	int colorIndexIndex = 0;

	point2* colorPoints;
	vec4* hColors;
	colorPoints = new point2[curve->size()*4];
	hColors = new vec4[curve->size()*4];
	//XYRGB
	for(int i = 0; i < curve->size(); i++){
		
		colorPoints[i*4 + 0] = curve->at(i) + point2(-0.5,  0.5);
		colorPoints[i*4 + 1] = curve->at(i) + point2(-0.5, -0.5);
		colorPoints[i*4 + 2] = curve->at(i) + point2( 0.5, -0.5);
		colorPoints[i*4 + 3] = curve->at(i) + point2( 0.5,  0.5);
		
		hColors[i*4 + 0] = colors[colorIndices[colorIndexIndex]];
		hColors[i*4 + 1] = colors[colorIndices[colorIndexIndex]];
		hColors[i*4 + 2] = colors[colorIndices[colorIndexIndex]];
		hColors[i*4 + 3] = colors[colorIndices[colorIndexIndex]];

		colorIndexIndex = (colorIndexIndex+1)%16;

	}
	cout << "alive1" << endl;

	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//create buffers
	GLuint hilbertColorProgram = InitShader("vshader2.glsl", "fshader2.glsl"); 
	glUseProgram(hilbertColorProgram);
	
	projLocHilbert = glGetUniformLocation(hilbertColorProgram, "Proj");

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//glBufferData(GL_ARRAY_BUFFER,sizeof(test), test, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, curve->size()*4*6*4, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, curve->size()*4*2*4, colorPoints);
	glBufferSubData(GL_ARRAY_BUFFER, curve->size()*4*2*4, curve->size()*4*4*4, hColors);

	GLuint vPosition = glGetAttribLocation(hilbertColorProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vColor = glGetAttribLocation(hilbertColorProgram, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(curve->size()*4*2*4));

	hilbertVao = vao;
	hilbertProgram = hilbertColorProgram;

	
	
	/***************************/
	hilbertCurve = curve;
}

//loads a polyline file and stores the data in a given struct
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
	float minX = 100;
	//cout << lines << endl;
	vector<float>* linePoints = new vector<float>();
	for(int s = 0; s < lines; s++){
		int len;
		ss >> len;
		p->segments[s] = len;
		//cout << len << endl;
		for(int l = 0; l < len; l++){

			ss >> val;
			minX = (val < minX)?val:minX;
			//cout << " " << val;
			linePoints->push_back(val);
			ss >> val;
			//cout << " " << val << endl;
			linePoints->push_back(val);
		}
	}

	cout << fileName << " minX = " << minX << endl;

	p->size = linePoints->size()*4;
	p->data = linePoints->data();
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
	//setViewport(0,15,0,15);
	glBufferData(GL_ARRAY_BUFFER, hilbertCurve->size()*8, hilbertCurve->data(), GL_STATIC_DRAW);

	defaultVao = vao;
	defaultBuffer = buffer;
}


void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
	glBindVertexArray(defaultVao);
	glBindBuffer(GL_ARRAY_BUFFER, defaultBuffer);
	glEnableVertexAttribArray( loc );
	
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	projLoc = glGetUniformLocation(program, "Proj");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        // sets white as color used to clear screen
}

//main display function that routes to the correct drawing function
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

//draw the sierpinski's gasket
void displaySierpinski(){
	glBindVertexArray(defaultVao);
	glUseProgram(program);
	glViewport(viewport.vX,viewport.vY,viewport.vWidth, viewport.vHeight);
	ortho = Ortho2D(-1,1,-1,1);
	
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);
	//glBindBuffer(GL_ARRAY_BUFFER,sierpinskiVbo);
	glDrawArrays(GL_POINTS, 0, SierpinskiIterations);
	glFlush();
}

//displays the given polyline for the struct
void displayPolyline(polyline* p){
	glBindVertexArray(defaultVao);
	glUseProgram(program);
	for(int h = 0; h < 4; h++){
		for(int v = 0; v < 4; v++){
			glViewport(viewport.vX+h*viewport.vWidth/4, viewport.vY+v*viewport.vHeight/4, viewport.vWidth/4, viewport.vHeight/4);
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

//draws the hilbert curve
void displayHilbert(){
	
	int vertices = pow((double)4, hilbertIterations);
		float size = pow((double)2, hilbertIterations);
	ortho = Ortho2D(0.5,size+0.5,0.5,size+0.5);
	glViewport(viewport.vX,viewport.vY,viewport.vWidth, viewport.vHeight);

	glBindVertexArray(hilbertVao);
	glUseProgram(hilbertProgram);
	
	glUniformMatrix4fv(projLocHilbert, 1, GL_TRUE, ortho);
	glDrawArrays(GL_QUADS, 0, vertices*4);

	glBindVertexArray(defaultVao);
	glBindBuffer(GL_ARRAY_BUFFER, defaultBuffer);
	glUseProgram(program);

	
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, ortho);
	//glBindBuffer(GL_ARRAY_BUFFER,sierpinskiVbo);
	printf("verts = %d\n", vertices);
	glDrawArrays(GL_LINE_STRIP, 0, vertices);
	glFlush();
}


//used to save the current drawing's preferred viewport
void setViewport(float l, float r, float b, float t){
	printf("setViewport(%02f, %02f, %02f, %02f)\n", l, r, b, t);
	viewport.left =l;
	viewport.right =r;
	viewport.bottom =b;
	viewport.top =t;
}

//handles key events
void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

	bool showHilbert = false;

	glBindVertexArray(defaultVao);

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
	case 'i':
		cout << "increase hilbert" << endl;
		hilbertIterations++;
		showHilbert = true;
		break;
	case 'r':
		cout << "decrease hilbert" << endl;
		if(hilbertIterations > 1){
			hilbertIterations--;
		}
		showHilbert = true;
		break;
	case 'h':
		showHilbert = true;
    	break;
	}

	if(showHilbert){
			cout << "display hilbert" << endl;
		generateHilbertCurve(hilbertIterations);
		mode = MODE_HILBERT;
		float size = pow((double)2, hilbertIterations);
		setViewport(0.5,size+0.5,0.5,size+0.5);
		
		glBindVertexArray(defaultVao);
		glBindBuffer(GL_ARRAY_BUFFER, defaultBuffer);
		glBufferData(GL_ARRAY_BUFFER, hilbertCurve->size()*8, hilbertCurve->data(), GL_STATIC_DRAW);
	}

	reshape(width, height);
	display();
}

//reshape function
//generates ands saves a 'suggested' viewport for the drawing functions
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

//main
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
