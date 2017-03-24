/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#include "Verify.h"
#include "InputOutputHandler.h"
#include "OAHelper.h"
//#include "ProjectInputRules.h"
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;
static oaNativeNS ns;

Verify::Verify()
{
}

Verify::~Verify()
{
}

/*
 */
bool Verify::NetConnectivity(oaDesign* origDesign, oaDesign* outDesign)
{
	oaBlock* origBlock = InputOutputHandler::ReadTopBlock(origDesign);
	oaBlock* outBlock = InputOutputHandler::ReadTopBlock(outDesign);

	map <oaString, vector<oaString> > netTerms;
	map <oaString, vector<oaString> > netInstTerms;

	// Populate netTerms and netInstTerms maps from original design block
	oaString netName, assocTermName, instName, masterCellName, termName;
	oaIter<oaNet> netIterator(origBlock->getNets());
	while (oaNet* net = netIterator.getNext()) {
		net->getName(ns, netName);
		oaIter<oaInstTerm> instTermIterator(net->getInstTerms());
		oaIter<oaTerm> termIterator(net->getTerms());
		
		//InstTerms
		while (oaInstTerm* instTerm = instTermIterator.getNext()) {
			instTerm->getTermName(ns, assocTermName);
			oaInst* inst = instTerm->getInst();
			inst->getName(ns, instName);
			inst->getCellName(ns, masterCellName);
			netInstTerms[netName].push_back( masterCellName + "_" + instName + "/" + assocTermName );
		}
		
		//Terms
		while (oaTerm* term = termIterator.getNext()) {
			term->getName(ns,termName);
			netTerms[netName].push_back(termName);
		}
	}

	//Print original design connectivity
	cout << "ORIGINAL DESIGN: " << endl;
	cout << "Net terms" << endl;
	for (map<oaString,vector<oaString> >::iterator it = netTerms.begin(); it!= netTerms.end(); it++) {
		cout << "    (*) " << it->first << endl;
		for (vector<oaString>::iterator it2 = it->second.begin(); it2!= it->second.end(); it2++)
			cout << "         => " << *it2 << endl;
	}
	cout << "Net instTerms" << endl;
	for (map<oaString,vector<oaString> >::iterator it = netInstTerms.begin(); it!= netInstTerms.end(); it++) {
		cout << "    (*) " << it->first << endl;
		for (vector<oaString>::iterator it2 = it->second.begin(); it2!= it->second.end(); it2++)
			cout << "         => " << *it2 << endl;
	}

	//======================================================================================================

	//Populate vector of net names in output design
	bool violationFlag = false;
	vector <oaString> outputNetNames;
	oaString netName2, assocTermName2, instName2, masterCellName2, termName2;
	oaIter<oaNet> netIterator2(outBlock->getNets());
	while (oaNet* net2 = netIterator2.getNext()) {
		net2->getName(ns, netName2);
		outputNetNames.push_back(netName2);
	}

	//Check that all original nets exist in output design
	cout << endl << "OUTPUT DESIGN: " << endl;
	//No need to check netTerms nets, already contained in netInstTerms nets
	for (map<oaString,vector<oaString> >::iterator it = netInstTerms.begin(); it!= netInstTerms.end(); it++) {
		if (find (outputNetNames.begin(), outputNetNames.end(), it->first) == outputNetNames.end() ) {
			violationFlag = true;
			cout << "Connectivity violation! Net: " << it->first << " not found in output design!" << endl; 
		}
	}

	//Check output design connectivity
	netIterator2.reset();
	while (oaNet* net2 = netIterator2.getNext()) {
		net2->getName(ns, netName2);
		oaIter<oaInstTerm> instTermIterator2(net2->getInstTerms());
		oaIter<oaTerm> termIterator2(net2->getTerms());
		
		//InstTerms
		while (oaInstTerm* instTerm2 = instTermIterator2.getNext()) {
			instTerm2->getTermName(ns, assocTermName2);
			oaInst* inst2 = instTerm2->getInst();
			inst2->getName(ns, instName2);
			inst2->getCellName(ns, masterCellName2);
			if (netInstTerms.find(netName2) == netInstTerms.end()){
				violationFlag = true;
				cout << "Violation! Net " << netName2 << " not found in original design instTerms list. " << endl;
			}
			else if (find (netInstTerms[netName2].begin(), netInstTerms[netName2].end(), 
						   masterCellName2+"/"+assocTermName2)
					  == netInstTerms[netName2].end() ) {
				violationFlag = true;
				cout << "Connectivity violation! Net: " << netName2 << "; Incorrect instTerm: "
					 << masterCellName2 + "_" + instName2 + "/" + assocTermName2 << endl; 
			}
		}
		
		//Terms
		while (oaTerm* term2 = termIterator2.getNext()) {
			term2->getName(ns,termName2);
			if (netTerms.find(netName2) == netTerms.end()){
				violationFlag = true;
				cout << "Violation! Net " << netName2 << " not found in original design terms list. " << endl;
			}
			else if (find (netTerms[netName2].begin(), netTerms[netName2].end(), termName2)
					  == netTerms[netName2].end() ) {
				violationFlag = true;
				cout << "Connectivity violation! Net: " << netName2 << "; Incorrect term: " 
					 << termName2 << endl; 
			}
		}
	}
	if (!violationFlag)
		cout << "Connectivity clean!" << endl;
	else
		cout << "Connectivity violations found!" << endl;

	return violationFlag;
}


