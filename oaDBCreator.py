#!/usr/bin/env python

import sys
import os

if len(sys.argv) != 3:
	print "Usage: oaDBCreator.py <PATH_TO_BENCHMARK> <BENCHMARK_NAME>"
	print "Assumes all macro LEFs are in the macros/ subdir inside <PATH_TO_BENCHMARK>"
	exit(1)

# Parse args
bench_path = sys.argv[1]
if bench_path.endswith('/'):
	bench_path = bench_path[:-1]
bench_name = sys.argv[2]

# Translate Tech LEF to OA
os.system("lef2oa -lib DesignLib -lef library/NangateOpenCellLibrary.tech.lef")

# Translate all Macro LEFs to OA
macro_files = os.listdir(bench_path+"/macros/")
for file in macro_files:
	if file.endswith(".lef"):
		os.system("lef2oa -lib DesignLib -lef "+bench_path+"/macros/"+file)

# Translate system design DEF to OA
os.system("def2oa -lib DesignLib -def "+bench_path+"/"+bench_name+".def")
