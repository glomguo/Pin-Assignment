/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#ifndef INPUTOUTPUTHANDLER_H
#define	INPUTOUTPUTHANDLER_H

#include <vector>
#include <string>
#include "oaDesignDB.h"
//#include "Connection.h"
using namespace std;
using namespace oa;

struct DesignInfo
{
    string libPath;
    string libName;
    string designName;
    string outputDesignName;
    string designView; 
    string inputRuleFileName;
    string summaryLogFileName;
};

class InputOutputHandler
{
public:
    InputOutputHandler();
    virtual ~InputOutputHandler();
	static void ReadInputArguments(char* argv[], DesignInfo& designInfo);
    static oaDesign* ReadOALibrary(oaLib* lib, DesignInfo designInfo);
	static oaBlock* ReadTopBlock(oaDesign* design);

    
private:

};

#endif	/* INPUTOUTPUTHANDLER_H */

