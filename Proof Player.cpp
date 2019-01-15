#pragma warning(disable : 4786)

#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "LogicTree.h"
#include "EGraph.h"
#include "FileIO.h"


using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

void Cursorblink(int);
void DrawCut(int, int, GLfloat, GLfloat);
void DrawLiteral(string Lit, int);
void DrawEG(EGraph);
int DrawHelp(EGraph, int);
int FindShift();
int FindShiftHelp(EGraph, int);
void PrintConsole();
void PrintHelp();
void PrintLoad();
void PrintAbout();
void DrawInterface();
void IntHelper();
void IntHelp(GLfloat, GLfloat, string);
void Process();

bool cursor;
bool consolearea;
int counter;

Proof p;

int wid; int hgt;
int oldwid, oldhgt;
bool fullscreen = false;
bool m1toggle = false;
bool move = false;
bool loadcheck = false;
bool loadsuccess = false;
bool helpcheck = false;
bool aboutcheck = false;
bool nextclick = false;
bool backclick = false;

LogicTree LTree;
EGraph EGr;

string console;
vector<string> consolehist;

void init(void) 
{
	glClearColor (0.25, 0.25, 0.25, 0.0);
	glShadeModel (GL_FLAT);

	glutTimerFunc(50, Cursorblink, 0);						// Timer function for blinking cursor
	cursor = true;											// cursor only blinks when this is true
	counter = 0;
	consolearea = true;										// true when mouse click is in console area

	EGr.clearGraph();
	consolehist.clear();
	console.erase();
}

void display(void) {

	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	glPushMatrix();

	DrawInterface();
	PrintConsole();

	if (helpcheck)											// Prints various windows depending on button presses
		PrintHelp();

	else if (loadcheck)
		PrintLoad();

	else if (aboutcheck)
		PrintAbout();

	else
		DrawEG(EGr);

	glPopMatrix();
	glutSwapBuffers();
}

void Cursorblink(int V) {									// Recall Function for blinking cursor

	if (counter == 0)										// displays cursor when count is between 0 and 9
		cursor = true;
	else if (counter == 10)									// hides cursor when count is between 10 and 19
		cursor = false;
	counter++;
	if (counter >= 20)										// reset counter to 0 when greater then 19
		counter = 0;
	if (consolearea || loadcheck)							// recursive call when cursor is supposed to blink
		glutTimerFunc(50, Cursorblink, 0);
	glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y)
{
	if (consolearea || loadcheck) {
		if ((int)key == 8) {								// backspace is pressed when in console area or load window
			if (console.size() > 0)
				console.erase(console.end()-1);
		}
		else if ((int)key == 13)							// Return key is pressed
			Process();
		else {
			console.insert(console.end(), key);				// else enter text
		}
	}
}

