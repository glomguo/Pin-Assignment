/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#include <iostream>
#include "oaDesignDB.h"
#include <vector>
#include <algorithm>
#include "InputOutputHandler.h"
#include "ProjectInputRules.h"
#include "OAHelper.h"

using namespace oa;
using namespace std;

static oaNativeNS ns;

/*
 * 
 */
int main(int argc, char *argv[])
{
    //Hello World
    cout << "=================================================" << endl;
    cout << "Automated Inter-Chip Pin Assignment" << endl;
    cout << "UCLA EE 201A Winter 2017 Course Project" << endl;
    cout << endl;
    cout << "<YOUR TEAM NUMBER HERE>" << endl;
    cout << "<YOUR NAMES HERE>" << endl;
    cout << "<YOUR STUDENT IDS HERE>" << endl;
    cout << "=================================================" << endl << endl;
   
    //Usage
    cout << "Ensure you have an existing OA design database before running this tool. Also please adhere to the following command-line usage:" << endl;
    cout << "./PinAssign <DESIGN NAME> <OUTPUT DESIGN NAME> <INPUT RULE FILE NAME> <MACRO INFO FILENAME>" << endl;
    cout << "For example:" << endl;            
    cout << "./PinAssign sbox_x2 sbox_x2_minrule benchmarks/sbox_x2/min.inputrules logs/sbox_x2/pinassign_sbox_x2_minrule.macros" << endl;

	// Initialize OA with data model 3
	oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, 3);
    oaRegionQuery::init("oaRQSystem");

    //Read in design library
    cout << "\nReading design library..." << endl;
    DesignInfo designInfo;
    InputOutputHandler::ReadInputArguments(argv, designInfo);
	oaLib* lib;
    oaDesign* design= InputOutputHandler::ReadOADesign(designInfo, lib);

	// Get the TopBlock for this design.
    oaBlock* block= InputOutputHandler::ReadTopBlock(design);

	// Fetch all instances in top block and save a unique master design copy for each
    cout << "\nSaving copies of each unique macro instance..." << endl;
	InputOutputHandler::SaveMacroDesignCopies(designInfo, block);
	
    //now, get the input rules from file
    cout << "\nReading input rules..." << endl;
    ProjectInputRules inputRules(designInfo.inputRuleFileName);
	//int pinLayer = ProjectInputRules::getPinLayer(); //pinLayer = 5;
	//int minRoutingLayer = ProjectInputRules::getMinRoutingLayer(); //minRoutingLayer = 6;
	//int maxRoutingLayer = ProjectInputRules::getMaxRoutingLayer(); //maxRoutingLayer = 7~9;
	float pinMoveStep = inputRules.getPinMoveStep(); //pinMoveStep = 0.28u;
	float minPinPitch = inputRules.getMinPinPitch(); //minPinPitch = 2~10u;
	float maxPinPerturbation = inputRules.getMaxPinPerturbation(); //maxPinPerturbation = 50~infinity(-1);
    inputRules.print();
	//As defined, 1u = 2000
	int pinMoveDistance = (int)(pinMoveStep * 2000);
	int minimumPinPitch = (int)(minPinPitch * 2000);
	int maximumPinPerturbation = (int)(maxPinPerturbation * 2000);
	cout << "*** Pin Move Distance is   " << pinMoveDistance << endl;
	cout << "*** minimum Pin Pitch is   " << minimumPinPitch << endl;
	cout << "*** max PinPerturbation is " << maximumPinPerturbation << endl;
    
    cout << "\nBeginning pin assignment..." << endl;
	//=====================================================================
    // All pin assignment code should be handled here
	// The scratch code below covers basic traversal and some useful functions provided
	// You are free to edit everything in this section (marked by ==)

	oaString netName, instName, masterCellName, assocTermName, termName;

	//MyMacro is a class that stores needed information for pins within it. Increase efficiency
	//All macros in the design will be instantialized before moving pin.
	//Declare pointers of Macros
	int num_of_macros = block->getInsts().getCount();
	MyMacro* allMacros[num_of_macros];
	vector<oaInst*> pointers_to_macros;
	vector<MyMacro*> pointers_to_MyMacros;
	oaIter<oaInst> instIterator(block->getInsts());
	while(oaInst* macro = instIterator.getNext())
	{
		pointers_to_macros.push_back(macro);
	}	
	
	for(int i = 0; i < num_of_macros; i++)
	{
		allMacros[i]= new MyMacro(pointers_to_macros[i], pinMoveDistance);	
	}
	
	for(int i = 0; i < num_of_macros; i++)
	{
		pointers_to_MyMacros.push_back(allMacros[i]);
	}
	
	//Iterate through nets and get a vector pointing to all pins sorted from longest HPWL to lowest HPWL
	vector<oaInstTerm*> instTermPointers;	
	oaIter<oaNet> netIterator(block->getNets());
	while (oaNet* net = netIterator.getNext()) {
		net->getName(ns, netName);
		oaIter<oaInstTerm> instTermIterator(net->getInstTerms());
		
		while (oaInstTerm* instTerm = instTermIterator.getNext()) {
			instTerm->getTermName(ns, assocTermName);
			//cout << "Name of the Pin is " << assocTermName << endl;
			if(assocTermName != "VDD" && assocTermName!= "VSS")
			{
				instTermPointers.push_back(instTerm);
				//cout << " HPWL is " << OAHelper::getHPWL(instTerm) << endl;
			}
		}
				
	}
	//sort all pins with HPWL from high to low
	sort(instTermPointers.begin(),instTermPointers.end(), OAHelper::less_HPWL);
	
	//Do the move for each pin.
	//#Pins to be operated depend on tradeoff between time and better optimizion
	for(int i = 0; i < instTermPointers.size(); i++){
		OAHelper::moveOnePin(instTermPointers[i], pinMoveDistance, minimumPinPitch, pointers_to_MyMacros, maximumPinPerturbation);		
	}

	cout << "Totol Number Of pins is " << instTermPointers.size() << endl;
		
	//Release memory
	for(int i = 0; i < num_of_macros; i++)
	{
		delete allMacros[i];	
	}
		
	/* 	//TEST FOR getPinsInSameMacroWithoutItself Function
	vector<oaInstTerm*> otherpins = OAHelper::getPinsInSameMacroWithoutItself(instTermPointers[0]);
	oaString thispinName;
	instTermPointers[0]->getTermName(ns,thispinName);
	cout << "This pin name is " << thispinName << endl;	
	for(int i = 0; i < otherpins.size(); i++ ){
		oaString otherpinName;
		otherpins[i]->getTermName(ns,thispinName);
		cout << " Other pin name is " << thispinName << endl;
	} */
		
	/***************************************************/
