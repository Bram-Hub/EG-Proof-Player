#ifndef _EGRAPH_H_
#define _EGRAPH_H_

#include <iostream>
#include <deque>
#include <string>
//#include "Transform.h"

using std::cout;
using std::endl;
using std::deque;
using std::string;
using namespace std;



//Class to Define a level of an existential Graph
//Used Recursivly to create a full graph

class EGraph {
private:
	deque<string> Literals;
	deque<EGraph> Cuts;

public:
	EGraph();								//Default Constructor
	void addLiteral(string);				//Add a new literal to the graph
	void addCut(EGraph);					//Add a new cut to the graph
	void add(LogicTree);					//Add a logic tree to the graph
	bool isLiteral(string);					//Decide if the symbol recieved from the tree is a literal
	void removeDoubleCuts();				//Remove all double cuts from the graph
	deque<EGraph>& getCuts();				//Get all the cuts in the level
	deque<string>& getLiterals();			//Get all the literals in the level
	void doubleCutHelper(EGraph&);			//The helper function for remove double cuts(it does all the work
	void printGraph(int);					//Print the Graph(Hard to read, woun't matter when Jim's done
	void clearGraph();						//Clear all entries in the graph
	int litSize();							//Get the number of literals in the graph
	int cutSize();							//Get the number of cuts in the graph
	string toString();						//Return a string representation of the graph for file writing
	void operator=(EGraph e);
	void fromString(string);

};



/******************************
//Functions for EGraph Class
******************************/

//Default Constructor - Clears the private members
EGraph::EGraph()
{
	Literals.clear();
	Cuts.clear();
}


//Add's a literal to the Graph
void EGraph::addLiteral(string c)
{
	bool isthere = false;
	for(int i = 0; i < Literals.size(); i++)		//Check if the literal already exists at that level
	{
		if(Literals[i] == c)
			isthere = 1;
	}

	if (!isthere)							//If the literal is not there add it
	{
		Literals.push_back(c);
	}
}


//Add a cut to the graph
void EGraph::addCut(EGraph c)
{
	Cuts.push_back(c);
}


//Add the contents of a Logic Tree to the Graph
void EGraph::add(LogicTree l)
{
	
	string thisSymbol = l.GetSymbol();			//Get the first symbol
	if(isLiteral(thisSymbol))					//If the symbol is a literal, just add it to this level
	{
		addLiteral(thisSymbol);
//		cout << "Adding a literal" << endl;
	}
	else if(thisSymbol == "&")						//Handle the conjunction statement
	{
//		cout << "Handling an & statement" << endl;
		l.GoLeft();									//Add whatever is left and right in the tree to the current level
		add(l);
		l.GoUp();
		l.GoRight();
		add(l);
	}
	else if(thisSymbol == "v")						//Handle the disjunction
	{

//		cout << "Handling an v statement" << endl;
		EGraph topcut;								//Create the neccesary cuts for a standard disjunction
		EGraph leftcut;
		EGraph rightcut;

		l.GoLeft();									//Add what's in the left subtree to a cut
		leftcut.add(l);
		l.GoUp();
		l.GoRight();								//Add what's in the right subtree to a cut
		rightcut.add(l);

		topcut.addCut(leftcut);						//Add the two sub cuts to a higher level cut
		topcut.addCut(rightcut);
		addCut(topcut);								//Add the whole thing to the graph

	}

	else if(thisSymbol == "->")						//Handle the conditional statememt
	{
//		cout << "Handling an -> statement" << endl;
		EGraph topcut;								//Create the needed subcuts
		EGraph subcut;

		l.GoLeft();									//Add whatever is to the left to a cut
		topcut.add(l);
		l.GoUp();
		l.GoRight();								//Add whatever is to the right to a sub-cut
		subcut.add(l);

		topcut.addCut(subcut);						//Add the subcut to the first cut
		addCut(topcut);								//Add the whole thing to the graph
	}

	else if(thisSymbol == "<->")					//Handle the bi-conditional
	{

//		cout <<"Handling an <-> Statement" << endl; //Create two conditional graphs, one for each direction
		EGraph forward;								//Graph for the forward Direction
		EGraph backward;							//Graph for the backward Direction

		l.GoLeft();									
		forward.add(l);								//Add the left side of the tree to the forward proof
		EGraph forCut;
		forCut.add(l);
		backward.addCut(forCut);					//Add the left side of the tree to a subcut for the backward proof

		l.GoUp();
		l.GoRight();

		backward.add(l);							//Add the right side of the tree to the backward proof
		EGraph backCut;
		backCut.add(l);								//Add the right side of the tree to a subcut for the forward proof
		forward.addCut(backCut);

		addCut(forward);							//Add both proofs the current level
		addCut(backward);
	}

	else if(thisSymbol == "~")						//Handle a negation
	{
//		cout << "Handling a ~ statement" << endl;
		EGraph thecut;								//Create a cut

		l.GoRight();								//Add whatever is to the right
		thecut.add(l);					

		addCut(thecut);								//Add the whole thing to the graph
	}

	else
	{
//		cout << "Matt's Logic isn't so good" << endl;		//Complain about my logic
	}


	
}