void reshape (int w, int h)
{
	if (w < 1000 && h < 700) {
		w = 1000;
		h = 700;
		glutReshapeWindow(w, h);
	}
	else if (w < 1000) {
		w = 1000;
		glutReshapeWindow(w, h);
	}
	else if (h < 700) {
		h = 700;
		glutReshapeWindow(w, h);
	}

	wid = w;
	hgt = h;
	GLfloat i = 6.0;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(-(w/2.0), (w/2.0), -(h/2.0), (h/2.0), 20, -20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void special(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:													// UP key does nothing
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:													// DOWN key does nothing
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:												// Advances to next step in proof is proof is loaded
			if (loadsuccess) {
				p.Next();
				EGr = p.getGraph();
			}
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:													// Goes back one step in proof if there is one loaded
			if (loadsuccess) {
				p.Previous();
				EGr = p.getGraph();
			}			
			glutPostRedisplay();
			break;
		default:
			break;
	}
}

void mouse(int button, int state, int x, int y) {
//	cout << x << ", " << y << endl;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (x < wid-25 && x > wid-125 && y > 25 && y < 70) {								// QUIT button
			exit(0);		
		}

		else if (x < wid-25 && x > wid-125 && y > 100 && y < 145 && !helpcheck) {			// HELP button
			helpcheck = true;
			loadcheck = false;
			aboutcheck = false;
			consolearea = false;
			cursor = false;
		}

		else if (x < wid-25 && x > wid-125 && y > 175 && y < 220 && !loadcheck) {			// LOAD button
			loadcheck = true;
			helpcheck = false;
			aboutcheck = false;
			consolearea = false;
			cursor = true;
			counter = 0;
			glutTimerFunc(50, Cursorblink, 0);
			console.erase();
		}

		else if (x < wid-25 && x > wid-125 && y > 250 && y < 295) {							// CLEAR button
			loadcheck = false;
			loadsuccess = false;
			helpcheck = false;
			aboutcheck = false;
			consolearea = false;
			cursor = false;
			counter = 0;
			console.erase();
			consolehist.clear();
			EGr.clearGraph();
			p.clearProof();
		}

		else if (x < wid-25 && x > wid-125 && y > hgt-70 && y < hgt-25) {					// ABOUT button
			aboutcheck = true;
			loadcheck = false;
			helpcheck = false;
			consolearea = false;
			cursor = false;
		}

		else if (x < wid-25 && x > wid-125 && y > 325 && y < 370 && loadsuccess) {			// NEXT button
			p.Next();
			EGr = p.getGraph();
		}

		else if (x < wid-25 && x > wid-125 && y > 400 && y < 445 && loadsuccess) {			// BACK button
			p.Previous();
			EGr = p.getGraph();
		}

		else if (x < wid-25 && x > wid-125 && y > 475 && y < 520 && loadsuccess) {			// START button
			p.gotoStep(0);
			EGr = p.getGraph();
		}

		else if (x < wid-255 && x > wid-275 && y > (hgt/2)-45 && y < (hgt/2)-25 && loadcheck) {		// Load close button
			loadcheck = false;
			cursor = false;
			console.erase();
		}

		else if (x < wid-255 && x > wid-275 && y > 205 && y < 225 && (helpcheck || aboutcheck)) {	
						// Help or About close button
			helpcheck = false;
			aboutcheck = false;
			cursor = false;
			console.erase();
		}

		else if (x < wid-150 && y < 100) {													// Console Area
			if (!consolearea && !loadcheck) {
				consolearea = true;
				cursor = true;
				counter = 0;
				glutTimerFunc(50, Cursorblink, 0);
			}
		}

		else {
			consolearea = false;
			cursor = false;
		}

	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (1000, 700); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}

void DrawCut(int x, int y, GLfloat w, GLfloat h) {
	// Function draws a "Cut", or circle, given a position and dimensions

	GLfloat PI = 3.1415926535898;
	GLfloat angle;
	GLfloat radius = 25;
	GLint subdiv = 20;

	glPushMatrix();

	glTranslatef(x, y, 0.0);												// Shifts circle over to desired position

	glPushMatrix();
	glScalef(w, h, 1.0);													// Scales circle to desired dimensions
	glColor3f (1.0, 1.0, 1.0);

	glBegin(GL_LINE_LOOP);													// Draws circle
	for (int i = 0; i < subdiv; i++) {
		angle = 2.0 * PI * i / subdiv;
		glVertex3f(radius * cos(angle), radius * sin(angle), 0.0);
	}
	glEnd();

	glPopMatrix();
	glPopMatrix();
}

void DrawLiteral(string Literal, int x) {
	// Function prints a literal in the EG, given a desired position

	glPushMatrix();
	glColor3f (1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glTranslatef(-10+x, -12, 0);
	glScalef(0.25, 0.25, 1.0);
	glutStrokeCharacter(GLUT_STROKE_ROMAN, Literal[0]);
	glLineWidth(1.0);
	glPopMatrix();

}

void DrawEG(EGraph EG) {
	// Function to draw an Existential Graph (EG)

	glPushMatrix();

	if (loadsuccess) {																// Display only if there is a proof loaded

		char *temp;
		temp = new char[2];
		itoa(p.getStepnum(), temp, 10);												// Convert step number to an ASCII character
		string temp2 = temp;

		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(((wid/2)*-1)+75, ((hgt/2)*-1)+75);
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, temp2[0]);					// Display step number
		if (temp2.size() == 2)
			glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, temp2[1]);				// If proof has double digits
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, ')');
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, ' ');
		for (int i = 0; i < p.getRule().size(); i++) {								// Shows rule used to get to step
			glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, p.getRule()[i]);
		}
		
		if (p.getStepnum() == p.TotalSteps()) {										// End of proof
			string text = " - End of Proof";

			for (i = 0; i < text.size(); i++) {
				glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
			}
		}
	}

	int shift = FindShift();														// Determine how wide proof will be for centering purposes

	glTranslatef(((shift/2)*-1)-75, -50, 0);										// Center proof
	DrawHelp(EG, 0);																// Draw Proof

	glPopMatrix();
}

