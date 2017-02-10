// *****************************************************************************
// HelloWorld.cpp
//
// The following tasks are performed by this program
//  1. Derive an oaTech observer to handle conflicts in the technology hierarchy
//  2. Derive an oaLibDefsList observer to handle warnings related to lib.defs
//  3. Open the design
//  4. Print the library name
//  5. Print the cell name
//  6. Print the view name
//  7. Create nets with the names "Hello" and "World"
//  8. Save these nets
//  9. Run the net iterator and print the existing nets in the design
//
// ****************************************************************************
// Except as specified in the OpenAccess terms of use of Cadence or Silicon
// Integration Initiative, this material may not be copied, modified,
// re-published, uploaded, executed, or distributed in any way, in any medium,
// in whole or in part, without prior written permission from Cadence.
//
//                Copyright 2002-2005 Cadence Design Systems, Inc.
//                           All Rights Reserved.
//
// To distribute any derivative work based upon this file you must first contact
// Si2 @ contracts@si2.org.
//
// *****************************************************************************
// *****************************************************************************

#include <iostream>
#include "oaDesignDB.h"

#include "/w/class/ee201a/ee201ata/oa/examples/oa/common/commonTechObserver.h"
#include "/w/class/ee201a/ee201ata/oa/examples/oa/common/commonLibDefListObserver.h"
#include "/w/class/ee201a/ee201ata/oa/examples/oa/common/commonFunctions.h"


using namespace oa;
using namespace std;

static oaNativeNS ns;




// ****************************************************************************
// printDesignNames()
//
// This function gets the library, cell and view names associated with the open
// design and prints them.
// ****************************************************************************
void
printDesignNames(oaDesign *design)
{
    oaString    libName;
    oaString    cellName;
    oaString    viewName;

    // Library, cell and view names are obtained.
    design->getLibName(ns, libName);
    design->getCellName(ns, cellName);
design->getViewName(ns, viewName);

    // Library, cell and view names are printed.
    cout << "\tThe library name for this design is : " << libName << endl;
    cout << "\tThe cell name for this design is : " << cellName << endl;
    cout << "\tThe view name for this design is : " << viewName << endl;
}



// ****************************************************************************
// void printNets()
//
//  This function invokes the net iterator for the design and prints the names
//  of the nets one by one.
// ****************************************************************************
void
printNets(oaDesign *design)
{
    // Get the TopBlock of the current design
    oaBlock *block = design->getTopBlock();

    if (block) {
        oaString        netName;

        cout << "The following nets exist in this design." << endl;

        // Iterate over all nets in the design
        oaIter<oaNet>   netIterator(block->getNets());
        while (oaNet * net = netIterator.getNext()) {
            net->getName(ns, netName);
            cout << "\t" << netName << endl;
        }
    } else {
        cout << "There is no block in this design" << endl;
    }
}

	/*	
		1.Iterate through OaNets
		2.get OaTerms & OalnstTerms connected with Oanets
		3.get number of nets Nn with ignoring OaTerms connected OaNets, Name with VDD & VSS (PrintName?)
		4.For the remaining, get number of end points Ne(i) for each(ith)net
		5.Ne(i) - 1 will be the fan-out for each net?
		6.If Ne(i) = 1, meaning floating, fan-out = 0 by defination, so the equation still holds.
		7.Overall Average fan-out = sum of Ne(i) / Nn
		*/

