#!/usr/bin/env python

import sys
import os

if len(sys.argv) != 4:
	print "Usage: extract_metric_params.py <OUTPUT_DESIGN_NAME> <LOG_PATH> <METRIC_LOG_FILENAME>"
	exit(1)

# Parse args
output_bench_name = sys.argv[1]
log_path = sys.argv[2]
metric_log_filename = sys.argv[3]

metric_log = log_path+"/"+metric_log_filename
metric_log_lines = []

# ===========================================================================================================

# Max wirelength
wire_report_lines = open(log_path+"/innovus_"+output_bench_name+"_wireReport.rpt").readlines()
max_wl = 0
for line in wire_report_lines:
	line = line.split()
	if 'Total' in line:
		current_wl = float(line[2])
		if current_wl > max_wl:
			max_wl = current_wl
metric_log_lines.append("Max wirelength = "+str(max_wl)+"\n")

# Mean wirelength
num_nets = os.popen("grep 'Number of nets' "+log_path+"/innovus_"+output_bench_name+"_routeReport.rpt").read()
num_nets = float(num_nets.split()[4])
total_wl = os.popen("grep 'Total length:' "+log_path+"/innovus_"+output_bench_name+"_routeReport.rpt").read()
total_wl = float(total_wl.split()[2][:-3])
mean_wl = total_wl/num_nets
metric_log_lines.append("Mean wirelength = "+str(mean_wl)+"\n")

# Mean macro pin perturbation
mean_perturb = os.popen("grep 'Mean macro pin perturbation' "+log_path+"/PAVerify_"+output_bench_name+".log").read()
mean_perturb = float(mean_perturb.split()[4])
metric_log_lines.append("Mean macro pin perturbation = "+str(mean_perturb)+"\n")
mean_perim = os.popen("grep 'Mean macro half-perimeter' "+log_path+"/PAVerify_"+output_bench_name+".log").read()
mean_perim = float(mean_perim.split()[3])
metric_log_lines.append("Mean macro half-perimeter = "+str(mean_perim)+"\n")

# Runtime
runtime_lines = open(log_path+"/runtime_"+output_bench_name+".log").readlines()
invs_runtime = 0; paverify_runtime = 0; total_runtime = 0
for i, line in enumerate(runtime_lines):
	line = line.split()
	if 'real' in line:
		time = line[1].split('m')
		time = float(time[0])*60 + float(time[1][:-1])
		if 'Innovus:' in runtime_lines[i-2].split():
			invs_runtime += time 
		elif 'PAVerify:' in runtime_lines[i-2].split():
			paverify_runtime += time 
		else:
			total_runtime = time
metric_runtime = total_runtime - invs_runtime - paverify_runtime
metric_log_lines.append("Runtime = "+str(metric_runtime)+"\n")

metric_log_lines.append("================================================\n")

# Innovus DRC and LVS check
c = 1
lvs = os.popen("grep 'Found no problems or warnings' "+log_path+"/innovus_"+output_bench_name+"_connectivity.rpt").read()
drc = os.popen("grep 'No DRC violations were found' "+log_path+"/innovus_"+output_bench_name+"_geometry.rpt").read()
verify_out = os.popen("grep 'MIN INNOVUS REQUIREMENTS' "+log_path+"/PAVerify_"+output_bench_name+".summary").read()
verify_out = verify_out.split()[3]

if (not lvs or not drc or verify_out != "MET"):
	c = 0
metric_log_lines.append("Innovus DRC/LVS constraint c = "+str(c)+"\n")

# MinPitch and MaxPerturb constraints
pmin = os.popen("grep 'pmin' "+log_path+"/PAVerify_"+output_bench_name+".summary").read()
pmax = os.popen("grep 'pmax' "+log_path+"/PAVerify_"+output_bench_name+".summary").read()
metric_log_lines.append("Min pitch constraint "+pmin)
metric_log_lines.append("Max perturbation constraint "+pmax)

# ===========================================================================================================
metric_log_lines = "".join(metric_log_lines)
open(metric_log, 'w').write(metric_log_lines)
