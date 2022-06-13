#!/bin/bash
#
# purge_dicom_toolkit.sh
# Copyright:
#   2020   Matthias Gierlings
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# Forcefully removes the DCMTK source code and all DCMTK builds.

rm -rf dcmtk
rm -rf dcmtk_coverage
rm -rf dcmtk_debug
rm -rf dcmtk_nosanitizers
rm -rf dcmtk_sanitizers