/* 	//Declare pointers of oaInstTerm
	vector<oaInstTerm*> instTermPointers;		
	
	oaIter<oaNet> netIterator(block->getNets());
	while (oaNet* net = netIterator.getNext()) {
		net->getName(ns, netName);
		oaIter<oaInstTerm> instTermIterator(net->getInstTerms());
		oaIter<oaTerm> termIterator(net->getTerms());
		
	
		

		while (oaInstTerm* instTerm = instTermIterator.getNext()) {
			instTerm->getTermName(ns, assocTermName);
			//cout << "Name of the Pin is " << assocTermName << endl;
			if(assocTermName != "VDD" && assocTermName!= "VSS")
			{
				instTermPointers.push_back(instTerm);
				//cout << " HPWL is " << OAHelper::getHPWL(instTerm) << endl;
			}
		}
				
	} */

/* 	for(int i=0; i<instTermPointers.size();i++)
	{
		oaString compartor = OAHelper::getMacroName(instTermPointers[i]);
		cout << "getMacroName=" <<compartor<<endl;
	}	 */
	
	//Input a pinX and vectors of all pins, output all pins owning same Macro as pinX
	//Test get Pins in the same Macro
/* 	oaInstTerm* testPin = instTermPointers[0];
 	vector<oaInstTerm*> testOutputPins = OAHelper::getPinsInSameMacro(testPin);
	for(int i = 0; i < testOutputPins.size(); i++)
	{
		oaString pinName;
		testOutputPins[i]->getTermName(ns, pinName);
		cout << "Macro is " << OAHelper::getMacroName(testOutputPins[i]) <<endl;
		cout << " Name of the Pin is " << pinName << endl;
		cout << "***********************" << endl;
	} */
	
	//TestSort
/* 	for(int i = 0; i < instTermPointers.size(); i++)
	{
		instTermPointers[i]->getTermName(ns, assocTermName);
		cout << "After Name of the Pin is " << assocTermName << endl;
		int HPWL = OAHelper::getHPWL(instTermPointers[i]);
		cout << " HPWL is " << HPWL << endl;			
	} */
	
/* 	for(int i = 0; i < instTermPointers.size(); i++)
	{
		oaString name;
		oaInstTerm* instTerm = instTermPointers[i];
		int HPWL = OAHelper::getHPWL(instTermPointers[i]);
		instTerm->getTermName(ns,name);
		oaPoint lowerRight, upperLeft, pinPoint;
		oaBox box = OAHelper::getMacroBBox(instTerm);
		pinPoint  = OAHelper::GetAbsoluteInstTermPosition(instTerm);
		box.getLowerRight(lowerRight);
		box.getUpperLeft(upperLeft);
		cout << "Name of the Pin is " << name << endl;
		cout << " HPWL is " << HPWL << endl;		
		cout << " Pin position is " << pinPoint.x() << " , " << pinPoint.y() << endl;
		cout << "  Corresponding Macro Name is " << OAHelper::getMacroName(instTerm) << endl;
		cout << "  Corresponding BBox Lower Right is " << lowerRight.x() << " , " << lowerRight.y() << endl;
		cout << "  Corresponding BBox Upper Left is " << upperLeft.x() << " , " << upperLeft.y() << endl;
		cout << "PIN POSITION IS " << OAHelper::getRelativePosition(instTerm) << endl;
		cout << " Direction to Move is " << OAHelper::getCorrectDirection(instTerm, pinMoveDistance) << endl;
		cout << "\t" << endl;
	} */
	
	//=====================================================================
	
    //Save the improved version of the design
    InputOutputHandler::SaveAndCloseAllDesigns(designInfo, design, block);

	if (lib)
		lib->close();

    cout << endl << "\nDone!" << endl;
    return 0;
}
