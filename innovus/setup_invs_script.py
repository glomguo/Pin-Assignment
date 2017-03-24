#!/usr/bin/env python

import sys
import os
import shutil

if len(sys.argv) != 10:
	print "Incorrect number of arguments."
	print "Assumes all macro files are in the macros/ subdir inside <PATH_TO_BENCHMARK>"
	print "Assumes benchmark SDC file is at <PATH_TO_BENCHMARK>"
	exit(1)

# Parse args
template_file = sys.argv[1]
output_file = sys.argv[2]
bench_name = sys.argv[3]
bench_out_name = sys.argv[4]
input_rule_file = sys.argv[5]
macro_info_file = sys.argv[6]
bench_path = sys.argv[7]
if bench_path.endswith('/'):
	bench_path = bench_path[:-1]
output_path = sys.argv[8]
if output_path.endswith('/'):
	output_path = output_path[:-1]
logs_path = sys.argv[9]
if logs_path.endswith('/'):
	logs_path = logs_path[:-1]

# Read template data and find linenumber marked with START
template_lines = open(template_file).readlines()
insert_lineno = -1
for lineno, line in enumerate(template_lines):
	if line.startswith("# START"):
		insert_lineno = lineno+1
		break

# Enter design information
if insert_lineno != -1:
	template_lines[insert_lineno] = 'set design_name "'+bench_name+'"\n'
	template_lines[insert_lineno+1] = 'set output_design_name "'+bench_out_name+'"\n'
	
	libs = ""
	macro_lines = open(macro_info_file).readlines()
	for line in macro_lines:
		line = line.split()
		shutil.copyfile (bench_path+'/macros/'+line[0]+'.lib', line[1]+'.lib')
		os.system("sed -i 's/"+line[0]+"/"+line[1]+"/g' "+line[1]+".lib")
		libs += line[1]+'.lib '
	template_lines[insert_lineno+2] = 'set libs "'+libs+'"\n'

	shutil.copyfile( bench_path+'/'+bench_name+'.sdc', bench_out_name+'.sdc')
	os.system("sed -i 's/^current_design.*/current_design "+bench_out_name+"/' "+bench_out_name+".sdc")
	os.system("sed -i '/^set_dont_use/d' "+bench_out_name+".sdc")
	template_lines[insert_lineno+3] = 'set sdc "'+bench_out_name+'.sdc"\n'

	template_lines[insert_lineno+4] = 'set output_path "'+output_path+'"\n'
	template_lines[insert_lineno+5] = 'set logs_path "'+logs_path+'"\n'

	input_line = open(input_rule_file).readline()
	inputs = input_line.split()
	template_lines[insert_lineno+6] = 'set bottom_routing_layer '+inputs[1]+'\n'
	template_lines[insert_lineno+7] = 'set top_routing_layer '+inputs[2]+'\n'

	template_lines[insert_lineno+8] = 'set lef "../library/NangateOpenCellLibrary.tech.lef"\n'

	# Write data to output Innovus Tcl file
	template_lines = "".join(template_lines)
	open(output_file, 'w').write(template_lines)

else:
	print "Error: START position not found in template Innovus script"