void printOverallAverageFanOut(oaDesign *design)
{
	// Get the TopBlock of the current design
	oaBlock *block = design->getTopBlock();

	int total_number_nets_with_instTerms = 0;
	int total_number_fan_outs = 0;
	double overall_average_fan_outs;
	
	if (block) {
		oaString netName;
		int number_instTerms;
	
		cout << endl << "----- Cheng Guo: Problem 2 -----" << endl;

		// Iterate over all nets in the design
		oaIter<oaNet>   netIterator(block->getNets());
		while (oaNet * net = netIterator.getNext()) {
			net->getName(ns, netName);
			if(net->getTerms(oacTermIterNotImplicit).isEmpty()){
				if(!net->getInstTerms(oacInstTermIterNotImplicit).isEmpty()){
					if(netName == "VDD"){
						cout << "\t" << netName << "          " << "This net is VDD" << endl;
						}
					else if(netName == "VSS"){
						cout << "\t" << netName << "          " <<"This net is VSS" << endl;
						}
					else{
						total_number_nets_with_instTerms ++;
						number_instTerms = net->getInstTerms(oacInstTermIterNotImplicit).getCount() - 1;
						total_number_fan_outs += number_instTerms;			
						cout << "\t" << netName << "          " << number_instTerms << endl;
						}	
				}			
				else{
					cout << "\t" << "This Net does not have InstTerms" << endl;
				}			
			}
			else{
				cout << "\t" << netName << "          " <<"This net is conncted to one or more terminal" << endl;
			}
			
		}			
	} else {
		cout << "There is no block in this design" << endl;
	}
	
	overall_average_fan_outs = (double)total_number_fan_outs / (double)total_number_nets_with_instTerms;
	cout << "\t" << "Total Number Of Nets With InstTerms is "<< total_number_nets_with_instTerms << endl;
	cout << "\t" << "Total Number Of Fan-outs is "<< total_number_fan_outs << endl;
	cout << "\t" << "Overall Average Fan-outs is "<< overall_average_fan_outs << endl;	
}

		
	/*	
		1.net->getInstTerm.getCount() == 2, use getOrigin()
		getInstTerm.getInst() returns oalnst
		getInstTerm.getInst().getOrigin(oaPoint &origin)
		This function fills out 'origin' with the origin of this reference. 
		oaPoint.x(), oaPoint.y();
		*/
		
void printWireLengthDistribution(oaDesign *design)
{
	// Get the TopBlock of the current design
	oaBlock *block = design->getTopBlock();
	
	if (block) {
	
		cout << endl << "----- Cheng Guo: Problem 3 -----" << endl;


		long int number_nets_2endpoints = 0;
		long int total_HWPL = 0;
		double overall_average_HWPL;
		
		// Iterate over all nets in the design
		oaIter<oaNet>   netIterator(block->getNets());
		while (oaNet * net = netIterator.getNext()) {
		
			oaPoint origin1;
			oaPoint origin2;

			int numberOfInstTerms = (int)net->getInstTerms(oacInstTermIterNotImplicit).getCount();
			int numberOfoaTerms = (int)net->getTerms(oacTermIterNotImplicit).getCount();
			
			if(numberOfoaTerms == 1 && numberOfInstTerms == 1)
			{			
				oaBox box;			
				oaIter<oaTerm> oaTermIter(net->getTerms(oacTermIterNotImplicit));
				oaIter<oaPin> oaPinIter(oaTermIter.getNext()->getPins());
				oaIter<oaPinFig> oaPinFigIter(oaPinIter.getNext()->getFigs());
				oaPinFigIter.getNext()->getBBox(box);
				box.getCenter(origin1);
				
				cout << "\t" << "x-coor of first Term is " << origin1.x() << " "
				<< "y-coor of frist Term is " << origin1.y() << endl;
								
				oaIter<oaInstTerm>   instTermIter(net->getInstTerms(oacInstTermIterNotImplicit));
				instTermIter.getNext()->getInst()->getOrigin(origin2);
				cout << "\t" << "x-coor of sceond InstTerm is "<< origin2.x() << " "
				<< "y-coor of second InstTerm is " << origin2.y() << endl;
					
				long int HWPL = abs(origin1.x() - origin2.x()) + abs(origin1.y() - origin2.y());				
				cout << "\t" << "HWPL is " << HWPL << endl;
				number_nets_2endpoints++;
				total_HWPL += HWPL;
			}

			if(numberOfInstTerms == 2){
			
				oaIter<oaInstTerm>   instTermIter(net->getInstTerms(oacInstTermIterNotImplicit));
				instTermIter.getNext()->getInst()->getOrigin(origin1);
				cout << "\t" << "x-coor of first InstTerm is "<< origin1.x() << " "
				<< "y-coor of first InstTerm is " << origin1.y() << endl;

				instTermIter.getNext()->getInst()->getOrigin(origin2);
				cout << "\t" << "x-coor of sceond InstTerm is "<< origin2.x() << " "
				<< "y-coor of second InstTerm is " << origin2.y() << endl;
					
				long int HWPL = abs(origin1.x() - origin2.x()) + abs(origin1.y() - origin2.y());			
				cout << "\t" << "HWPL is "<< HWPL << endl;

				number_nets_2endpoints++;
				total_HWPL += HWPL;
			}

		}
				

		overall_average_HWPL = (double)total_HWPL / (double)number_nets_2endpoints;

		cout << "\t" << "Total HWPL is " << total_HWPL << endl;
		cout << "\t" << "Total number of nets with two endpoints is " << number_nets_2endpoints << endl;
		cout << "\t" << "Overall average HWPL is " << overall_average_HWPL << endl;
        }

	else {
		cout << "There is no block in this design" << endl;
	}
	
}
		
