#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "EGraph.h"
#include <iostream>
#include <deque>
#include <string>


using namespace std;


EGraph transform(EGraph inGraph);			//Transform function for EGATP
EGraph paste(string mainLit, deque<string> inLits, deque<EGraph> inCuts);		//Paste for EGATP


//Function to transform
EGraph transform(EGraph inGraph)
{
	if(inGraph.litSize() == 0 && inGraph.cutSize() == 0)
	{
		return inGraph;
	}
	
	else
	{
		
		deque<EGraph> mainCuts = inGraph.getCuts();
		deque<string> mainLits = inGraph.getLiterals();

		if(mainLits.size() > 0)
		{
			EGraph send;

			string l = mainLits[0];
			mainLits.pop_front();

			for(int i = 0; i < mainLits.size(); i++)
			{
				send.addLiteral(mainLits[i]);
			}

			for(i = 0; i < mainCuts.size(); i++)
			{
				send.addCut(mainCuts[i]);
			}

			EGraph back = transform(send);

			EGraph final = paste(l, back.getLiterals(), back.getCuts());

			return final;
		}

		else
		{
			EGraph mainGraph;
			EGraph keptOne = mainCuts[0];
			mainCuts.pop_front();

			deque<string> keptLits = keptOne.getLiterals();
			deque<EGraph> keptCuts = keptOne.getCuts();

			for(int i = 0; i < keptLits.size(); i++)
			{
				EGraph e;
				e.addLiteral(keptLits[i]);

				for(int j = 0; j < mainCuts.size(); j++)
				{
					e.addCut(mainCuts[i]);
				}

				EGraph back1 = transform(e);
				mainGraph.addCut(back1);

			}

			for(i = 0; i < keptCuts.size(); i++)
			{
				EGraph e;
				e.addCut(keptCuts[i]);

				for(int j = 0; j < mainCuts.size(); j++)
				{
					e.addCut(mainCuts[i]);
				}

				EGraph back2 = transform(e);
				mainGraph.addCut(back2);
			}

			return mainGraph;

		}
	}

	
}


EGraph paste(string mainLit, deque<string> inLits, deque<EGraph> inCuts)
{
	EGraph out;

	if(inLits.size() == 0 && inCuts.size() == 0)
	{
		out.addLiteral(mainLit);
		return out;
	}

	if(inLits.size() > 0 && inCuts.size() == 0)
	{
		EGraph back2;
		for(int i = 0; i < inLits.size(); i++)
		{
			back2.addLiteral(inLits[i]);
		}
		back2.addLiteral(mainLit);
		return back2;
	}

	else
	{
		EGraph top;
		for(int j = 0; j < inCuts.size(); j++)
		{
			EGraph t = inCuts[j];
			t.addLiteral(mainLit);
			top.addCut(t);
		}

		return top;

	}



	return out;
}
		

		



#endif