/*
 */
bool Verify::Macros(oaDesign* origDesign, oaDesign* outDesign)
{
	oaBlock* origBlock = InputOutputHandler::ReadTopBlock(origDesign);
	oaBlock* outBlock = InputOutputHandler::ReadTopBlock(outDesign);

	oaString instName, instName2, masterCellName, masterCellName2;
	oaPoint instOrigin, instOrigin2;
	oaOrient instOrient, instOrient2;

	map <oaString, pair<oaPoint, oaOrient> > instPlacement;

	// Populate instOrigins and instOrients maps from original design block
	oaIter<oaInst> instIterator(origBlock->getInsts());
	while (oaInst* inst = instIterator.getNext()) {
		inst->getName(ns, instName);
		inst->getCellName(ns, masterCellName);
		inst->getOrigin(instOrigin);
		instOrient = inst->getOrient();

		instPlacement[masterCellName+"_"+instName] = make_pair(instOrigin, instOrient);
	}

	//Print original design macro information
	cout << "ORIGINAL DESIGN: " << endl;
	for (map<oaString,pair<oaPoint,oaOrient> >::iterator it = instPlacement.begin(); it!= instPlacement.end(); it++) {
		cout << "    (*) " << it->first << endl;
		cout << "         => Origin: " << it->second.first.x() << "," << it->second.first.y() << endl;
		cout << "         => Orient: " << it->second.second << endl;
	}

	//======================================================================================================

	//Populate vector of inst names in output design
	bool violationFlag = false;
	vector <oaString> outputInstNames;
	oaIter<oaInst> instIterator2(outBlock->getInsts());
	while (oaInst* inst2 = instIterator2.getNext()) {
		inst2->getCellName(ns, masterCellName2);
		outputInstNames.push_back(masterCellName2);
	}

	//Check that all original insts exist in output design
	cout << endl << "OUTPUT DESIGN: " << endl;
	for (map<oaString,pair<oaPoint,oaOrient> >::iterator it = instPlacement.begin(); it!= instPlacement.end(); it++) {
		if (find (outputInstNames.begin(), outputInstNames.end(), it->first) == outputInstNames.end() ) {
			violationFlag = true;
			cout << "Macro violation! Inst: " << it->first << " not found in output design!" << endl; 
		}
	}

	//Check output macro origin/orientations
	instIterator2.reset();
	while (oaInst* inst2 = instIterator2.getNext()) {
		inst2->getCellName(ns, masterCellName2);
		inst2->getOrigin(instOrigin2);
		instOrient2 = inst2->getOrient();
	
		if (instPlacement.find(masterCellName2) == instPlacement.end()){
			violationFlag = true;
			cout << "Violation! Inst " << masterCellName2 << " not found in original design. " << endl;
		}
		else if ( instOrigin2 != instPlacement[masterCellName2].first || 
				  instOrient2 != instPlacement[masterCellName2].second   ) {
			violationFlag = true;
			cout << "Macro violation! Macro: " << masterCellName2 << "; Mismatch in origin or orientation! " << endl; 
		}
	}

	if (!violationFlag)
		cout << "Macro placement/orientation clean!" << endl;
	else
		cout << "Macro placement/orientation violations found!" << endl;

	return violationFlag;
}


