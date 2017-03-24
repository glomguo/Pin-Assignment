#######################################################
#                                                     
#  Innovus Command Logging File                     
#  Created on Mon Mar 13 01:15:27 2017                
#                                                     
#######################################################

#@(#)CDS: Innovus v16.10-p004_1 (64bit) 05/12/2016 14:48 (Linux 2.6.18-194.el5)
#@(#)CDS: NanoRoute 16.10-p004_1 NR160506-1445/16_10-UB (database version 2.30, 325.6.1) {superthreading v1.28}
#@(#)CDS: AAE 16.10-p003 (64bit) 05/12/2016 (Linux 2.6.18-194.el5)
#@(#)CDS: CTE 16.10-p002_1 () May  3 2016 03:35:25 ( )
#@(#)CDS: SYNTECH 16.10-d040_1 () Apr 22 2016 00:57:16 ( )
#@(#)CDS: CPE v16.10-p007
#@(#)CDS: IQRC/TQRC 15.2.1-s073 (64bit) Tue May  3 11:39:50 PDT 2016 (Linux 2.6.18-194.el5)

set_global _enable_mmmc_by_default_flow      $CTE::mmmc_default
suppressMessage ENCEXT-2799
set lefDefOutVersion 5.7
setMultiCpuUsage -localCpu 8
suppressMessage ENCLF-200 ENCLF-201 LEFPARS-2043 LEFPARS-2007 TECHLIB-459
suppressMessage IMPVL-159 TECHLIB-436 IMPLF-217 IMPLF-217 IMPEXT-2773 IMPEXT-2766 IMPEXT-2882
set init_pwr_net VDD
set init_gnd_net VSS
set init_lef_file ../library/NangateOpenCellLibrary.tech.lef
set init_design_netlisttype OA
set init_oa_design_view layout
set init_oa_design_lib DesignLib
set init_oa_design_cell rockettile_x2_minrule
create_library_set -name TT_LIB -timing [list $libs]
create_rc_corner -name _TT_rc_corner_ -T 25.0
create_delay_corner -name TT -library_set TT_LIB -rc_corner _TT_rc_corner_
create_constraint_mode -name CON -sdc_file [list $sdc]
create_analysis_view -name TT_VIEW -delay_corner TT -constraint_mode CON
init_design -setup TT_VIEW -hold TT_VIEW
setDesignMode -process 45
setTrialRouteMode -maxRouteLayer 9 -minRouteLayer 7
setPinAssignMode -minLayer 7 -maxLayer 9
setNanoRouteMode -routeBottomRoutingLayer 7
setNanoRouteMode -routeTopRoutingLayer 9
addRing -layer {top metal2 bottom metal2 left metal3 right metal3} -spacing {top 1 bottom 1 left 1 right 1} -width {top 1 bottom 1 left 1 right 1} -nets { VDD VSS }
sroute -blockPin useLef -connect { blockPin } -nets { VDD VSS } -layerChangeRange {metal2 metal3}
trialRoute
routeDesign
setExtractRCMode -engine postRoute -effortLevel low
extractRC
editDeleteViolations
ecoRoute
verifyGeometry -error 10000 -report ../logs/rockettile_x2/innovus_rockettile_x2_minrule_geometry.rpt
verifyConnectivity -error 10000 -report ../logs/rockettile_x2/innovus_rockettile_x2_minrule_connectivity.rpt
reportRoute > ../logs/rockettile_x2/innovus_rockettile_x2_minrule_routeReport.rpt
reportWire ../logs/rockettile_x2/innovus_rockettile_x2_minrule_wireReport.rpt 0
summaryReport -noHtml -outfile ../logs/rockettile_x2/innovus_rockettile_x2_minrule_summaryReport.rpt
saveDesign ../outputs/rockettile_x2/rockettile_x2_minrule.invs
defOut -routing ../outputs/rockettile_x2/rockettile_x2_minrule.def
streamOut ../outputs/rockettile_x2/rockettile_x2_minrule.gds -mapFile layer.map -libName DesignLib -units 2000 -mode ALL
