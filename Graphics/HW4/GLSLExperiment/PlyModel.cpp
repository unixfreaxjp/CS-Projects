#include "PlyModel.h"
#include <sstream>
#include "textfile.h"
#include <random>

using namespace std;

PlyModel* loadPly(string fileName){
	char* fn = new char[fileName.size()+1];
	strcpy(fn, fileName.c_str());
	char* content = textFileRead(fn); //read file

	PlyModel* model = NULL;

	int vertexCount;
	int faceCount;

	vec4* vertices;
	int* indices;

	if(content != NULL){

		stringstream ss(content);
		string line;
		string dummy;
		getline(ss, line); 
		if( line == "ply"){ //check if ply format
			getline(ss, dummy);//skip line

			ss >> dummy >> dummy >> vertexCount;
			//skip 4 lines
			for(int i = 0; i < 4; i++){
				getline(ss, dummy);
			}
			ss >> dummy >> dummy >> faceCount;
						
			//create arrays
			vertices = new vec4[vertexCount];
			indices = new int[faceCount*3];

			//skip to end of header
			getline(ss, line);
			cout << line << endl;
			do {
				getline(ss, line);
				//cout << line << endl;
			} while(line != "end_header"); 
			
			float x, y, z;
			//read vertices
			for(int v = 0; v < vertexCount; v ++){
				ss >> x;
				ss >> y;
				ss >> z;
				vertices[v] = vec4(x, y, z, 1);
			}

			//each line if the index section starts with a 3
			//just read it into the dummy to skip it
			int dummy;
			//read indices
			for(int f = 0; f < faceCount; f++){
				ss >> dummy; //skip the 3
				ss >> indices[f*3 + 0];
				ss >> indices[f*3 + 1];
				ss >> indices[f*3 + 2];
			}

			vec4* vertexNormals = new vec4[vertexCount];

			vec4* geometry = new vec4[faceCount*3];
			vec4* normals = new vec4[faceCount*3];

			for(int f = 0; f < faceCount; f++){
				geometry[f*3 + 0] = vertices[indices[f*3 + 0]];
				geometry[f*3 + 1] = vertices[indices[f*3 + 1]];
				geometry[f*3 + 2] = vertices[indices[f*3 + 2]];

				vec4 normal = Angel::normalize(Angel::cross((geometry[f*3 + 1]-geometry[f*3]), (geometry[f*3 + 2]-geometry[f*3])));
				
				vertexNormals[indices[f*3 + 0]] += normal;
				vertexNormals[indices[f*3 + 1]] += normal;
				vertexNormals[indices[f*3 + 2]] += normal;
			}

			for(int f = 0; f < faceCount; f++){
				normals[f*3 + 0] = Angel::normalize(vertexNormals[indices[f*3 + 0]]);
				normals[f*3 + 1] = Angel::normalize(vertexNormals[indices[f*3 + 1]]);
				normals[f*3 + 2] = Angel::normalize(vertexNormals[indices[f*3 + 2]]);
			}


			model = new PlyModel(faceCount*3, geometry, normals);
			free(vertices);
			free(indices);
			free(vertexNormals);
		} else {
			cout << "not ply\n";
		}
		
	} else {
		cout << "content null\n";
	}
	return model;
}

PlyModel::PlyModel(int vertexCount, vec4* vertices, vec4* normals)
{
	this->vertexCount = vertexCount;
	this->vertices = vertices;
	this->normals = normals;

	this->min = vertices[0];
	this->max = vertices[0];

	for(int v = 0; v < vertexCount; v++){
		this->min.x = (vertices[v].x < this->min.x)?vertices[v].x:this->min.x;
		this->min.y = (vertices[v].y < this->min.y)?vertices[v].y:this->min.y;
		this->min.z = (vertices[v].z < this->min.z)?vertices[v].z:this->min.z;
		
		this->max.x = (vertices[v].x > this->max.x)?vertices[v].x:this->max.x;
		this->max.y = (vertices[v].y > this->max.y)?vertices[v].y:this->max.y;
		this->max.z = (vertices[v].z > this->max.z)?vertices[v].z:this->max.z;
	}	

	this->center = (min+max)/2;
	this->size = (max-min);

	float largestDim = (size.x > size.y)?size.x:size.y;
	largestDim = (largestDim > size.z)?largestDim:size.z;

	this->scaleFactor = 1.0f/largestDim;
	//printf("scale %f\n", scaleFactor);
	//printf("center (%f, %f, %f)\n", this->center.x, this->center.y, this->center.z);
	//printf("size (%f, %f, %f)\n", this->size.x, this->size.y, this->size.z);

}



PlyModel::~PlyModel(void)
{
	free(this->vertices);
	free(this->normals);
}
