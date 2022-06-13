#!/bin/bash
# 
# instrument_dll.sh
# Copyright:
#   2020,2021	Marius Merten
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# This script checks the requirements for a sane afl-fuzz process
# for the fo-dicom library.

source dotnet_mono_utils.sh

function err() {
	echo "$@" >&2
}

SHARPFUZZ_GIT_URL="https://github.com/Metalnem/sharpfuzz.git"
SHARPFUZZ_DIR="$(realpath sharpfuzz)/"

# Input handling
if [ $# -ne 1 ]; then
	err "Usage: ./instrument_dll.sh <dll>"
	exit 1
fi

TARGET_DLL=$(realpath $1)
if [ ! -f "$TARGET_DLL" ]; then
	err "Specified target dll \"$TARGET_DLL\" does not exist."
	exit 1
fi

# Clone if necessary
if [ ! -d $SHARPFUZZ_DIR ]; then
	git clone $SHARPFUZZ_GIT_URL $SHARPFUZZ_DIR
fi

# Build SharpFuzz
pushd .
cd $SHARPFUZZ_DIR
ls -1 src/*/*.csproj | xargs sed -i "s/;netcoreapp5.0//g"

if [ "$NET_TOOLCHAIN" == "DOTNET" ]; then
	dotnet build
else
	ls -1 src/*/*.csproj | xargs -I"{}" nuget restore "{}"
	msbuild
fi
popd

# Instrument by executing SharpFuzz CommandLine on DLL
SHARPFUZZ_CL_DLL="${SHARPFUZZ_DIR}src/SharpFuzz.CommandLine/bin/Debug/netcoreapp3.1/SharpFuzz.CommandLine.dll"
if [ ! -f "$SHARPFUZZ_CL_DLL" ]; then
	err "SharpFuzz CommandLine application unavailable. Cannot instrument without it."
	exit 1
fi

echo ""
echo "Instrumenting $TARGET_DLL"
echo ""
if [ "$NET_TOOLCHAIN" == "DOTNET" ]; then
    dotnet $SHARPFUZZ_CL_DLL $TARGET_DLL
else
    mono $SHARPFUZZ_CL_DLL $TARGET_DLL
fi

