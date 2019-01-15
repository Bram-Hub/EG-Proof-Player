#ifndef _LOGICTREE_H_
#define _LOGICTREE_H_

#include <iostream>
#include <list>
#include <string>

using std::cout;
using std::endl;
using std::list;
using std::string;

typedef list<string> listdef;

class Node {
public:
	string symbol;
	Node *LChild, *RChild, *Parent;
	Node() { LChild = RChild = Parent = NULL; }
	Node (string Item) {
		symbol = Item;
		LChild = RChild = Parent = NULL;
	}
};

class LogicTree {
private:
	Node *root;
	list<string> parsed;
	int height;

public:
	LogicTree();
	void CreateTree(string);
	void CreateHelper(list<string>);
	bool OperatorChk(string);
	listdef SplitLeft(listdef, listdef::iterator);
	listdef SplitRight(listdef, listdef::iterator);
	int FindHeight(int, Node*);
	bool GoLeft();
	bool GoRight();
	bool GoUp();
	void GoToRoot();
	void PrintRoot();
	void PrintList(listdef);
	string GetSymbol();
	void ClearTree();
	void ClearHelp(Node *);

};

LogicTree::LogicTree() {
	parsed.clear();
	root = new Node;
	height = 0;
}

void LogicTree::CreateTree(string statement) {
	// Function creates a STL List version of the logical statement
	// Each element in the List is comprised of a symbol or operator

	int pos = 0;
	string temp;
	int temppos = 0;

	while (statement[pos] != '\0') {					// Parse through entire statement
		if (statement[pos] == '(') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back("(");
		}
		else if (statement[pos] == ')') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back(")");
		}
		else if (statement[pos] == ' ') {				// Space found - put latest symbol in list if exists
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
		}
		else if (statement[pos] == '~') {
			parsed.push_back("~");
			temp.erase();
		}
		else if (statement[pos] == '-' && statement[pos+1] == '>') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back("->");
			pos++;
		}
		else if (statement[pos] == '<' && statement[pos+1] == '-' && statement[pos+2] == '>') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back("<->");
			pos+=2;
		}
		else if (statement[pos] == '&') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back("&");
		}
		else if (statement[pos] == 'v') {
			if (temppos > 0) {
				parsed.push_back(temp);
				temp.erase();
				temppos = 0;							// reset temp symbol
			}
			parsed.push_back("v");
		}
		else {											// Non-operator found (part of symbol)
			temp += statement[pos];
			temppos++;
		}
		pos++;		
	}
	if (temppos > 0) {
		parsed.push_back(temp);
		temp.erase();
	}

//	PrintList(parsed);
	CreateHelper(parsed);
	GoToRoot();
	height = FindHeight(0, root);
}

void LogicTree::CreateHelper(list<string> listhelper) {
	// This is a recursive helper function
	// it searches the list for the first operator at depth 0
	// Depth in this case is how far nested into parenthesis we are
	// Once an operator is found, the operator is added into the tree at the local root,
	// and the list to the left is added to the left child, and the list to the right is added to the right child
	int depth = 0;
	bool found = false;
	listdef::iterator it = listhelper.begin();
	listdef::iterator newit;
	list<string> lefthalf;
	list<string> righthalf;

	if (listhelper.size() == 1) {
		root->symbol = *it;
		return;
	}

	while (it != listhelper.end()) {
		lefthalf.clear();
		righthalf.clear();

		if (*it == "(")									// Open Parenthesis found, increase depth
			depth++;
		else if (*it == ")")							// Close Parenthesis found, decrease depth
			depth--;
		else if (depth == 0 && *it == "~") {
			root->symbol = *it;
			
			newit = it;
			newit++;
			while (newit != listhelper.end()) {
				righthalf.push_back(*newit);
				newit++;
			}

			Node *newchild;
			newchild = new Node;
			newchild->Parent = root;
			root->RChild = newchild;
			GoRight();
			CreateHelper(righthalf);
			GoUp();
		}
		else {
			if (depth == 0 && OperatorChk(*it) && found == false) {		// Depth is 0 and an operator is found

				root->symbol = *it;						// set local root to the current operator

				/* NO CLUE IF THIS WORKS ATM */

				newit = listhelper.begin();

				while (newit != it) {
					lefthalf.push_back(*newit);
					newit++;
				}
				
				newit = it;
				newit++;
				while (newit != listhelper.end()) {
					righthalf.push_back(*newit);
					newit++;
				}
				
				Node *newnode1;							// Create new node and make it the local root's left child
				newnode1 = new Node;
				newnode1->Parent = root;
				root->LChild = newnode1;
				Node *newnode2;							// Create new node and make it the local root's right child
				newnode2 = new Node;
				newnode2->Parent = root;
				root->RChild = newnode2;

				GoLeft();
				CreateHelper(lefthalf);
				GoUp();
				GoRight();
				CreateHelper(righthalf);
				GoUp();
				found = true;
			}
		}
		it++;
	}

	if (!found) {							// No Operators found on level 0
		if (listhelper.front() == "(" && listhelper.back() == ")") {			// First and last entries are parenthesis
			listhelper.pop_back();			// Remove First and Last entries of list
			listhelper.pop_front();
			CreateHelper(listhelper);		// Call Helper function with updated list
		}
	}
}