int DrawHelp(EGraph EG, int spacing) {
	// A Recursive Helper function for drawing Exitential Graphs

	deque<string> Ls = EG.getLiterals();
	deque<EGraph> Cs = EG.getCuts();

	bool lits = false;
	int initspace;

	spacing += 25;										// Standard spacing is 25 pixels

	if (Ls.size() != 0) {								// No literals in this cut
		lits = true;
	}
	while (Ls.size() != 0) {							// Display literals
		DrawLiteral(Ls.front(), spacing);
		Ls.pop_front();
		spacing += 50;									// Spacing between literals is double
	}

	if (lits)
		spacing -= 25;									// end of literals, de-indent

	while (Cs.size() != 0) {							// Recurse into each Cut
		initspace = spacing;
		spacing = DrawHelp(Cs.front(), initspace);		// get spacing of cut by recursing into Cut
		Cs.pop_front();

		GLfloat cutsize = (spacing-initspace)/50.0;		// Determine size of circle to be drawn
		int cutpos = (spacing+initspace)/2;
		DrawCut(cutpos, 0, cutsize, cutsize * 0.75);	// Draw circle (Cut)
		spacing += 25;
	}

	return spacing;
}

int FindShift() {
	// Function is same as DrawEG, without display commands. Purpose is to estimate width of EG before being drawn

	int spacing = 0;
	return FindShiftHelp(EGr, spacing);

}
int FindShiftHelp(EGraph EG, int spacing) {
	// A Recursive Helper function to determine width of proof before being drawn. 
	// Same as DrawHelp, without any display commands

	deque<string> Ls = EG.getLiterals();
	deque<EGraph> Cs = EG.getCuts();

	bool lits = false;
	int initspace;

	spacing += 25;										// Standard spacing is 25 pixels

	if (Ls.size() != 0) {								// No literals in this cut
		lits = true;
	}

	while (Ls.size() != 0) {
		Ls.pop_front();
		spacing += 50;									// Spacing between literals is double
	}

	if (lits)
		spacing -= 25;									// end of literals, de-indent

	while (Cs.size() != 0) {							// Recurse into each Cut
		initspace = spacing;
		spacing = FindShiftHelp(Cs.front(), initspace);	// get spacing of cut by recursing into Cut
		Cs.pop_front();

		spacing += 25;
	}

	return spacing;


}

