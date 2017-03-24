#!/bin/bash

rm -f run_innovus.tcl
rm -rf timingReports
rm -f *.cmd*
rm -f *.log*
rm -f *.sdc
mv cds.lib cds.lib.bak
rm -f *.lib
mv cds.lib.bak cds.lib
