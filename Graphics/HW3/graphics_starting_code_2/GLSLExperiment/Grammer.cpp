#include "Grammer.h"
#include "textfile.h"
#include <sstream>
using namespace std;

Grammer::Grammer(char* fileName)
{
	char* content = textFileRead(fileName);
	stringstream ss(content);
	string line;
	
	do {
		getline(ss, line);
	} while (line[0] == '#');

	string dummy;
	stringstream slen(line);
	int iter;
	string start;
	map<char, char> rep;
	map<char, string> rules;
	slen >> dummy >> Grammer::len;
	ss >> dummy >> iter;
	ss >> dummy >> Grammer::rot.x >> Grammer::rot.y >> Grammer::rot.z;
	getline(ss, line);
	while(getline(ss, line)){
		stringstream ls(line);
		if(line.find("rep") == 0){
			string repStr;
			ls >> dummy >> repStr;
			if(repStr.length() == 3){
				rep[repStr[0]] = repStr[2];
			} else if(repStr.length() == 2){
				rep[repStr[0]] = '\0';
			}
		} else if (line.find("start") == 0){
			ls >> dummy >> start;
		} else {
			string from;
			string to;
			ls >> from >> to;
			rules[from[0]] = to;
		}

	}

	/*
	for(auto it = rep.begin(); it != rep.end(); ++it){
		cout << it->first << " -> " << it->second << endl;
	}
	for(auto it = rules.begin(); it != rules.end(); ++it){		
		cout << it->first << " -> " << it->second << endl;
	}
	*/

	pattern = start;
	string newPattern;
	iter = 1;
	for(int i = 0; i < iter; i++){
		newPattern = "";
		for(int c = 0; c < pattern.length(); c++){
			if(rules.find(pattern[c]) != rules.end()){
				newPattern += rules.at(pattern[c]);
			} else {
				newPattern += pattern[c];
			}
		}
		pattern = newPattern;
	}

	newPattern = "";
	for(int c = 0; c < pattern.length(); c++){
		if(rep.find(pattern[c]) != rep.end()){
			char newChar = rep.at(pattern[c]);
			if(newChar != '\0'){
				newPattern += newChar;
			}
		} else {
			newPattern += pattern[c];
		}
	}

	pattern = newPattern;

	printf("%s rot %f %f %f\n", fileName, rot.x, rot.y, rot.z);
}


Grammer::~Grammer(void)
{
}