/*
 */
bool Verify::SystemPins(oaDesign* origDesign, oaDesign* outDesign)
{
	oaBlock* origBlock = InputOutputHandler::ReadTopBlock(origDesign);
	oaBlock* outBlock = InputOutputHandler::ReadTopBlock(outDesign);

	oaString termName;
	oaPoint termPos, termPos2;
	bool violationFlag = false;

	oaIter<oaTerm> termIterator(origBlock->getTerms());
	while (oaTerm* term = termIterator.getNext()) {
		term->getName(ns,termName);
		termPos = OAHelper::GetTermPosition(term);

		oaTerm* outTerm = oaTerm::find(outBlock, oaName(ns, termName));
		if (outTerm == NULL){
			violationFlag = true;
			cout << "System pin violation! Pin: " << termName << " not found in output design!" << endl; 
		}
		else {
			termPos2 = OAHelper::GetTermPosition(outTerm);
			if (termPos != termPos2){
				violationFlag = true;
				cout << "System pin violation! Pin: " << termName << "; Location mismatch! " << endl; 
			}
		}

	}

	if (!violationFlag)
		cout << "System pin placement clean!" << endl;
	else
		cout << "System pin placement violations found!" << endl;

	return violationFlag;
}


/*
 */
bool Verify::MacroPinPlacement(oaDesign* origDesign, oaDesign* outDesign, ProjectInputRules inputRules)
{
	oaBlock* origBlock = InputOutputHandler::ReadTopBlock(origDesign);
	oaBlock* outBlock = InputOutputHandler::ReadTopBlock(outDesign);

	oaString instName, masterCellName, outMasterCellName, assocTermName, otherAssocTermName;
	oaPoint instTermPos, outInstTermPos, otherInstTermPos;
	oaBox instBBox, outInstBBox;
	oaInt4 boxTop, boxBottom, boxLeft, boxRight;
	oaUInt4 boxHeight, boxWidth;
	bool violationFlag = false;

	float step = inputRules.getPinMoveStep() *MICRON_TO_DBU;
	float minPinPitch = inputRules.getMinPinPitch() *MICRON_TO_DBU;
	//cout << "Step: " << step << endl;

	//Build legal pin position table from original design block
	oaIter<oaInst> instIterator(origBlock->getInsts());
	while (oaInst* inst = instIterator.getNext()) {
		vector <oaPoint> topEdgeLocs, bottomEdgeLocs, leftEdgeLocs, rightEdgeLocs;

		inst->getName(ns, instName);
		inst->getCellName(ns, masterCellName);

		//cout << masterCellName+"_"+instName << endl;

		inst->getBBox(instBBox);
		boxTop = instBBox.top(); boxBottom = instBBox.bottom();
		boxLeft = instBBox.left(); boxRight = instBBox.right();
		boxHeight = instBBox.getHeight(); boxWidth = instBBox.getWidth();
		//cout << "Top: " << boxTop << endl;
		//cout << "Bottom: " << boxBottom << endl;
		//cout << "Left: " << boxLeft << endl;
		//cout << "Right: " << boxRight << endl;
		//cout << "Height: " << boxHeight << endl;
		//cout << "Width: " << boxWidth << endl;

		//Vert edge = left/right. Horz edge = top/bottom.
		//NOTE: Value MAX_PIN_OFFSET according to techLEF metal6 track width
		bool foundVertEdgePin = false, foundHorzEdgePin = false;
		oaPoint vertEdgePinPos, horzEdgePinPos;
		oaIter<oaInstTerm> instTermIterator(inst->getInstTerms());
		oaInstTerm* instTerm;
		while ( (instTerm = instTermIterator.getNext()) &&
				!(foundVertEdgePin && foundHorzEdgePin)) {
			instTermPos = OAHelper::GetAbsoluteInstTermPosition(instTerm);
			//Find horz and vert pins that are NOT near corners
			if ( ( abs(instTermPos.x()-boxLeft) <= MAX_PIN_OFFSET || 
				   abs(instTermPos.x()-boxRight) <= MAX_PIN_OFFSET ) &&
			     ( abs(instTermPos.y()-boxTop) > MAX_PIN_OFFSET && 
				   abs(instTermPos.y()-boxBottom) > MAX_PIN_OFFSET ) ) {
				vertEdgePinPos = instTermPos;
				foundVertEdgePin = true;
			}
			else if ( ( abs(instTermPos.x()-boxLeft) > MAX_PIN_OFFSET && 
						abs(instTermPos.x()-boxRight) > MAX_PIN_OFFSET ) &&
					  ( abs(instTermPos.y()-boxTop) <= MAX_PIN_OFFSET || 
						abs(instTermPos.y()-boxBottom) <= MAX_PIN_OFFSET ) ) {
				horzEdgePinPos = instTermPos;
				foundHorzEdgePin = true;
			}
		}

		if (!foundVertEdgePin || !foundHorzEdgePin) {
			cout << "Something went wrong looking for starter pins!" << endl;
			return true;
		}
		//cout << "Vert starter pin: " << vertEdgePinPos.x() << "," << vertEdgePinPos.y() << endl;
		//cout << "Horz starter pin: " << horzEdgePinPos.x() << "," << horzEdgePinPos.y() << endl;

		//Left and Right Edge Locations
		if (abs(vertEdgePinPos.x()-boxLeft) <= MAX_PIN_OFFSET){  //Left edge
			oaInt4 pinOffset = abs(vertEdgePinPos.x()-boxLeft);

			leftEdgeLocs.push_back(vertEdgePinPos);
			oaPoint curr(0,0);
			curr +=(vertEdgePinPos);
			while (curr.y()-step >= boxBottom) { //Traverse down
				curr.set(vertEdgePinPos.x(), curr.y()-step);
				leftEdgeLocs.push_back(curr);
			}
			curr.set(0,0);
			curr +=(vertEdgePinPos);
			while (curr.y()+step <= boxTop) { //Traverse up
				curr.set(vertEdgePinPos.x(), curr.y()+step);
				leftEdgeLocs.push_back(curr);
			}

			//Project to right edge
			for (vector<oaPoint>::iterator it = leftEdgeLocs.begin(); it!= leftEdgeLocs.end(); it++) {
				oaPoint tmp(0,0);
				tmp +=(*it);
				tmp.set( (tmp.x()+boxWidth-2*pinOffset), tmp.y() );
				rightEdgeLocs.push_back(tmp);
			}
		}
		else if (abs(vertEdgePinPos.x()-boxRight) <= MAX_PIN_OFFSET){  //Right edge
			oaInt4 pinOffset = abs(vertEdgePinPos.x()-boxRight);

			rightEdgeLocs.push_back(vertEdgePinPos);
			oaPoint curr(0,0);
			curr +=(vertEdgePinPos);
			while (curr.y()-step >= boxBottom) { //Traverse down
				curr.set(vertEdgePinPos.x(), curr.y()-step);
				rightEdgeLocs.push_back(curr);
			}
			curr.set(0,0);
			curr +=(vertEdgePinPos);
			while (curr.y()+step <= boxTop) { //Traverse up
				curr.set(vertEdgePinPos.x(), curr.y()+step);
				rightEdgeLocs.push_back(curr);
			}

			//Project to left edge
			for (vector<oaPoint>::iterator it = rightEdgeLocs.begin(); it!= rightEdgeLocs.end(); it++) {
				oaPoint tmp(0,0);
				tmp +=(*it);
				tmp.set( (tmp.x()-boxWidth+2*pinOffset), tmp.y() );
				leftEdgeLocs.push_back(tmp);
			}
		}

		//Top and Bottom Edge Locations
		if (abs(horzEdgePinPos.y()-boxTop) <= MAX_PIN_OFFSET){  //Top edge
			oaInt4 pinOffset = abs(horzEdgePinPos.y()-boxTop);

			topEdgeLocs.push_back(horzEdgePinPos);
			oaPoint curr(0,0);
			curr +=(horzEdgePinPos);
			while (curr.x()-step >= boxLeft) { //Traverse left
				curr.set(curr.x()-step, horzEdgePinPos.y());
				topEdgeLocs.push_back(curr);
			}
			curr.set(0,0);
			curr +=(horzEdgePinPos);
			while (curr.x()+step <= boxRight) { //Traverse right
				curr.set(curr.x()+step, horzEdgePinPos.y());
				topEdgeLocs.push_back(curr);
			}

			//Project to bottom edge
			for (vector<oaPoint>::iterator it = topEdgeLocs.begin(); it!= topEdgeLocs.end(); it++) {
				oaPoint tmp(0,0);
				tmp +=(*it);
				tmp.set( tmp.x(), tmp.y()-boxHeight+2*pinOffset );
				bottomEdgeLocs.push_back(tmp);
			}
		}
		else if (abs(horzEdgePinPos.y()-boxBottom) <= MAX_PIN_OFFSET){  //Bottom edge
			oaInt4 pinOffset = abs(horzEdgePinPos.y()-boxBottom);

			bottomEdgeLocs.push_back(horzEdgePinPos);
			oaPoint curr(0,0);
			curr +=(horzEdgePinPos);
			while (curr.x()-step >= boxLeft) { //Traverse left
				curr.set(curr.x()-step, horzEdgePinPos.y());
				bottomEdgeLocs.push_back(curr);
			}
			curr.set(0,0);
			curr +=(horzEdgePinPos);
			while (curr.x()+step <= boxRight) { //Traverse right
				curr.set(curr.x()+step, horzEdgePinPos.y());
				bottomEdgeLocs.push_back(curr);
			}

			//Project to top edge
			for (vector<oaPoint>::iterator it = bottomEdgeLocs.begin(); it!= bottomEdgeLocs.end(); it++) {
				oaPoint tmp(0,0);
				tmp +=(*it);
				tmp.set( tmp.x(), tmp.y()+boxHeight-2*pinOffset );
				topEdgeLocs.push_back(tmp);
			}
		}

		//Print legal pin position table
		//cout << "Left edge legal locations: " <<endl;
		//for (vector<oaPoint>::iterator it = leftEdgeLocs.begin(); it!= leftEdgeLocs.end(); it++)
		//	cout << "\t(" << (*it).x() << "," << (*it).y() << ")" << endl;
		//cout << "Right edge legal locations: " <<endl;
		//for (vector<oaPoint>::iterator it = rightEdgeLocs.begin(); it!= rightEdgeLocs.end(); it++)
		//	cout << "\t(" << (*it).x() << "," << (*it).y() << ")" << endl;
		//cout << "Top edge legal locations: " <<endl;
		//for (vector<oaPoint>::iterator it = topEdgeLocs.begin(); it!= topEdgeLocs.end(); it++)
		//	cout << "\t(" << (*it).x() << "," << (*it).y() << ")" << endl;
		//cout << "Bottom edge legal locations: " <<endl;
		//for (vector<oaPoint>::iterator it = bottomEdgeLocs.begin(); it!= bottomEdgeLocs.end(); it++)
		//	cout << "\t(" << (*it).x() << "," << (*it).y() << ")" << endl;
		
		//=================================================================================================
		//Match modified pin locations with legal position table
		
		bool foundOutInst = false;
		oaIter<oaInst> instIterator2(outBlock->getInsts());
		while (oaInst* outInst = instIterator2.getNext()) {
			outInst->getCellName(ns, outMasterCellName);
			if (outMasterCellName == masterCellName+"_"+instName) {
				foundOutInst = true;
				outInst->getBBox(outInstBBox);
				boxTop = outInstBBox.top(); boxBottom = outInstBBox.bottom();
				boxLeft = outInstBBox.left(); boxRight = outInstBBox.right();
				boxHeight = outInstBBox.getHeight(); boxWidth = outInstBBox.getWidth();

				//Iterate through all pins, verify legal location and min pitch
				oaIter<oaInstTerm> instTermIterator2(outInst->getInstTerms());
				oaIter<oaInstTerm> instTermIterator3(outInst->getInstTerms());
				while ( oaInstTerm* outInstTerm = instTermIterator2.getNext() ) {
					outInstTermPos = OAHelper::GetAbsoluteInstTermPosition(outInstTerm);
					outInstTerm->getTermName(ns, assocTermName);

					if (assocTermName == "VDD" || assocTermName == "VSS") { continue; }

					//Match with legal locations
					bool foundMatchingLoc = false;
					for (vector<oaPoint>::iterator it = leftEdgeLocs.begin();
						                      it!= leftEdgeLocs.end(); it++) {
						if (*it==outInstTermPos) { 
							foundMatchingLoc = true;
							//cout << "Matched: (" << (*it).x() << "," << (*it).y() << ")" << endl;
							break;
						}
					}
					for (vector<oaPoint>::iterator it = rightEdgeLocs.begin();
						                      it!= rightEdgeLocs.end() && !foundMatchingLoc; it++) {
						if (*it==outInstTermPos) { 
							foundMatchingLoc = true;
							//cout << "Matched: (" << (*it).x() << "," << (*it).y() << ")" << endl;
							break;
						}
					}
					for (vector<oaPoint>::iterator it = topEdgeLocs.begin();
						                      it!= topEdgeLocs.end() && !foundMatchingLoc; it++) {
						if (*it==outInstTermPos) { 
							foundMatchingLoc = true;
							//cout << "Matched: (" << (*it).x() << "," << (*it).y() << ")" << endl;
							break;
						}
					}
					for (vector<oaPoint>::iterator it = bottomEdgeLocs.begin();
						                      it!= bottomEdgeLocs.end() && !foundMatchingLoc; it++) {
						if (*it==outInstTermPos) { 
							foundMatchingLoc = true;
							//cout << "Matched: (" << (*it).x() << "," << (*it).y() << ")" << endl;
							break;
						}
					}
					if (!foundMatchingLoc) {
						violationFlag = true;
						cout << "Macro pin violation! Macro: " << outMasterCellName << 
							    "; Pin: " << assocTermName << "; Location: (" <<
								outInstTermPos.x() << "," << outInstTermPos.y() <<
								") does not occupy a legal position!" << endl; 
					}

					//Verify minimum pin pitch
					//NOTE: Inefficient O(n^2) implementation, okay for small #pins
						//Can sort pin locations along edge and compare consecutive - O(nlogn)
					instTermIterator3.reset();
					while ( oaInstTerm* otherInstTerm = instTermIterator3.getNext() ) {
						otherInstTerm->getTermName(ns, otherAssocTermName);
						otherInstTermPos = OAHelper::GetAbsoluteInstTermPosition(otherInstTerm);

						if ( otherInstTerm!=outInstTerm && 
							 otherAssocTermName!="VDD" && otherAssocTermName!="VSS" &&
							 ( abs(otherInstTermPos.x()-outInstTermPos.x())
							 + abs(otherInstTermPos.y()-outInstTermPos.y()) ) < minPinPitch ) { 
							violationFlag = true;
							cout << "Macro minimum pin pitch violation! Macro: " << outMasterCellName << endl;
							cout << "\tPin1: " << assocTermName << "; Location: (" <<
									outInstTermPos.x() << "," << outInstTermPos.y() << ")" << endl;
							cout << "\tPin2: " << otherAssocTermName << "; Location: (" <<
									otherInstTermPos.x() << "," << otherInstTermPos.y() << ")" << endl;
						}
					}
				}
				break;  //Out of inst iteration
			}
		}
		
		if (!foundOutInst){
			violationFlag = true;
			cout << "Macro violation! Macro: " << masterCellName+"_"+instName << 
				    " not found in output design!" << endl; 
		}
	}


	if (!violationFlag)
		cout << "Macro pin placement (legal location/min pitch) clean!" << endl;
	else
		cout << "Macro pin placement (legal location/min pitch) violations found!" << endl;

	return violationFlag;
}


