/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#ifndef OAHELPER_H
#define	OAHELPER_H

#include <vector>
#include <string>
#include "oaDesignDB.h"

using namespace std;
using namespace oa;

//A class that stores needed information for pins within it. Increase efficiency
//All macros in the design will be instantialized before moving pin.
class MyMacro 
{
	public:
		oaString getMacroName();
		oaPoint getBottomestLeft();
		oaPoint getBottomestRight();
		oaPoint getTopestLeft();
		oaPoint getTopestRight();
		oaPoint getLeftestBottom();
		oaPoint getLeftestTop();
		oaPoint getRightestBottom();
		oaPoint getRightestTop();

		MyMacro(oaInst* macro, int minimumPitch);
	private:
		oaInst* _macro;
		int _minimumPitch;
		oaString _macroName;
		vector<oaInstTerm*> _pointerAllPins;
		int _pinOffset;
		int _leftestOffset, _rightestOffset, _topestOffset, _bottomestOffset;
		oaPoint _macroUpperLeft, _macroLowerRight;
		oaPoint _bottomestLeft, _bottomestRight, _topestLeft, _topestRight, 
				_leftestBottom, _leftestTop, _rightestBottom, _rightestTop;
				
		void initialize(oaInst* macro, int minimumPitch);
		void setAllPins();
		void calculateCorners();
		
};

class OAHelper
{
public:
    OAHelper();
    virtual ~OAHelper();
	static oaPoint GetTermPosition(oaTerm* term);
	static oaPoint GetUpperLeftTermPosition(oaTerm* term);
	static oaPoint GetLowerRightTermPosition(oaTerm* term);
	static oaPoint GetAbsoluteInstTermPosition(oaInstTerm* instTerm);
	static oaPoint GetUpperLeftPinPosition(oaInstTerm* instTerm);
	static oaPoint GetLowerRightPinPosition(oaInstTerm* instTerm);
	static void MovePinToPosition(oaInstTerm* instTerm, oaPoint newAbsolutePos);
	static void MovePinByOffset(oaInstTerm* instTerm, oaPoint offset);
	static int getHPWL(oaInstTerm* instTerm);
	static bool less_HPWL(oaInstTerm* term1, oaInstTerm* term2);
	static oaBox getMacroBBox(oaInstTerm* instTerm);
	static oaString getMacroName(oaInstTerm* instTerm);
	static oaString getPinName(oaInstTerm* instTerm);
	static vector<oaInstTerm*> getPinsInSameMacro(oaInstTerm* instTerm);
	static vector<oaInstTerm*> getPinsInSameMacroWithoutItself(oaInstTerm* instTerm);
	static int getPinOffset(oaInstTerm* pin);
	static oaString getRelativePosition(oaInstTerm* pin);
	static oaString getCorrectDirection(oaInstTerm* pin, int moveStep);
	static int getMahattenDistance(oaInstTerm* pin1, oaInstTerm* pin2);
	static bool isViolatePerturbation(oaPoint originalPosition, oaPoint currentPosition, int perturbation);
	static void moveOnePin(oaInstTerm* pin, int moveStep, int minimumPitch, vector<MyMacro*> allMacros, int perturbation);
    
private:

};


#endif	/* OAHELPER_H */

