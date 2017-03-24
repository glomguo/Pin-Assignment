/* EE201A Winter 2017 Course Project
 * Author: Vishesh Dokania (vdokania@ucla.edu)
 */

#ifndef VERIFY_H
#define	VERIFY_H

#include <vector>
#include <string>
#include "oaDesignDB.h"
#include "ProjectInputRules.h"

#define MICRON_TO_DBU 2000         //From Nangate Tech LEF
#define MAX_PIN_OFFSET 140         //based on Metal 6

using namespace std;
using namespace oa;

class Verify
{
public:
    Verify();
    virtual ~Verify();
	static bool NetConnectivity(oaDesign* origDesign, oaDesign* outDesign);
	static bool Macros(oaDesign* origDesign, oaDesign* outDesign);
	static bool SystemPins(oaDesign* origDesign, oaDesign* outDesign);
	static bool MacroPinPlacement(oaDesign* origDesign, oaDesign* outDesign, ProjectInputRules inputRules);
	static bool MacroPinPerturbation(oaDesign* origDesign, oaDesign* outDesign, ProjectInputRules inputRules);
    
private:

};

#endif	/* VERIFY_H */

