#!/bin/bash
#
# afl_setup.sh
# Copyright:
#   2019   Matthias Gierlings
#
# This software is released under the Simplified BSD License
# (see license.txt)
#
# Sets kernel and system parameters for usage of AFL.

echo core >/proc/sys/kernel/core_pattern
if [ -f "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor" ]; then
    echo performance | tee "/sys/devices/system/cpu/cpu*/cpufreq/scaling_governor"
fi

