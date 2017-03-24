if {![namespace exists ::IMEX]} { namespace eval ::IMEX {} }
set ::IMEX::dataVar [file dirname [file normalize [info script]]]
set ::IMEX::libVar ${::IMEX::dataVar}/libs

create_library_set -name TT_LIB\
   -timing\
    [list [list ${::IMEX::libVar}/mmmc/RocketTile_1_tile1.lib\
          ${::IMEX::libVar}/mmmc/RocketTile_1_tile2.lib]]
create_rc_corner -name _TT_rc_corner_\
   -preRoute_res 1\
   -postRoute_res 1\
   -preRoute_cap 1\
   -postRoute_cap 1\
   -postRoute_xcap 1\
   -preRoute_clkres 0\
   -preRoute_clkcap 0\
   -T 25
create_delay_corner -name TT\
   -library_set TT_LIB\
   -rc_corner _TT_rc_corner_
create_constraint_mode -name CON\
   -sdc_files\
    [list ${::IMEX::libVar}/mmmc/rockettile_x2_minrule.sdc]
create_analysis_view -name TT_VIEW -constraint_mode CON -delay_corner TT
set_analysis_view -setup [list TT_VIEW] -hold [list TT_VIEW]
