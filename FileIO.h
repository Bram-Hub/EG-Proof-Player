#ifndef _FILEIO_H_
#define _FILEIO_H_

#include<iostream>
#include<fstream>
#include<string>
#include<deque>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::deque;


class Step{
private:
	string stepType;
	EGraph stepGraph;
public:
	
	Step()
	{
		
		stepGraph.clearGraph();
	}

	Step(string t, EGraph e)
	{
		stepType = t;
		stepGraph = e;
	}
	
	string getType()
	{
		return stepType;
	}
	
	EGraph getGraph()
	{
		return stepGraph;
	}

	void printStep()
	{
		cout << stepType << endl;
		stepGraph.printGraph(0);
	}
	
};

class Proof{
private:
	deque<Step> ProofSteps;
	int nsteps;
	int currentStep;

public:
	EGraph initialGraph;
	Proof();
	bool readProof(string);
	void writeProof(string);
	void printProof();
	void clearProof();
	void gotoStep(int);
	EGraph getGraph();
	string getRule();
	void Next();
	void Previous();
	int getStepnum();
	int TotalSteps();

};


Proof::Proof()
{
	currentStep = 0;
	nsteps = 0;
	initialGraph.clearGraph();
	ProofSteps.clear();
}

bool Proof::readProof(string FileName) {
	// Function reads a proof from a file

	ifstream f;
	f.open(FileName.c_str(), std::ios::in);
	if (f == NULL) {										// Invalid file
		return false;
	}

	char *hin = new char[30];
	
	f.getline(hin, 30);
	string test;
	test.erase();
	test.append(hin);

	if(test != "StartEGProof")								// File does not contain valid header
		return false;


	char *in = new char[1000];

	f.getline(in, 300);
	string sin;
	sin.append(in);

	initialGraph.fromString(in);
	string text = "Initial Graph";

	Step thisStep(text,initialGraph);
	ProofSteps.push_back(thisStep);
	nsteps = 0;

	int typekey = 0;

	string tString;
	string eString;
	EGraph e;

	while(f.getline(in, 300))								// Each loop records the step used in proof, and the EG
	{
		tString.erase();
		tString.append(in);
		char* g = new char[1000];
		f.getline(g, 300);
		eString.erase();
		eString.append(g);
		e.clearGraph();
		e.fromString(eString);
		Step thisStep(tString, e);
		ProofSteps.push_back(thisStep);
		nsteps++;
	}

	currentStep = 0;										// reset step back to beginning

	return true;
}


void Proof::writeProof(string FileName)
{
	ofstream f;
	f.open(FileName.c_str(), std::ios::out);



	string s = initialGraph.toString();
	f << s << endl;

	for(int i = 0; i < ProofSteps.size(); i++)
	{
		string type = ProofSteps[i].getType();
		EGraph e = ProofSteps[i].getGraph();
		string graphString = e.toString();
		f << type << endl;
		f << graphString << endl;
	}
}

void Proof::printProof()
{
	initialGraph.printGraph(0);
	for(int i = 0; i < ProofSteps.size(); i++)
	{
		ProofSteps[i].printStep();
	}

}

void Proof::clearProof() {
	currentStep = 0;
	nsteps = 0;
	initialGraph.clearGraph();
	ProofSteps.clear();
}

void Proof::gotoStep(int stepnum) {
	currentStep = stepnum;
}

EGraph Proof::getGraph() {
	return ProofSteps[currentStep].getGraph();
}

string Proof::getRule() {
	return ProofSteps[currentStep].getType();
}

void Proof::Next() {
	if (currentStep != nsteps) {
		currentStep++;
	}
}
void Proof::Previous() {
	if (currentStep > 0) {
		currentStep--;
	}
}

int Proof::getStepnum() {
	return currentStep;
}

int Proof::TotalSteps() {
	return nsteps;
}




#endif