/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#include <iostream>
#include "oaDesignDB.h"
#include <vector>
#include "InputOutputHandler.h"
#include "ProjectInputRules.h"
#include "OAHelper.h"
#include "Verify.h"

using namespace oa;
using namespace std;

static oaNativeNS ns;

int main(int argc, char *argv[])
{
    //Hello World
    cout << "=================================================" << endl;
    cout << "Automated Inter-Chip Pin Assignment" << endl;
    cout << "UCLA EE 201A Winter 2017 Course Project" << endl;
    cout << "<Post-Pin Assignment Verification Script>" << endl;
    cout << "=================================================" << endl << endl;
   
    //Usage
    cout << "Ensure you have an existing OA design database which has undergone pin assignment before running this tool. Also please adhere to the following command-line usage:" << endl;
    cout << "./PAVerify <DESIGN NAME> <OUTPUT DESIGN NAME> <INPUT RULE FILE NAME> <OUTPUT SUMMARY LOG FILENAME>" << endl;
    cout << "For example:" << endl;            
    cout << "./PAVerify sbox_x2 sbox_x2_minrule benchmarks/sbox_x2/min.inputrules logs/sbox_x2/PAVerify_sbox_x2_minrule.summary" << endl;

	// Initialize OA with data model 3
	oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, 3);
    oaRegionQuery::init("oaRQSystem");

    //Read in design library
    cout << "\nReading design library..." << endl;
    DesignInfo designInfo;
    InputOutputHandler::ReadInputArguments(argv, designInfo);
	oaLib* lib;
	InputOutputHandler::ReadOALibrary(lib, designInfo);

	//Read input rule file
    cout << "\nReading input rules..." << endl;
    ProjectInputRules inputRules(designInfo.inputRuleFileName); 
    inputRules.print();

	//Read input and output designs
	oaScalarName libName(ns, designInfo.libName.c_str());
	oaScalarName viewName(ns, designInfo.designView.c_str());
	oaScalarName cellNameOrig(ns, designInfo.designName.c_str());
	oaScalarName cellNameOut(ns, designInfo.outputDesignName.c_str());

	oaDesign *origDesign = oaDesign::open(libName, cellNameOrig, viewName,'r');
	oaDesign *outDesign = oaDesign::open(libName, cellNameOut, viewName,'r');
	
	ofstream summaryLog;
	summaryLog.open(designInfo.summaryLogFileName.c_str()); 
    summaryLog << "==================" << endl;
    summaryLog << " PAVerify SUMMARY " << endl;
    summaryLog << "==================" << endl;
    summaryLog << endl;
    summaryLog << "Design: " << designInfo.outputDesignName << endl << endl;
	
	//Final message flags
	bool minInnovusRequirements = true;

	//=================================================================================

	cout << endl << "1. Verifying net connectivity" << endl << endl;
	bool connectivityViolation = Verify::NetConnectivity(origDesign, outDesign);
	if (connectivityViolation) {
		summaryLog << "    >> Connectivity violations found! <<" << endl;
		minInnovusRequirements  = false;
	}
	else
		summaryLog << "    Connectivity clean! " << endl;

	cout << endl << "2. Verifying macro placement and orientation" << endl << endl;
	bool macroViolation = Verify::Macros(origDesign, outDesign);
	if (macroViolation) {
		summaryLog << "    >> Macro placement/orientation violations found! <<" << endl;
		minInnovusRequirements  = false;
	}
	else
		summaryLog << "    Macro placement/orientation clean! " << endl;

	cout << endl << "3. Verifying system pin placement" << endl << endl;
	bool systPinViolation = Verify::SystemPins(origDesign, outDesign);
	if (systPinViolation) {
		summaryLog << "    >> System Pin placement violations found! <<" << endl;
		minInnovusRequirements = false;
	}
	else
		summaryLog << "    System pin placement clean! " << endl;

	cout << endl << "4. Verifying macro pin placement and min pitch" << endl << endl;
	bool macroPinPlacementViolation = Verify::MacroPinPlacement(origDesign, outDesign, inputRules);
	if (macroPinPlacementViolation) {
		summaryLog << "    >> Macro pin placement violations found! <<" << endl;
	}
	else
		summaryLog << "    Macro pin placement clean! " << endl;

	cout << endl << "5. Verifying macro pin perturbation" << endl << endl;
	bool macroPinPerturbationViolation = Verify::MacroPinPerturbation(origDesign, outDesign, inputRules);
	if (macroPinPerturbationViolation) {
		summaryLog << "    >> Macro pin perturbation violations found! <<" << endl;
	}
	else
		summaryLog << "    Macro pin perturbation clean! " << endl;

	//=================================================================================
	
	summaryLog << endl << "==============================" << endl;
	if (minInnovusRequirements)
		summaryLog << "MIN INNOVUS REQUIREMENTS MET :)" << endl;
	else
		summaryLog << "MIN INNOVUS REQUIREMENTS VIOLATED :(" << endl;
	if (!macroPinPlacementViolation)
		summaryLog << "pmin = 1" << endl;
	else
		summaryLog << "pmin = 0" << endl;
	if (!macroPinPerturbationViolation )
		summaryLog << "pmax = 1" << endl;
	else
		summaryLog << "pmax = 0" << endl;
	
    summaryLog.close();
	
	//Close all designs
	if (origDesign->isDesign())
		origDesign->close();
	if (outDesign->isDesign())
		outDesign->close();

	if (lib)
		lib->close();

    cout << endl << "\nDone!" << endl;
    return 0;
}