void DrawInterface() {
	// Function draws the buttons along the right side of the screen

	GLfloat xpos, ypos;
	glPushMatrix();
	glColor3f (1.0, 1.0, 1.0);

	glBegin(GL_LINES);									// Draw Lines dividing different areas of the screen
	glVertex2f(wid/2-150, -1*hgt/2);
	glVertex2f(wid/2-150, hgt/2);
	glVertex2f(wid/2-150, hgt/2-100);
	glVertex2f(-1*wid/2, hgt/2-100);
	glEnd();

	xpos = wid/2-25;									// Draw buttons
	ypos = hgt/2-25;

	glColor3f(0.0, 0.0, 0.3);
	IntHelp(xpos, ypos, "QUIT");

	xpos = wid/2-25;
	ypos = hgt/2-100;

	glColor3f(0.0, 0.0, 0.3);
	IntHelp(xpos, ypos, "HELP");

	xpos = wid/2-25;
	ypos = hgt/2-175;

	glColor3f(0.0, 0.0, 0.3);
	IntHelp(xpos, ypos, "LOAD");

	xpos = wid/2-25;
	ypos = hgt/2-250;

	glColor3f(0.0, 0.0, 0.3);
	IntHelp(xpos, ypos, "CLEAR");

	if (loadsuccess) {									// These buttons are only displayed if a proof is loaded

		xpos = wid/2-25;
		ypos = hgt/2-325;

		glColor3f(0.3, 0.0, 0.0);
		IntHelp(xpos, ypos, "NEXT");

		xpos = wid/2-25;
		ypos = hgt/2-400;

		glColor3f(0.3, 0.0, 0.0);
		IntHelp(xpos, ypos, "BACK");

		xpos = wid/2-25;
		ypos = hgt/2-475;

		glColor3f(0.3, 0.0, 0.0);
		IntHelp(xpos, ypos, "START");
	}

	xpos = wid/2-25;
	ypos = ((hgt/2)*-1)+70;

	glColor3f(0.25, 0.25, 0.25);
	IntHelp(xpos, ypos, "ABOUT");

	glPopMatrix();
}

void IntHelp(GLfloat xpos, GLfloat ypos, string text) {
	// A Helper function to draw each button, given a position and text for the button

	glBegin(GL_POLYGON);
	glVertex2f(xpos, ypos);
	glVertex2f(xpos, ypos-45);
	glVertex2f(xpos-100, ypos-45);
	glVertex2f(xpos-100, ypos);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(xpos, ypos);
	glVertex2f(xpos, ypos-45);
	glVertex2f(xpos-100, ypos-45);
	glVertex2f(xpos-100, ypos);
	glEnd();

	if (text.size() == 5)
		glRasterPos2f (xpos-90, ypos-30);
	else 
		glRasterPos2f (xpos-80, ypos-30);

	for (int i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void PrintConsole() {
	// Function prints out the text in the console area. Console history displays the previous 3 lines, as well as the current text

	int i;
	vector<string>::iterator it;

	glPushMatrix();

	glRasterPos2f (wid/2*-1+40, hgt/2-20);													// Console history
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');

	if (consolehist.size() > 2) {
		it = consolehist.end();
		it-= 3;
		for (i = 0; i < it->size(); i++) {
			glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , (*it)[i]);
		
		}
	}

	glRasterPos2f (wid/2*-1+40, hgt/2-40);
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');

	if (consolehist.size() > 1) {
		it = consolehist.end();
		it-= 2;
		for (i = 0; i < it->size(); i++) {
			glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , (*it)[i]);
		
		}
	}

	glRasterPos2f (wid/2*-1+40, hgt/2-60);
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');

	if (consolehist.size() > 0) {
		for (i = 0; i < (consolehist.back()).size(); i++) {
			glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , (consolehist.back())[i]);
		
		}
	}

	glRasterPos2f (wid/2*-1+40, hgt/2-80);													// Current text being entered
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , '>');
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , ' ');
	if (!loadcheck) {
		for (i = 0; i < console.size(); i++) {
			glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , console[i]);
			
		}
		if (cursor && consolearea)															// Cursor
			glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , '_');
	}
	glPopMatrix();

}

