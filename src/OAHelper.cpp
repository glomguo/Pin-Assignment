/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#include "OAHelper.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

static oaNativeNS ns;

OAHelper::OAHelper()
{
}

OAHelper::~OAHelper()
{
}

/* Function returns position of an oaTerm as an oaPoint
 * oaTerm -> oaPin -> oaPinFig -> oaBox -> getCenter()
 */
oaPoint OAHelper::GetTermPosition(oaTerm* term)
{
	oaIter<oaPin> pinIterator(term->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();
	oaBox box; oaPoint point;
	pinFig->getBBox(box);
	box.getCenter(point);
	return point;
}

oaPoint OAHelper::GetUpperLeftTermPosition(oaTerm* term)
{
	oaIter<oaPin> pinIterator(term->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();
	oaBox box; oaPoint point;
	pinFig->getBBox(box);
	box.getUpperLeft(point);
	return point;
}

oaPoint OAHelper::GetLowerRightTermPosition(oaTerm* term)
{
	oaIter<oaPin> pinIterator(term->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();
	oaBox box; oaPoint point;
	pinFig->getBBox(box);
	box.getLowerRight(point);
	return point;
}

/* Function returns ABSOLUTE position of an oaInstTerm as an oaPoint
 * The associated oaTerm is fetched, which is basically the corresponding pin inside the macro block
 * The oaTerm position is then added to the oaInst origin to get the absolute instTerm position
 * Note: We cannot simply use the oaInst origin as the oaInstTerm position(as in Labs 1 & 3).
 */
oaPoint OAHelper::GetAbsoluteInstTermPosition(oaInstTerm* instTerm)
{
	//Get relative position of associated terminal inside the instance master
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint relativePos = GetTermPosition(assocTerm);
    
	//Get the origin and orientation of the instance
	oaInst* inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	
	//Rotate relative position by orient
	oaTransform trans = oaTransform(orient);
	relativePos.transform(trans);
	
	//Compute absolute position in global design
	oaPoint absolutePos = relativePos + instOrigin;
	return absolutePos;
}

//Use to calculate pin's offset by abstructing upperLeft from center
oaPoint OAHelper::GetUpperLeftPinPosition(oaInstTerm* instTerm)
{	
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint relativePos = GetUpperLeftTermPosition(assocTerm);
	oaInst* inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	oaTransform trans = oaTransform(orient);
	relativePos.transform(trans);
	oaPoint absolutePos = relativePos + instOrigin;
	return absolutePos;
}

//Use to calculate pin's offset by abstructing center from lowerRight
oaPoint OAHelper::GetLowerRightPinPosition(oaInstTerm* instTerm)
{	
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint relativePos = GetLowerRightTermPosition(assocTerm);
	oaInst* inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	oaTransform trans = oaTransform(orient);
	relativePos.transform(trans);
	oaPoint absolutePos = relativePos + instOrigin;
	return absolutePos;
}

/* Function moves the pin (oaTerm) associated with instTerm to ABSOLUTE position newPos
 */
void OAHelper::MovePinToPosition(oaInstTerm* instTerm, oaPoint newAbsolutePos)
{
	//Get current relative position of associated term
	oaTerm* assocTerm = instTerm->getTerm();
	oaPoint currentRelativePos = GetTermPosition(assocTerm);
	
	//Get origin, orientation and inverse orientation of inst
	oaInst *inst = instTerm->getInst();
	oaPoint instOrigin;
	inst->getOrigin(instOrigin);
	oaOrient orient = inst->getOrient();
	oaOrient inverseOrient = orient.getRelativeOrient(oacR0);

	//Find new relative position and rotate it according to inverse orient
	oaPoint newRelativePos = newAbsolutePos - instOrigin;
	oaTransform trans = oaTransform(inverseOrient);
	newRelativePos.transform(trans);
	
	//Calculate offset
	oaPoint offset = newRelativePos - currentRelativePos;
	oaTransform trans2 = oaTransform(offset);
	
	//Find associated term's pinfig and apply move
	oaIter<oaPin> pinIterator(assocTerm->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();

	pinFig->move(trans2);
}

/* Function moves the pin (oaTerm) associated with instTerm by the offset oaPoint
 */
void OAHelper::MovePinByOffset(oaInstTerm* instTerm, oaPoint offset)
{
	//Get orientation of inst, and find its inverse
	oaInst *inst = instTerm->getInst();
	oaOrient orient = inst->getOrient();
	oaOrient inverseOrient = orient.getRelativeOrient(oacR0);
	
	//Rotate offset according to inverse orient
	oaTransform trans = oaTransform(inverseOrient);
	offset.transform(trans);
		
	oaTransform trans2 = oaTransform(offset);

	//Find associated term's pinfig and apply move
	oaTerm* assocTerm = instTerm->getTerm();
	oaIter<oaPin> pinIterator(assocTerm->getPins());
	oaIter<oaPinFig> pinFigIterator(pinIterator.getNext()->getFigs());
	oaPinFig* pinFig = pinFigIterator.getNext();

	pinFig->move(trans2);
}

//input is pointer to a pin, return its HPWL.
int OAHelper::getHPWL(oaInstTerm* instTerm)
 {
	oaNet* net = instTerm->getNet(false);	
	oaString netName;
	oaPoint point;
	int xcoor_min = 0, xcoor_max = 0, ycoor_min = 0, ycoor_max = 0;
	//net->getName(ns, netName);
	//cout << " Net Name is " << netName << endl;
	oaIter<oaInstTerm> instTermIterator(net->getInstTerms());
	oaIter<oaTerm> termIterator(net->getTerms());
	
	//BBOX contins LowerLeft point(xmin, ymin) and UpperRight point(xmax, ymax)
	//Initialize two points
	oaInstTerm* instTerm1 = instTermIterator.getNext();	
	point = GetAbsoluteInstTermPosition(instTerm1);
	xcoor_min = point.x();
	xcoor_max = point.x();
	ycoor_min = point.y();
	ycoor_max = point.y();
	
	//Iterate all instTerms
	while(instTerm1 = instTermIterator.getNext())
	{
		point = GetAbsoluteInstTermPosition(instTerm1);
		if(point.x() < xcoor_min) xcoor_min = point.x();
		if(point.x() > xcoor_max) xcoor_max = point.x();
		if(point.y() < ycoor_min) ycoor_min = point.y();
		if(point.y() > ycoor_max) ycoor_max = point.y();
	}
	
	//Itrrate all Terms
	while(oaTerm* term = termIterator.getNext())
	{
		point = GetTermPosition(term);
		if(point.x() < xcoor_min) xcoor_min = point.x();
		if(point.x() > xcoor_max) xcoor_max = point.x();
		if(point.y() < ycoor_min) ycoor_min = point.y();
		if(point.y() > ycoor_max) ycoor_max = point.y();
	}
		
	//calculate HPWL
	int HPWL = abs(xcoor_max - xcoor_min) + abs(ycoor_max - ycoor_min);
	return HPWL;
 }
 
//Function returns True if HPWL of term1 > term2, mainly use for sort function
bool OAHelper::less_HPWL(oaInstTerm* term1, oaInstTerm* term2)
 {
	int HPWL1 = getHPWL(term1);
	int HPWL2 = getHPWL(term2);
	if(HPWL1 > HPWL2)
		return true;
	return false;
 }
 
 //input is a pointer to a pin, get its Macro BBox.
 oaBox OAHelper::getMacroBBox(oaInstTerm* instTerm)
 {
	oaInst* inst = instTerm->getInst();
	oaBox box;
	inst->getBBox(box);
	return box;
 }
 
 //input is a pointer to a pin, get its Macro name.
 oaString OAHelper::getMacroName(oaInstTerm* instTerm)
 {
	oaInst* inst = instTerm->getInst();
	oaString macroName;
	inst->getName(ns, macroName);
	return macroName;
 }
 
 //input is a pointer to a pin, return its name.
 oaString OAHelper::getPinName(oaInstTerm* instTerm)
 {	
	oaString pinName; 
	instTerm->getTermName(ns,pinName);
	return pinName;
 }
 
 //input is a pointer to a pin, return a vector contains all pointers to pins within its Macro (including itself)
 vector<oaInstTerm*> OAHelper::getPinsInSameMacro(oaInstTerm* instTerm)
 {
	vector<oaInstTerm*> pointer_to_pins;
	oaInst* Macro = instTerm->getInst();
	oaIter<oaInstTerm> instTermIterator(Macro->getInstTerms());
	while(oaInstTerm* pin = instTermIterator.getNext())
	{		
		oaString pinName;
		pin->getTermName(ns, pinName);
		//cout << "Name of the Pin is " << assocTermName << endl;
		if(pinName != "VDD" && pinName!= "VSS")
		{
			pointer_to_pins.push_back(pin);
			//cout << " HPWL is " << OAHelper::getHPWL(instTerm) << endl;
		}		
	}
	return pointer_to_pins;
 }
 
 //input is a pointer to a pin, return a vector contains all pointers to pins within its Macro (excluding itself)
 vector<oaInstTerm*> OAHelper::getPinsInSameMacroWithoutItself(oaInstTerm* instTerm)
 {
	vector<oaInstTerm*> pointer_to_pins;
	oaString thisPinName;
	instTerm->getTermName(ns,thisPinName);
	oaInst* Macro = instTerm->getInst();
	oaIter<oaInstTerm> instTermIterator(Macro->getInstTerms());
	while(oaInstTerm* pin = instTermIterator.getNext())
	{		
		oaString pinName;
		pin->getTermName(ns, pinName);
		//cout << "Name of the Pin is " << assocTermName << endl;
		if(pinName != "VDD" && pinName!= "VSS" && pinName!= thisPinName)
		{
			pointer_to_pins.push_back(pin);
			//cout << " HPWL is " << OAHelper::getHPWL(instTerm) << endl;
		}		
	}
	return pointer_to_pins;
 }
 
 //Define PIN OFFSET =  ABS(lower.x()-upper.x())/2
 int OAHelper::getPinOffset(oaInstTerm* pin)
 {
	oaPoint pinLowerRight, pinUpperLeft;
	pinLowerRight = GetLowerRightPinPosition(pin);
	pinUpperLeft = GetUpperLeftPinPosition(pin);
	int offset = (pinLowerRight.x() - pinUpperLeft.x())/2;
	return offset;
 }
 
 //return relative position of a pin to its Macro (Left, Right, Top or Bottom)
 oaString OAHelper::getRelativePosition(oaInstTerm* pin)
 {
	int offset = getPinOffset(pin);
	oaBox box = getMacroBBox(pin);
	oaPoint bBoxLowerRight, bBoxUpperLeft, pinCenter;
	pinCenter = GetAbsoluteInstTermPosition(pin);
	oaString left = "left",right = "right",top = "top",bottom = "bottom";
	box.getLowerRight(bBoxLowerRight);
	box.getUpperLeft(bBoxUpperLeft);
	if(bBoxLowerRight.x() == pinCenter.x() + offset)
		return right;
	if(bBoxLowerRight.y() == pinCenter.y() - offset)
		return bottom;
	if(bBoxUpperLeft.x() == pinCenter.x() - offset)
		return left;
	if(bBoxUpperLeft.y() == pinCenter.y() + offset)
		return top;
	else
		return "ERROR OF KNOWING RELATIVE PIN POSITION";
 }
 
 //return correct direction for a pin to move.
 //Try both directions and choose the lower HPWL as correct direction
 oaString OAHelper::getCorrectDirection(oaInstTerm* pin, int moveStep)
 {
	oaString position = getRelativePosition(pin);
	oaPoint upperOffset = oaPoint(0, moveStep);
	oaPoint lowerOffset = oaPoint(0, -moveStep);
	oaPoint leftOffset = oaPoint(-moveStep, 0);
	oaPoint rightOffset = oaPoint(moveStep, 0);
	if(position == "left"){
		MovePinByOffset(pin, upperOffset);
		int upperNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Up HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, lowerOffset);
		MovePinByOffset(pin, lowerOffset);
		int lowerNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Down HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, upperOffset);
		if(upperNewHPWL < lowerNewHPWL)
			return "LeftToTop";
		if(upperNewHPWL > lowerNewHPWL)
			return "LeftToBottom";
		if(upperNewHPWL = lowerNewHPWL)
			return "cannotImprove";
	}
	else if(position == "right")
	{
		MovePinByOffset(pin, upperOffset);
		int upperNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Up HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, lowerOffset);
		MovePinByOffset(pin, lowerOffset);
		int lowerNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Down HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, upperOffset);
		if(upperNewHPWL < lowerNewHPWL)
			return "RightToTop";
		if(upperNewHPWL > lowerNewHPWL)
			return "RightToBottom";
		if(upperNewHPWL = lowerNewHPWL)
			return "cannotImprove";
	}
	else if(position == "top")	
	{
		MovePinByOffset(pin, leftOffset);
		int leftNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Left HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, rightOffset);
		MovePinByOffset(pin, rightOffset);
		int rightNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Right HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, leftOffset);
		if(leftNewHPWL < rightNewHPWL)
			return "TopToLeft";
		if(leftNewHPWL > rightNewHPWL)
			return "TopToRight";
		if(leftNewHPWL = rightNewHPWL)
			return "cannotImprove";
	}
	else if(position == "bottom")	
	{
		MovePinByOffset(pin, leftOffset);
		int leftNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Left HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, rightOffset);
		MovePinByOffset(pin, rightOffset);
		int rightNewHPWL = getHPWL(pin);
		//cout << " After Trying Moving Right HPWL is " << getHPWL(pin) << endl;
		MovePinByOffset(pin, leftOffset);
		if(leftNewHPWL < rightNewHPWL)
			return "BottomToLeft";
		if(leftNewHPWL > rightNewHPWL)
			return "BottomToRight";
		if(leftNewHPWL = rightNewHPWL)
			return "cannotImprove";
	}
	else
		return "Cannot Determine Pin Location";	 		 
 }

 //return Mahatten Distance between two pins.
