# EE201A Winter 2017 Course Project
# Author: Vishesh Dokania (vdokania@ucla.edu)

# START===============================================
set design_name "rockettile_x2"
set output_design_name "rockettile_x2_maxrule"
set libs "RocketTile_1_tile1.lib RocketTile_1_tile2.lib "
set sdc "rockettile_x2_maxrule.sdc"
set output_path "../outputs/rockettile_x2"
set logs_path "../logs/rockettile_x2"
set bottom_routing_layer 7
set top_routing_layer 8
set lef "../library/NangateOpenCellLibrary.tech.lef"
# END==================================================

# Compatible with OA Translators
set lefDefOutVersion 5.7

# User Specified CPU usage for Innovus
setMultiCpuUsage -localCpu 8

# Messages to Suppress
suppressMessage ENCLF-200 ENCLF-201 LEFPARS-2043 LEFPARS-2007 TECHLIB-459
suppressMessage IMPVL-159 TECHLIB-436 IMPLF-217 IMPLF-217 IMPEXT-2773 IMPEXT-2766 IMPEXT-2882

# default settings
set init_pwr_net VDD
set init_gnd_net VSS
set init_lef_file "$lef"
set init_design_netlisttype OA
set init_oa_design_view layout
set init_oa_design_lib DesignLib
set init_oa_design_cell "$output_design_name"

# MCMM setup
create_library_set -name TT_LIB -timing [list $libs]
create_rc_corner -name _TT_rc_corner_ -T 25.0
create_delay_corner -name TT -library_set TT_LIB -rc_corner _TT_rc_corner_
create_constraint_mode -name CON -sdc_file [list $sdc]
create_analysis_view -name TT_VIEW -delay_corner TT -constraint_mode CON
init_design -setup {TT_VIEW} -hold {TT_VIEW}

setDesignMode -process 45
setTrialRouteMode -maxRouteLayer $top_routing_layer -minRouteLayer $bottom_routing_layer

setPinAssignMode -minLayer $bottom_routing_layer -maxLayer $top_routing_layer
setNanoRouteMode -routeBottomRoutingLayer $bottom_routing_layer
setNanoRouteMode -routeTopRoutingLayer $top_routing_layer

#generateVias

# Power rings around system boundary
addRing -layer {top metal2 bottom metal2 left metal3 right metal3} -spacing {top 1 bottom 1 left 1 right 1} -width {top 1 bottom 1 left 1 right 1} -nets { VDD VSS }

# Special route for VDD VSS nets
sroute -blockPin useLef -connect { blockPin } -nets { VDD VSS } -layerChangeRange {metal2 metal3}

# Initial trial routing
trialRoute

# Routing
routeDesign

setExtractRCMode -engine postRoute -effortLevel low
extractRC

# Attempt to fix violations and reroute
editDeleteViolations
ecoRoute

# Generate reports
verifyGeometry -error 10000 -report "${logs_path}/innovus_${output_design_name}_geometry.rpt"
verifyConnectivity -error 10000 -report "${logs_path}/innovus_${output_design_name}_connectivity.rpt"
reportRoute > ${logs_path}/innovus_${output_design_name}\_routeReport.rpt
reportWire ${logs_path}/innovus_${output_design_name}\_wireReport.rpt 0
summaryReport -noHtml -outfile ${logs_path}/innovus_${output_design_name}_summaryReport.rpt

# Output final design
saveDesign ${output_path}/${output_design_name}.invs
defOut -routing ${output_path}/${output_design_name}.def
streamOut ${output_path}/${output_design_name}.gds -mapFile layer.map -libName DesignLib -units 2000 -mode ALL

exit