void PrintHelp() {
	// HELP button is pressed
	// Brings up a window displaying the help information
	// can be closed by pressing enter, the x in the upper right, or any other button on right side

	glPushMatrix();

	glColor3f(0.3, 0.0, 0.0);
	glBegin(GL_POLYGON);																	// Displays window
	glVertex2f(((wid/2)*-1)+100, (hgt/2)-200);
	glVertex2f((wid/2)-250, (hgt/2)-200);
	glVertex2f((wid/2)-250, ((hgt/2)*-1)+40);
	glVertex2f(((wid/2)*-1)+100, ((hgt/2)*-1)+40);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);																	// Window Border
	glVertex2f(((wid/2)*-1)+100, (hgt/2)-200);
	glVertex2f((wid/2)-250, (hgt/2)-200);
	glVertex2f((wid/2)-250, ((hgt/2)*-1)+40);
	glVertex2f(((wid/2)*-1)+100, ((hgt/2)*-1)+40);
	glEnd();

	glBegin(GL_LINE_LOOP);																	// X for close window box
	glVertex2f(wid/2-255, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-225);
	glVertex2f(wid/2-255, (hgt/2)-225);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(wid/2-255, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-225);
	glVertex2f(wid/2-275, (hgt/2)-205);
	glVertex2f(wid/2-255, (hgt/2)-225);
	glEnd();

	int i = 0;
	string text = "Help";																	// Shows text for help window

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-25, (hgt/2)-250);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

	text = "The screen is divided up into three areas:";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-290);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "Console Area";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-315);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "     The upper left of the screen is where text commands can be entered. Valid commands are:";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-340);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        > quit   -   Quits the program";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-360);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        > load xxxx   -   Loads a file containing a valid proof, where xxxx is the filename";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-380);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        > show xxxx   -   Displays an Existential Graph, where xxxx is a valid Fitch statement";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-400);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "Display Area";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-425);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "     In the lower left is the display area, where all the graphs are shown";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-450);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "Interface Area";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-475);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "There are 8 buttons along the right side of the screen. They are:";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-495);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        QUIT  -  Quits the program";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-515);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        HELP  -  Displays the help window";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-535);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        LOAD  -  Loads the proof from the filename entered";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-555);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        CLEAR  -  Clears the display and console areas";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-575);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        NEXT  -  Only available when there is a proof loaded. Advances to the next step in the proof";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-595);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        BACK  -  Only available when there is a proof loaded. Goes back one step in the proof";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-615);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "        START  -  Only available when there is a proof loaded. Goes to the first step in the proof";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-250, (hgt/2)-635);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	glPopMatrix();
}

void PrintLoad() {
	// LOAD button is pressed
	// Brings up a window to enter the filename of proof to load
	// can be closed by pressing enter, the x in the upper right, or any other button on right side

	glPushMatrix();
	glColor3f(0.3, 0.0, 0.0);
	glBegin(GL_POLYGON);																// Draw Window
	glVertex2f(((wid/2)*-1)+100, 50);
	glVertex2f(wid/2-250, 50);
	glVertex2f(wid/2-250, -50);
	glVertex2f(((wid/2)*-1)+100, -50);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);																// Window Border
	glVertex2f(((wid/2)*-1)+100, 50);
	glVertex2f(wid/2-250, 50);
	glVertex2f(wid/2-250, -50);
	glVertex2f(((wid/2)*-1)+100, -50);
	glEnd();

	glBegin(GL_LINE_LOOP);																// Draws X in upper right
	glVertex2f(wid/2-255, 45);
	glVertex2f(wid/2-275, 45);
	glVertex2f(wid/2-275, 25);
	glVertex2f(wid/2-255, 25);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(wid/2-255, 45);
	glVertex2f(wid/2-275, 25);
	glVertex2f(wid/2-275, 45);
	glVertex2f(wid/2-255, 25);
	glEnd();

	string text = "Enter the filename containing the proof you'd like to load";
	int i;

	glRasterPos2f (((wid/2)*-1)+150, 25);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_9_BY_15, text[i]);		
	}

	glRasterPos2f (((wid/2)*-1)+150, -25);
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15, '>');
	glutBitmapCharacter (GLUT_BITMAP_9_BY_15, ' ');
	for (i = 0; i < console.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_9_BY_15, console[i]);		
	}

	if (cursor)
		glutBitmapCharacter (GLUT_BITMAP_9_BY_15 , '_');

	glPopMatrix();

}