int OAHelper::getMahattenDistance(oaInstTerm* pin1, oaInstTerm* pin2)
{
	
	oaPoint pin1Position = GetAbsoluteInstTermPosition(pin1);
	oaPoint pin2Position = GetAbsoluteInstTermPosition(pin2);
	return abs(pin1Position.x() - pin2Position.x()) + abs(pin1Position.y() - pin2Position.y());
}

//return true if perturbation for a pin violates input value.
bool OAHelper::isViolatePerturbation(oaPoint originalPosition, oaPoint currentPosition, int perturbation)
{
	int manhattenDistance = abs(originalPosition.x() - currentPosition.x()) + abs(originalPosition.y() - currentPosition.y());
	if(manhattenDistance > perturbation)
		return true;
	else
		return false;
}


/*********************
This is the main function for moving a pin
1.Get all needed infomation (Macro and corresponding extreme points, etc)
2.Move one step (0.28u) in its correct direction
	if position after one step reaches corner (8 corners in total)
	update correct direction and next pin position to move.
3.Check if there exits a conflict
	Loop to reach next position where no conflicts between this pin and all other pins in the same macro.
	update current position as a potential choose to move.
4.If HPWL after move is larger than before move, then undo the move and end function for this pin.
5.If Pertubation after move exceeds input, then undo the move and end function for this pin.
6.If 4 & 5 does not happen, update pin position and restart from 2 until 5 & 6 happens.		
**********************/
void OAHelper::moveOnePin(oaInstTerm* pin, int moveStep, int minimumPitch, vector<MyMacro*> allMacros, int perturbation)
{
	oaString pinMacroName = getMacroName(pin);
	oaString pinName;
	pin->getTermName(ns,pinName);
	
	//get Extreme/Marginal points from corresponding Macro prestored in MyMacro instances
	oaPoint bottomestLeft, bottomestRight, topestLeft, topestRight, 
				leftestBottom, leftestTop, rightestBottom, rightestTop;
	for(int i = 0; i < allMacros.size(); i++)
	{
		oaString macroName = allMacros[i]->getMacroName();
		if(pinMacroName == macroName){
			bottomestLeft = allMacros[i]->getBottomestLeft();
			bottomestRight= allMacros[i]->getBottomestRight();
			topestLeft = allMacros[i]->getTopestLeft();
			topestRight = allMacros[i]->getTopestRight();
			leftestBottom = allMacros[i]->getLeftestBottom();
			leftestTop = allMacros[i]->getLeftestTop();
			rightestBottom = allMacros[i]->getRightestBottom();
			rightestTop = allMacros[i]->getRightestTop();
		}
	}	
	
	//cout << " bottomestLeft is  " << bottomestLeft.x() << " , " << bottomestLeft.y() << endl;
	//cout << " bottomestRight is " << bottomestRight.x() << " , " << bottomestRight.y() << endl;
	//cout << " topestLeft is     " << topestLeft.x() << " , " << topestLeft.y() << endl;
	//cout << " topestRight is    " << topestRight.x() << " , " << topestRight.y() << endl;
	//cout << " leftestBottom is  " << leftestBottom.x() << " , " << leftestBottom.y() << endl;
	//cout << " leftestTop is     " << leftestTop.x() << " , " << leftestTop.y() << endl;
	//cout << " rightestBottom is " << rightestBottom.x() << " , " << rightestBottom.y() << endl;
	//cout << " rightestTop is    " << rightestTop.x() << " , " << rightestTop.y() << endl;
				
	vector<oaInstTerm*> pointers_to_all_other_pins = getPinsInSameMacroWithoutItself(pin);
	int number_of_pins = pointers_to_all_other_pins.size();
	//this vector stores position info for all other pins. Increase efficiency.
	vector<oaPoint> position_of_all_other_pins;
	for(int i = 0; i < pointers_to_all_other_pins.size(); i++){
		oaPoint conflictPinPosition = GetAbsoluteInstTermPosition(pointers_to_all_other_pins[i]);
		position_of_all_other_pins.push_back(conflictPinPosition);
	}
	
	oaString previousCorrectDirection = getCorrectDirection(pin, moveStep);
	oaString currentCorrectDirection = previousCorrectDirection;
	oaPoint originalPinPosition = GetAbsoluteInstTermPosition(pin);
	oaPoint previousPinPosition = originalPinPosition;
	oaPoint currentPinPosition;
	int originalHPWL = getHPWL(pin);
	int oldHPWL = originalHPWL;
	int newHPWL;
	
	bool isBetterHPWL = true;
	//cout << "correctDirection is " << previousCorrectDirection << endl;	
	while(isBetterHPWL)
	{		
		if(previousCorrectDirection == "LeftToTop"){
			if(previousPinPosition.y() == leftestTop.y()){
				currentPinPosition = topestLeft;
				MovePinToPosition(pin, topestLeft);
				currentCorrectDirection = "TopToRight";
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x(), previousPinPosition.y() + moveStep);
				MovePinToPosition(pin, currentPinPosition);
			}					
		}				
		
		if(previousCorrectDirection == "RightToTop"){			
			if(previousPinPosition.y() == rightestTop.y()){
				currentPinPosition = topestRight;
				MovePinToPosition(pin, topestRight);
				currentCorrectDirection = "TopToLeft";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x(), previousPinPosition.y() + moveStep);
				MovePinToPosition(pin, currentPinPosition);
			}
		}			
		
		if(previousCorrectDirection == "LeftToBottom"){
			if(previousPinPosition.y() == leftestBottom.y()){
				currentPinPosition = bottomestLeft;
				MovePinToPosition(pin, bottomestLeft);
				currentCorrectDirection = "BottomToRight";			
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x(), previousPinPosition.y() - moveStep);
				MovePinToPosition(pin, currentPinPosition);
			}
		}
		
		if(previousCorrectDirection == "RightToBottom"){
			if(previousPinPosition.y() == rightestBottom.y()){
				currentPinPosition = bottomestRight;
				MovePinToPosition(pin, bottomestRight);
				currentCorrectDirection = "BottomToLeft";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x(), previousPinPosition.y() - moveStep);
				MovePinToPosition(pin, currentPinPosition);
			}
			
		}
		
		if(previousCorrectDirection == "TopToLeft"){
			if(previousPinPosition.x() == topestLeft.x()){
				currentPinPosition = leftestTop;
				MovePinToPosition(pin, leftestTop);
				currentCorrectDirection = "LeftToBottom";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x() - moveStep, previousPinPosition.y());
				MovePinToPosition(pin, currentPinPosition);
			}
			
		}
		
		if(previousCorrectDirection == "TopToRight"){
			if(previousPinPosition.x() == topestRight.x()){
				currentPinPosition = rightestTop;
				MovePinToPosition(pin, rightestTop);
				currentCorrectDirection = "RightToBottom";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x() + moveStep, previousPinPosition.y());
				MovePinToPosition(pin, currentPinPosition);
			}
			
		}
		
		if(previousCorrectDirection == "BottomToLeft"){
			if(previousPinPosition.x() == bottomestLeft.x()){
				currentPinPosition = leftestBottom;
				MovePinToPosition(pin, leftestBottom);
				currentCorrectDirection = "LeftToTop";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x() - moveStep, previousPinPosition.y());
				MovePinToPosition(pin, currentPinPosition);
			}			
		}
		
		if(previousCorrectDirection == "BottomToRight"){
			if(previousPinPosition.x() == bottomestRight.x()){
				currentPinPosition = rightestBottom;
				MovePinToPosition(pin, rightestBottom);
				currentCorrectDirection = "RightToTop";				
				//cout << "***change corner" << endl;
			}
			else{
				currentPinPosition = oaPoint(previousPinPosition.x() + moveStep, previousPinPosition.y());
				MovePinToPosition(pin, currentPinPosition);
			}			
		}

		
		
		
		oaPoint beforeConflict = currentPinPosition;
		oaPoint afterConflict;
		bool isConflict = true;
		while(isConflict){
			
			int number_of_no_conflict_pins = 0;									
			for(int i = 0; i < pointers_to_all_other_pins.size(); i++){
				
				oaPoint conflictPinPosition = position_of_all_other_pins[i];
				int mahattenDistance = abs(beforeConflict.x()-conflictPinPosition.x()) + abs(beforeConflict.y()-conflictPinPosition.y());
					
				if(mahattenDistance >= minimumPitch){
					number_of_no_conflict_pins++;
				}
				else{					
					//cout << "CONFLICT*******************************************************CONFLICT" << endl;					
					if(currentCorrectDirection == "LeftToTop"){
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.y() == topestLeft.y()){
							afterConflict = oaPoint(conflictPinPosition.x() + minimumPitch, topestLeft.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "TopToRight";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}												
						//first case where the conflict pin is at least (1.4u) away of leftestTop, so we can insert pin between
						else if(beforeConflict.x() == conflictPinPosition.x() && leftestTop.y() - conflictPinPosition.y() >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x(), conflictPinPosition.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}				
						//second case where the conflict pin is <=1.4u away of leftestTop, but still in the same side
						else if(beforeConflict.x() == conflictPinPosition.x() && leftestTop.y() - conflictPinPosition.y() < minimumPitch){
							afterConflict = oaPoint(topestLeft.x() + minimumPitch, topestLeft.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "TopToRight";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}					
						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToTop Function" << endl; 
							goto AFTERCONFLICT;
						}
					}
					
					if(currentCorrectDirection == "RightToTop"){												
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.y() == topestRight.y()){
							afterConflict = oaPoint(conflictPinPosition.x() - minimumPitch, topestRight.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "TopToLeft";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of rightestTop, so we can insert pin between
						else if(conflictPinPosition.x() == beforeConflict.x() && rightestTop.y() - conflictPinPosition.y() >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x(), conflictPinPosition.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}			
						//second case where the conflict pin is <=1.4u away of rightestTop, but still in the same side
						else if(conflictPinPosition.x() == beforeConflict.x() && rightestTop.y() - conflictPinPosition.y() < minimumPitch){
							afterConflict = oaPoint(topestRight.x() - minimumPitch, topestRight.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "TopToLeft";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						else{
							cout << "ERROR in considering conflict pin cases! Check RightToTop Function" << endl;
							goto AFTERCONFLICT;
					}					
				}
				
					if(currentCorrectDirection == "LeftToBottom"){						
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.y() == bottomestLeft.y()){
							afterConflict = oaPoint(conflictPinPosition.x() + minimumPitch, bottomestLeft.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "BottomToRight";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of leftestBottom, so we can insert pin between
						else if(conflictPinPosition.x() == beforeConflict.x() && (conflictPinPosition.y() - leftestBottom.y()) >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x(), conflictPinPosition.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}
						//second case where the conflict pin is <=1.4u away of leftestBottom, but still in the same side
						else if(conflictPinPosition.x() == beforeConflict.x() && conflictPinPosition.y() - leftestBottom.y() < minimumPitch){
							afterConflict = oaPoint(bottomestLeft.x() + minimumPitch, bottomestLeft.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "BottomToRight";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}
						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToBottom Function" << endl; 
							goto AFTERCONFLICT;
						}
					}
					
					if(currentCorrectDirection == "RightToBottom"){		
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.y() == bottomestRight.y()){
							afterConflict = oaPoint(conflictPinPosition.x() - minimumPitch, bottomestRight.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "BottomToLeft";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of rightestBottom, so we can insert pin between
						else if(conflictPinPosition.x() == beforeConflict.x() && conflictPinPosition.y() - rightestBottom.y() > minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x(), conflictPinPosition.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//second case where the conflict pin is <=1.4u away of rightestBottom, but still in the same side
						else if(conflictPinPosition.x() == beforeConflict.x() &&  conflictPinPosition.y()- rightestBottom.y() <= minimumPitch){
							afterConflict = oaPoint(bottomestRight.x() - minimumPitch, bottomestRight.y());
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "BottomToLeft";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}				
						else{
							cout << "ERROR in considering conflict pin cases! Check RightToBottom Function" << endl; 
							goto AFTERCONFLICT;
						}
						
					}
					
					if(currentCorrectDirection == "TopToLeft"){
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.x() == leftestTop.x()){
							afterConflict = oaPoint(leftestTop.x(), conflictPinPosition.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "LeftToBottom";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of topestLeft, so we can insert pin between
						else if(beforeConflict.y() == conflictPinPosition.y() && conflictPinPosition.x() - topestLeft.x() >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x() - minimumPitch, conflictPinPosition.y());
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//second case where the conflict pin is <=1.4u away of topestLeft, but still in the same side
						else if(beforeConflict.y() == conflictPinPosition.y() && conflictPinPosition.x() - topestLeft.x() < minimumPitch){
							afterConflict = oaPoint(leftestTop.x(), leftestTop.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "LeftToBottom";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}
						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToBottom Function" << endl;	
							goto AFTERCONFLICT;
						}
					}	
					
					if(currentCorrectDirection == "TopToRight"){												
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.x() == rightestTop.x()){
							afterConflict = oaPoint(rightestTop.x(), conflictPinPosition.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "RightToBottom";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of topestRight, so we can insert pin between
						else if(beforeConflict.y() == conflictPinPosition.y() && topestRight.x() - conflictPinPosition.x()  >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x() + minimumPitch, conflictPinPosition.y());
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//second case where the conflict pin is <=1.4u away of topestRight, but still in the same side
						else if(beforeConflict.y() == conflictPinPosition.y() && topestRight.x() - conflictPinPosition.x() < minimumPitch){
							afterConflict = oaPoint(rightestTop.x(), rightestTop.y() - minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "RightToBottom";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}

						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToBottom Function" << endl; 
							goto AFTERCONFLICT;
						}
					}
					
					if(currentCorrectDirection == "BottomToLeft"){
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.x() == leftestBottom.x()){
							afterConflict = oaPoint(leftestTop.x(), conflictPinPosition.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "LeftToTop";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of bottomestLeft, so we can insert pin between
						else if(beforeConflict.y() == conflictPinPosition.y() && conflictPinPosition.x() - bottomestLeft.x() >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x() - minimumPitch, conflictPinPosition.y());
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//second case where the conflict pin is <=1.4u away of bottomestLeft, but still in the same side
						else if(beforeConflict.y() == conflictPinPosition.y() && conflictPinPosition.x() - bottomestLeft.x() < minimumPitch){
							afterConflict = oaPoint(leftestBottom.x(), leftestBottom.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "LeftToTop";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}
						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToTop Function" << endl;
							goto AFTERCONFLICT;
						}
					}
					
					if(currentCorrectDirection == "BottomToRight"){
						//third case where the conflict pin is on the other side
						if(conflictPinPosition.x() == rightestBottom.x()){
							afterConflict = oaPoint(rightestBottom.x(), conflictPinPosition.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "RightToTop";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//first case where the conflict pin is at least (1.4u) away of bottomestRight, so we can insert pin between
						else if(beforeConflict.y() == conflictPinPosition.y() && bottomestRight.x() - conflictPinPosition.x()  >= minimumPitch){
							afterConflict = oaPoint(conflictPinPosition.x() + minimumPitch, conflictPinPosition.y());
							MovePinToPosition(pin, afterConflict);
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}						
						//second case where the conflict pin is <=1.4u away of bottomestRight, but still in the same side
						else if(beforeConflict.y() == conflictPinPosition.y() && bottomestRight.x() - conflictPinPosition.x() < minimumPitch){
							afterConflict = oaPoint(rightestBottom.x(), rightestBottom.y() + minimumPitch);
							MovePinToPosition(pin, afterConflict);
							currentCorrectDirection = "RightToTop";
							beforeConflict = afterConflict;
							goto End_of_Conflict;
						}
						else{
							cout << "ERROR in considering conflict pin cases! Check LeftToBottom Function" << endl;
							goto AFTERCONFLICT;					
						}
					}
				
				
				}
			}				
			if(number_of_no_conflict_pins == number_of_pins){
				isConflict = false;
				break;
			}									
			End_of_Conflict:
			;
		}
				
		AFTERCONFLICT:
		
		currentPinPosition = beforeConflict;				
		//cout << " Current Position of is " << currentPinPosition.x() << " , " << currentPinPosition.y() << endl;
		
		newHPWL = getHPWL(pin);
		//cout << " oldHPWL = " << oldHPWL << endl;
		//cout << " newHPWL = " << newHPWL << endl;
		
		if(newHPWL >= oldHPWL){
			MovePinToPosition(pin, previousPinPosition);
			isBetterHPWL = false;
			//cout << " Initial Position of " << pinName << " is " << originalPinPosition.x() << " , " << originalPinPosition.y() << endl;
			//cout << " Final Position of " << pinName << " is " << previousPinPosition.x() << " , " << previousPinPosition.y() << endl;
			//cout << " End due to HPWL !" << endl;
			//cout << " originalHPWL  = " << originalHPWL << endl;
			//cout << " aftermoveHPWL = " << oldHPWL << endl;
			//cout << " \t" << endl;
			goto End_of_move;
		}
		
		bool isPerturbationLimited = true;
		//cout << " isPerturbationLimited is " << isPerturbationLimited << endl;
		//When perturbation = "inf"
		if(perturbation == -2000){
			isPerturbationLimited = false;
			//cout << " perturbation = " << perturbation << endl;
			//cout << " isPerturbationLimited is " << isPerturbationLimited << endl;
		}
		
		if(isPerturbationLimited == true){
					
			if(isViolatePerturbation(originalPinPosition, currentPinPosition, perturbation)){				
				//cout << " perturbation = " << perturbation << endl;				
				//when Pertubation > maxnimum input, undo the move and break
				MovePinToPosition(pin, previousPinPosition);
				//cout << " Initial Position of " << pinName << " is " << originalPinPosition.x() << " , " << originalPinPosition.y() << endl;
				//cout << " Final Position of " << pinName << " is " << previousPinPosition.x() << " , " << previousPinPosition.y() << endl;
				//cout << " End due to Pertubation !" << endl;
				//cout << " /t" << endl;
				goto End_of_move;
			}
		}
		
		//cout << "Update Info!" << endl;
		//update HPWL and PinPosition
		previousPinPosition = currentPinPosition;
		previousCorrectDirection = currentCorrectDirection;
		oldHPWL = newHPWL;
	}
	
	End_of_move:
	;
}
		
 //A class that stores needed information for pins within it. Increase efficiency
 //All macros in the design will be instantialized before moving pin.
MyMacro::MyMacro(oaInst* macro, int minimumPitch){
	_macro = macro;
    _minimumPitch = minimumPitch;
	macro->getName(ns, _macroName);
	setAllPins();
	calculateCorners();
	//cout << "Object is being initialized!"<< endl;
}

void MyMacro::setAllPins(){
	vector<oaInstTerm*> pointerAllPins;
	oaIter<oaInstTerm> instTermIterator(_macro->getInstTerms());
	while(oaInstTerm* pin = instTermIterator.getNext())
	{		
		oaString pinName;
		pin->getTermName(ns, pinName);
		//cout << "Name of the Pin is " << assocTermName << endl;
		if(pinName != "VDD" && pinName!= "VSS")
		{
			pointerAllPins.push_back(pin);
			//cout << " HPWL is " << OAHelper::getHPWL(pin) << endl;
		}		
	}

	oaInstTerm* pin = pointerAllPins[0];
	oaPoint pinLowerRight, pinUpperLeft;
	pinLowerRight = OAHelper::GetLowerRightPinPosition(pin);
	pinUpperLeft = OAHelper::GetUpperLeftPinPosition(pin);
	_pinOffset = abs((pinLowerRight.x() - pinUpperLeft.x())/2);
	//cout << "_pinOffset is " << _pinOffset <<endl;
	
	_pointerAllPins.assign(pointerAllPins.begin(), pointerAllPins.end());
}

void MyMacro::calculateCorners(){
	oaBox macroBBox;
	_macro->getBBox(macroBBox);
	macroBBox.getUpperLeft(_macroUpperLeft);
	macroBBox.getLowerRight(_macroLowerRight);
	//cout << " _macroUpperLeft is " << _macroUpperLeft.x() << " , " << _macroUpperLeft.y() <<endl;
	//cout << " _macroLowerRight is " << _macroLowerRight.x() << " , " << _macroLowerRight.y() <<endl;
	_leftestOffset = _macroUpperLeft.x() + _pinOffset + _minimumPitch;
	//cout << " leftestOffset is " << _leftestOffset << endl;
	_rightestOffset = _macroLowerRight.x() - _pinOffset - _minimumPitch;
	//cout << " rightestOffset is " << _rightestOffset << endl;
	_topestOffset = _macroUpperLeft.y() - _pinOffset - _minimumPitch;
	//cout << " topestOffset is " << _topestOffset << endl;
	_bottomestOffset = _macroLowerRight.y() + _pinOffset + _minimumPitch;
	//cout << " bottomestOffset is " << _bottomestOffset << endl;
	//cout << " size of pointers is " << _pointerAllPins.size() << endl;
	
	bool left_or_right_flag = true;
	bool top_or_bottom_flag = true;
	for(int i = 0; i < _pointerAllPins.size(); i++)
	{
		oaString iterator_pin_position = OAHelper::getRelativePosition(_pointerAllPins[i]);
		oaPoint pinLeftRightTopBottom = OAHelper::GetAbsoluteInstTermPosition(_pointerAllPins[i]);

		if(left_or_right_flag)
		{
			if(iterator_pin_position == "left" || iterator_pin_position == "right")
			{				
				//cout << "***pinLeftRight position is " << pinLeftRightTopBottom.x() << " "
				//<< pinLeftRightTopBottom.y() << endl;
				
				//initialize
				int y_coor;
				y_coor = OAHelper::GetAbsoluteInstTermPosition(_pointerAllPins[i]).y();
				//cout << "y_coor is "<< y_coor <<endl;
				while(y_coor < _topestOffset){
					y_coor += _minimumPitch;
				}
				_leftestTop = oaPoint(_macroUpperLeft.x() + _pinOffset, y_coor);
				_rightestTop = oaPoint(_macroLowerRight.x() - _pinOffset, y_coor);
				while(y_coor > _bottomestOffset){
					y_coor -= _minimumPitch;
				}
				_leftestBottom = oaPoint(_macroUpperLeft.x() + _pinOffset, y_coor);
				_rightestBottom = oaPoint(_macroLowerRight.x() - _pinOffset, y_coor);
				left_or_right_flag = false;						
			}					
		}
		if(top_or_bottom_flag)
		{
			if(iterator_pin_position == "top" || iterator_pin_position == "bottom")
			{
				//cout << "***pinTopBottom position is " << pinLeftRightTopBottom.x() << " "
				//<< pinLeftRightTopBottom.y() << endl;
				
				//initialize
				int x_coor;
				x_coor = OAHelper::GetAbsoluteInstTermPosition(_pointerAllPins[i]).x();
				//cout << "x_coor is "<< x_coor <<endl;
				while(x_coor < _rightestOffset){
					x_coor += _minimumPitch;
				}
				_topestRight = oaPoint(x_coor, _macroUpperLeft.y() - _pinOffset);
				_bottomestRight = oaPoint(x_coor, _macroLowerRight.y() + _pinOffset);
				while(x_coor > _leftestOffset){
					x_coor -= _minimumPitch;
				}
				_topestLeft = oaPoint(x_coor, _macroUpperLeft.y() - _pinOffset);
				_bottomestLeft = oaPoint(x_coor, _macroLowerRight.y() + _pinOffset);
				top_or_bottom_flag = false;						
			}					
		}
	}
}

oaString MyMacro::getMacroName(){
	return _macroName;
}

oaPoint MyMacro::getBottomestLeft(){
	return _bottomestLeft;
}

oaPoint MyMacro::getBottomestRight(){
	return _bottomestRight;
}

oaPoint MyMacro::getTopestLeft(){
	return _topestLeft;
}

oaPoint MyMacro::getTopestRight(){
	return _topestRight;
}

oaPoint MyMacro::getLeftestBottom(){
	return _leftestBottom;
}

oaPoint MyMacro::getLeftestTop(){
	return _leftestTop;
}

oaPoint MyMacro::getRightestBottom(){
	return _rightestBottom;
}

oaPoint MyMacro::getRightestTop(){
	return _rightestTop;
}