bool LogicTree::OperatorChk(string tochk) {
	if (tochk == "&")
		return true;
	else if (tochk == "v")
		return true;
	else if (tochk == "->")
		return true;
	else if (tochk == "<->")
		return true;
	else 
		return false;
}

listdef LogicTree::SplitLeft(listdef thelist, listdef::iterator it) {
	listdef newlist;
	listdef::iterator newit = thelist.begin();

	while (newit != it) {
		newlist.push_back(*newit);
		newit++;
	}
	return newlist;
}

listdef LogicTree::SplitRight(listdef thelist, listdef::iterator it) {
	listdef newlist;
	listdef::iterator newit = it;
	newit++;

	while (newit != thelist.end()) {
		newlist.push_back(*newit);
		newit++;
	}
	return newlist;
}

int LogicTree::FindHeight(int curhgt, Node *curnode) {

	if (curnode->LChild == NULL && curnode->RChild == NULL)
		return curhgt;

	int t1 = 0;
	int t2 = 0;


	if (curnode->LChild != NULL)
		t1 = FindHeight(curhgt+1, curnode->LChild);
	if (curnode->RChild != NULL)
		t2 = FindHeight(curhgt+1, curnode->RChild);

	if (t1 > t2)
		return t1;
	else
		return t2;
}

bool LogicTree::GoLeft() {
	if (root->LChild != NULL) {
		root = root->LChild;
		return true;
	}
	else {
		cout << "No Left Child" << endl;
		return false;
	}
}

bool LogicTree::GoRight() {
	if (root->RChild != NULL) {
		root = root->RChild;
		return true;
	}
	else {
		cout << "No Right Child" << endl;
		return false;
	}
}

bool LogicTree::GoUp() {
	if (root->Parent != NULL) {
		root = root->Parent;
		return true;
	}
	else {
		cout << "Already at Root" << endl;
		return false;
	}
}

void LogicTree::GoToRoot() {
	while (root->Parent != NULL)
		root = root->Parent;
}

void LogicTree::PrintRoot() {
	cout << root->symbol << endl;
}

void LogicTree::PrintList(listdef thelist) {
	cout << thelist.size() << ":  ";
	listdef::iterator it;
	it = thelist.begin();
	while (it != thelist.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;
}

string LogicTree::GetSymbol() {
	return root->symbol;
}

void LogicTree::ClearTree() {
	GoToRoot();
	if (root->LChild != NULL)
		ClearHelp(root->LChild);
	if (root->RChild != NULL)
	ClearHelp(root->RChild);
	root->LChild = NULL;
	root->RChild = NULL;
	parsed.clear();
	height = 0;
	root = new Node;
}

void LogicTree::ClearHelp(Node *todel) {
	todel->Parent = NULL;
	if (todel->LChild != NULL)
		ClearHelp(todel->LChild);
	if (todel->RChild != NULL)
		ClearHelp(todel->RChild);
	todel->LChild = NULL;
	todel->RChild = NULL;
	delete todel;
}


#endif