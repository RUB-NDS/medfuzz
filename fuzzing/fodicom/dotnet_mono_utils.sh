#!/bin/bash
# 
# dotnet_mono_utils.sh
# Copyright:
#   2020,2021	Marius Merten
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# Script to determine .NET eligibility by checking the dependencies
# of dotnet and mono. If both are installed dotnet is preferred.

DNET_DEPENDENCIES=(dotnet)
MONO_DEPENDENCIES=(mono nuget msbuild)

# assume dotnet installed
NET_TOOLCHAIN="DOTNET"
for DEP in $DNET_DEPENDENCIES; do
	if ! which $DEP &>/dev/null; then
		NET_TOOLCHAIN="MONO" # get proven wrong, try mono
		break
	fi
done

# if dotnet is available skip the mono check
if [ $NET_TOOLCHAIN == "DOTNET" ]; then
	MONO_DEPENDENCIES=()
fi

for DEP in $MONO_DEPENDENCIES; do
	if ! which $DEP &>/dev/null; then
		NET_TOOLCHAIN=""
		break
	fi
done

# Override
#NET_TOOLCHAIN="MONO"

if [[ -z $NET_TOOLCHAIN ]]; then
	echo "You must install \"dotnet\" or \"mono\". If both are available dotnet is preferred." 1>&2
	exit 1
fi

# First argument should be a CSPROJ or a solution root directory to build
build() {
	BUILD_TARGET="$1"
	SOLUTION_DIRECTORY="$2"

	echo "Building $BUILD_TARGET with $NET_TOOLCHAIN"

	if [ "$NET_TOOLCHAIN" == "DOTNET" ]; then
		STATUS=0
		if [ -f $BUILD_TARGET ]; then
			dotnet build $BUILD_TARGET || STATUS=$?
		elif [ -d $BUILD_TARGET ]; then
			pushd .
			cd $BUILD_TARGET
			dotnet build || STATUS=$?
			popd
		fi
		return $STATUS
	elif [ "$NET_TOOLCHAIN" == "MONO" ]; then
		if [ ! -f $BUILD_TARGET ]; then
			echo "Building with Mono requires a specific build file, such as a csproj." 1>&2
			return
		fi
		if [ -n "$SOLUTION_DIRECTORY" ]; then
			nuget restore -SolutionDirectory $SOLUTION_DIRECTORY $BUILD_TARGET
		else
			nuget restore $BUILD_TARGET
		fi
		msbuild $BUILD_TARGET
		return
	fi
}
