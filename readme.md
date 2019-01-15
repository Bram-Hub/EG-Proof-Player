# EG Proof Player
## Authors
2003:
James Felicito
Matthew Zuckerman

## About

## System Requirements

Microsoft Windows – any version
Open GL support – the file glut32.dll must be present in your windows/system/ directory or in your winnt/system directory – depending on your operating system.


The Application:

Launch the Application by double clicking on the executable.

The Screen is divided into three areas:

Console Area:
	The area in the upper left of the screen in which text commands can be entered.
	Valid Commands are:
		> quit
		> load xxxx – xxxx is the filename containing the proof.
				See below for proof file details
		> show xxxx – xxxx is a FOL statement
				Shows the Graph Equivalent for the FOL statement entered
				See below for FOL details
		> fullscreen	Toggles between full screen and window mode

Display Area:
	The large main area in the lower left
	Shows proofs and Existential Graphs

Interface Area:
	Contains Buttons to execute commands in the program
	Buttons Include:
		Quit – Quits the program
		Help – Displays the help window
		Load – Load a proof from the filename entered in the window that appears
		Clear – Clear the screen and the proof in memory
		Next – Display the next step of the proof
		Back – Display the previous step of the proof
		Start – Goes to the first Step in the Proof
		About – Displays a window with information about the program
* Next, Back and Start only appear if a proof has been loaded



The Proof File:

The Proof file is a text file.  The *.egf extention is used to limit confusion, but any extention will work.

The file format is as follows(the *’s are not in the file)

*********************************
StartEGProof
(Initial Graph)
(Step Name)
(Next Graph)
.
.
.
(Step Name)
(Final Graph)
***********************************

The first line (StartEGProof) is required – this denotes that this is a proof player file
After that a line representing the initial state of the graph is present
Next alternating lines of step descriptions and the corresponding graphs are in the file

Graph Writing Rules:

Writing a Graph as a line of text is not easy, but here goes:

Each line starts with the literals on the main level of the graph.  Each literal is terminated with a vertical line i.e a| or cold|.  The vertical line will not appear on the graph, but is needed.  After this, all the cuts on the main level are shown.  Each cut starts with ( and ends with ).  Inside each cut, the rules are the same as for the mail level.

NOTEs:  There should be no blank spaces in the graph lines.
	   For each level of the graph literals terminated by a | first, then cuts

Examples

P|(Q|)
Summer|(Precip|(Snow|))
A|B|(C|(D|E|))(F|G|)
()    this is an empty cut

Entering FOL Statements:

When using the show command a FOL statement is then entered using the following semantics:

~  is the negation operator
& is the conjunction operator
v is the disjunction operator
-> is the conditional operator (not this is a – and a > one after the other
<-> is the bi-conditional operator (note this is a < , a – and a > one after the other

Statements should be entered using parentheses often.

Examples:

A			Just a literal
~A
A & B
A v B
A->B
A<->B
(A&B)->(C->(~D))