// ****************************************************************************
// main()
//
// This is the top level function that opens the design, prints library, cell,
// and view names, creates nets, and iterates the design to print the net
// names.
// ****************************************************************************
int
main(int    argc,
     char   *argv[])
{
    try {
        // Initialize OA with data model 3, since incremental technology
        // databases are supported by this application.
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, 3);

        oaString                libPath("./DesignLib");
        oaString                library("DesignLib");
        oaViewType              *viewType = oaViewType::get(oacMaskLayout);
        oaString                cell("s1196_bench");
        oaString                view("layout");
        oaScalarName            libName(ns,
                                        library);
        oaScalarName            cellName(ns,
                                         cell);
        oaScalarName            viewName(ns,
                                         view);
        oaScalarName            libraryName(ns,library);
        // Setup an instance of the oaTech conflict observer.
        opnTechConflictObserver myTechConflictObserver(1);

        // Setup an instance of the oaLibDefList observer.
        opnLibDefListObserver   myLibDefListObserver(1);

        // Read in the lib.defs file.
        oaLib *lib = oaLib::find(libraryName);

        if (!lib) {
            if (oaLib::exists(libPath)) {
                // Library does exist at this path but was not in lib.defs
                lib = oaLib::open(libraryName, libPath);
            } else {
            char *DMSystem=getenv("DMSystem");
            if(DMSystem){
                    lib = oaLib::create(libraryName, libPath, oacSharedLibMode, DMSystem);
                } else {
                    lib = oaLib::create(libraryName, libPath);
                }
            }
			 if (lib) {
                // We need to update the user's lib.def file since we either
                // found or created the library without a lib.defs reference.
                updateLibDefsFile(libraryName, libPath);
            } else {
                // Print error mesage
                cerr << "ERROR : Unable to create " << libPath << "/";
                cerr << library << endl;
                return(1);
            }
        }
        // Create the design with the specified viewType,
        // Opening it for a 'write' operation.
        cout << "The design is created and opened in 'write' mode." << endl;

        oaDesign    *design = oaDesign::open(libraryName, cellName, viewName,
                                             viewType, 'r');

        // The library, cell, and view names are printed.
        printDesignNames(design);
                  printNets(design);

        // Get the TopBlock for this design.
        oaBlock *block = design->getTopBlock();

        // If no TopBlock exist yet then create one.
        if (!block) {
            block = oaBlock::create(design);
        }


        //EE 201A Lab 1 Problem 2 starts here		
		printOverallAverageFanOut(design);
	
        //EE 201A Lab 1 Problem 3 starts here
		printWireLengthDistribution(design);

        //Output answers:
        cout << "Problem 2 -- Average fanout " << "2.10959" << endl;
        cout << "Problem 3 -- Average wirelength " << "10437 Micron" << endl;

        // The design is closed.
        design->close();

        // The library is closed.
        lib->close();

    } catch (oaCompatibilityError &ex) {
        handleFBCError(ex);
        exit(1);

    } catch (oaException &excp) {
        cout << "ERROR: " << excp.getMsg() << endl;
        exit(1);
    }

    return 0;
}



