#!/bin/bash
# 
# get_random_unused_port.sh
# Copyright:
# 	2019,2020	Matthias Gierlings
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# This script returns a random unused port.

f_ports=$(mktemp)
seq 10000 65535 > $f_ports
echo $(ss -rtanH | cut -d':' -f2 | cut -d' ' -f1 | tr -s '\n' '\n' | sort -u  | comm -23 $f_ports - | shuf | head -n1)
rm $f_ports