void PrintAbout() {
	// ABOUT button is pressed
	// Brings up a window containing information about the program
	// can be closed by pressing enter, the x in the upper right, or any other button on right side

	glPushMatrix();

	glColor3f(0.3, 0.0, 0.0);
	glBegin(GL_POLYGON);																	// Window
	glVertex2f(((wid/2)*-1)+100, (hgt/2)-200);
	glVertex2f((wid/2)-250, (hgt/2)-200);
	glVertex2f((wid/2)-250, ((hgt/2)*-1)+100);
	glVertex2f(((wid/2)*-1)+100, ((hgt/2)*-1)+100);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);																	// Window border
	glVertex2f(((wid/2)*-1)+100, (hgt/2)-200);
	glVertex2f((wid/2)-250, (hgt/2)-200);
	glVertex2f((wid/2)-250, ((hgt/2)*-1)+100);
	glVertex2f(((wid/2)*-1)+100, ((hgt/2)*-1)+100);
	glEnd();

	glBegin(GL_LINE_LOOP);																	// X in upper right
	glVertex2f(wid/2-255, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-225);
	glVertex2f(wid/2-255, (hgt/2)-225);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(wid/2-255, (hgt/2)-205);
	glVertex2f(wid/2-275, (hgt/2)-225);
	glVertex2f(wid/2-275, (hgt/2)-205);
	glVertex2f(wid/2-255, (hgt/2)-225);
	glEnd();

	int i = 0;
	string text = "Existential Graph Proof Viewer";											// Text of window

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-150, (hgt/2)-250);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}

	text = "James Felicito";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-65, (hgt/2)-300);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "and";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-15, (hgt/2)-325);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, text[i]);
	}

	text = "Matthew Zuckerman";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-85, (hgt/2)-350);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "PHIL 4963 - Intermediate Logic";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-130, (hgt/2)-450);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "Spring 2003";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-55, (hgt/2)-480);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}

	text = "Professor Bram van Heuveln";

	glRasterPos2f (( ( (((wid/2)*-1)+100) +((wid/2)-250) )/2)-115, (hgt/2)-510);
	for (i = 0; i < text.size(); i++) {
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, text[i]);
	}


	glPopMatrix();

}

void Process() {
	// Function called when enter key is pressed while in console or Load window

	if (loadcheck) {																// In Load Window
		loadcheck = false;
		p.clearProof();
		EGr.clearGraph();
		loadsuccess = p.readProof(console);											// Read in Proof from the given file
		console.erase();
		string msg;
		if (loadsuccess == false) {													// File Invalid
			msg = "Error loading designated file";
			consolehist.push_back(msg);
			EGr.clearGraph();
			p.clearProof();
		}
		else {																		// Load Proof and display first step
			msg = "File loaded";
			consolehist.push_back(msg);
			EGr = p.getGraph();
		}
	}
	else {																			// In Console Area
		if (console == "quit")														// Quits program
			exit(0);
		else if (console == "fullscreen") {											// Toggles between fullscreen and window mode
			if (!fullscreen) {
				oldwid = wid;														// saves old window dimensions
				oldhgt = hgt;
				glutFullScreen();
				fullscreen = true;
			}
			else {
				glutReshapeWindow(oldwid, oldhgt);									// Returns to window mode
				glutPositionWindow(100, 100);
				fullscreen = false;
			}
		}
		else if (console.size() > 5 && console[0] == 'l' && console[1] == 'o' && 
				console[2] == 'a' && console[3] == 'd') {							// load command entered
			string temp = console;
			temp.erase(0,5);

			p.clearProof();
			EGr.clearGraph();
			loadsuccess = p.readProof(temp);										// same as when entered from load window, read proof

			consolehist.push_back(console);
			console.erase();
			if (loadsuccess == false) {												// File Invalid
				console = "Error loading designated file";
				EGr.clearGraph();
				p.clearProof();
			}
			else {																	// Load proof and display first step
				console = "File loaded";
				EGr = p.getGraph();
			}
		}
		else if (console.size() > 5 && console[0] == 's' && console[1] == 'h' && 
				console[2] == 'o' && console[3] == 'w') {							// show command entered
			string temp = console;
			temp.erase(0,5);

			LTree.ClearTree();														// clears the tree
			LTree.CreateTree(temp);													// converts text statement into a Logic Tree
			EGr.clearGraph();
			EGr.add(LTree);															// converts Logic Tree into an Existential Graph

		}
		consolehist.push_back(console);
		console.erase();
	}
	glutPostRedisplay();
}