//Determine if the given string is a literal or not
bool EGraph::isLiteral(string s)				
{
	if(s == "v" || s == "&" || s == "->" || s == "~" || s=="<->")
		return false;
	return true;
}


//Double cut starter function
void EGraph::removeDoubleCuts()
{
	doubleCutHelper(*this);						//Call doubleCut Helper on the top level
}




//Function that actually removes the double cuts
void EGraph::doubleCutHelper(EGraph &e)
{
	deque<EGraph> baseCuts = e.getCuts();
	deque<string> baseLits = e.getLiterals();
	
		

	for(int i=0; i < baseCuts.size(); i++)					//Check for each cut in the current level
	{
		EGraph inner = baseCuts[i];
		deque<string> l = inner.getLiterals();
		deque<EGraph> g = inner.getCuts();

		if(g.size() == 0)									//If there are no cuts, just return
		{
			return;
	
		}
		
		if(g.size() == 1)									//If there is one cut, see if there is a double cut
		{
			deque<string> innerLits = g[0].getLiterals();
			if(l.size() == 0)								//Only do it if there is nothing between the two cuts
			{

				e.clearGraph();								//Clear what is currently there

				for(int li = 0; li < baseLits.size(); li++)	//Put the base literals back
				{
					e.addLiteral(baseLits[li]);
				}
			
				for(int k=0; k<innerLits.size(); k++)		//Put the literals inside the double cut on the base level
				{
					//cout << "moving literal" << endl;
					e.addLiteral(innerLits[k]);

				}

				deque<EGraph> innerCuts = g[0].getCuts();	

				for(k = 0; k<innerCuts.size(); k++)			//Put the cuts inside the double cut on the base level
				{
					e.addCut(innerCuts[k]);
				}


			}
			else
			{
				for(int k=0; k<g.size(); k++)				//If there were inner literals
				{
					doubleCutHelper(g[k]);
				}
			}
		}

		else												//Handle the rest of this crap
		{
			for(int q = 0; q < g.size(); q++)
			{
				doubleCutHelper(g[q]);
			}
		}
		


	}
}


//Return the cuts of the graph
deque<EGraph>& EGraph::getCuts()
{
	return Cuts;
}


//Return the literals in the graph
deque<string>& EGraph::getLiterals()
{
	return Literals;
}


//Cheesy method of printing the graphs
void EGraph::printGraph(int n)
{
	for(int i=0; i < Literals.size(); i++)
	{
		for(int j=0; j < n; j++)
		{
			cout << "*";
		}
		cout << Literals[i] << endl;
		
	}
	n++;
	for(i=0; i < Cuts.size(); i++)
	{
		Cuts[i].printGraph(n);
	}
	
}


//Clear the graph - needed to implement the double cuts without incedent
void EGraph::clearGraph()
{
	Literals.clear();
	Cuts.clear();
}

//Get the number of literals
int EGraph::litSize()
{
	return Literals.size();
}


//Get the number of cuts
int EGraph::cutSize()
{
	return Cuts.size();
}



string EGraph::toString() {
	// Function converts existential graph into a text string. Used when writing to a file
	string s;
	
	deque<string>::iterator it = Literals.begin();

	for(int i = 0; i < Literals.size(); i++)
	{
		s += Literals[i];
		s += "|";

	}

	for(int j = 0; j < Cuts.size(); j++)
	{
		EGraph thisCut = Cuts[j];
		string t = thisCut.toString();
		s += "(";
		s += t;
		s += ")";
	}

	return s;
}


void EGraph::fromString(string s) {
	// Function converts textual string into an existential graph. Used when reading from a file

	int n = 0;
	while(n < s.size() && s[n] != '(')
	{
		string t;


		while(s[n] != '|')
		{
			t += s[n];
			n++;
		}

		addLiteral(t);
		n++;
	}

	if(n >= s.size())
	{
		return;
	}

	else
	{
		n++;

		while(n < s.size())
		{
			int lower = n;
			int ok = 0;
			int nopen = 1;
			string send;
			while(ok == 0)
			{
				if(s[n] == '(')
					nopen++;
				if(s[n] == ')')
					nopen--;

			
				if(nopen > 0)
				{
					ok = 0;
					send += s[n];
				}
				else
					ok = 1;
				n++;
			}

			EGraph newone;
			newone.fromString(send);
			addCut(newone);

			n ++;
		}
	}
}



void EGraph::operator=(EGraph e)
{
	Literals = e.getLiterals();
	Cuts = e.getCuts();
}



#endif