/*
 */
bool Verify::MacroPinPerturbation(oaDesign* origDesign, oaDesign* outDesign, ProjectInputRules inputRules)
{
	oaBlock* origBlock = InputOutputHandler::ReadTopBlock(origDesign);
	oaBlock* outBlock = InputOutputHandler::ReadTopBlock(outDesign);

	oaString instName, masterCellName, outMasterCellName, assocTermName, otherAssocTermName;
	oaPoint instTermPos, outInstTermPos;
	bool violationFlag = false;
	oaInt4 currentPinPerturb, totalPinPerturb=0;
	oaUInt4 boxHeight, boxWidth, totalMacroHalfPerimeter=0, numMacros=0, numPins=0;
	oaBox instBBox;

	float maxPinPerturb = inputRules.getMaxPinPerturbation() * MICRON_TO_DBU;
	bool infPerturbAllowed = false;
	cout << "Max Pin Perturb (per pin): " << maxPinPerturb << endl;
	if (maxPinPerturb < 0){  //No need to check if Infinite perturbation allowed
		cout << "Infinite perturbation allowed." << endl;
		infPerturbAllowed = true;
	}

	oaIter<oaInst> instIterator(origBlock->getInsts());
	oaIter<oaInst> instIterator2(outBlock->getInsts());

	while (oaInst* inst = instIterator.getNext()) {
		numMacros++;

		inst->getBBox(instBBox);
		boxHeight = instBBox.getHeight(); boxWidth = instBBox.getWidth();
		totalMacroHalfPerimeter += (boxHeight + boxWidth);

		inst->getName(ns, instName);
		inst->getCellName(ns, masterCellName);

		instIterator2.reset();
		bool foundOutInst = false;
		oaInst* outInst;
		while (outInst = instIterator2.getNext()) {
			outInst->getCellName(ns, outMasterCellName);
			if (outMasterCellName == masterCellName+"_"+instName) {
				foundOutInst = true;
				break;
			}
		}
		
		if (!foundOutInst){
			violationFlag = true;
			cout << "Macro violation! Macro: " << masterCellName+"_"+instName << 
					" not found in output design!" << endl; 
		}
		else {
			oaIter<oaInstTerm> instTermIterator(inst->getInstTerms());
			while ( oaInstTerm* instTerm = instTermIterator.getNext() ) {
				numPins++;

				instTerm->getTermName(ns, assocTermName);
				instTermPos = OAHelper::GetAbsoluteInstTermPosition(instTerm);
				
				oaInstTerm* outInstTerm = oaInstTerm::find(outInst, oaName(ns, assocTermName));
				if (outInstTerm == NULL){
					violationFlag = true;
					cout << "Macro pin violation! Macro: " << outMasterCellName << 
							"; Pin: " << assocTermName << " not found in output design!" << endl; 
				}
				else {
					outInstTermPos = OAHelper::GetAbsoluteInstTermPosition(outInstTerm);
					currentPinPerturb = abs(instTermPos.x() - outInstTermPos.x()) +
											 abs(instTermPos.y() - outInstTermPos.y()); 
					totalPinPerturb += currentPinPerturb;

					if (!infPerturbAllowed && currentPinPerturb > maxPinPerturb){
						violationFlag = true;
						cout << "Max perturbation violation! Macro: " << outMasterCellName << 
								"; Pin: " << assocTermName << 
								"; Perturbation: " << currentPinPerturb << endl; 
					}
				}
			}
		}
	}

	cout << endl << "Mean macro pin perturbation: " << totalPinPerturb/numPins << endl;
	cout << "Mean macro half-perimeter: " << totalMacroHalfPerimeter/numMacros << endl << endl;

	if (!violationFlag)
		cout << "Macro pin perturbation clean!" << endl;
	else
		cout << "Macro pin perturbation violations found!" << endl;

	return violationFlag;
}
