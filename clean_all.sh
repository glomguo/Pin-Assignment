#!/bin/bash

rm -f *.log
rm -rf logs
rm -rf outputs
rm -rf DesignLib
rm -f lib.defs
cd innovus
./clean_invs.sh
cd ..
