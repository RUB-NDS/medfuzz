#!/bin/bash
# 
# prober.sh
# Copyright:
#   2020,2021	Marius Merten
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# Clones and builds fo-dicom and creates a C# project which
# references the instrumented fo-dicom dll and also builds the latter
# project. The final build process should generate a fuzzable .net application.

source dotnet_mono_utils.sh

FUZZ_TEMPLATE_DIR="$(realpath fo-fuzz_template)/"

FUZZ_PROJ="fo-fuzz"
FUZZ_PROJ_DIR="$(realpath $FUZZ_PROJ)/"
FUZZ_CSPROJ="${FUZZ_PROJ_DIR}${FUZZ_PROJ}.csproj"

FO_DICOM_GIT="https://github.com/fo-dicom/fo-dicom.git"
FO_DICOM_DIR="$(realpath fo-dicom/)/"

#FCORE_CSPROJ="${FO_DICOM_DIR}FO-DICOM.Core/FO-DICOM.Core.csproj"
CORE_CSPROJ="${FO_DICOM_DIR}Platform/NetCore/DICOM.NetCore.csproj"
MONO_CSPROJ="${FO_DICOM_DIR}Platform/Mono/DICOM.Mono.csproj"

FO_CSPROJ="$CORE_CSPROJ" # Default to DOTNET
if [ "$NET_TOOLCHAIN" == "MONO" ]; then # Use mono Platform
	FO_CSPROJ="$MONO_CSPROJ"
fi
FO_DIR="${FO_CSPROJ%/*.csproj}/"


# Clone fo-dicom
if [ ! -d "$FO_DICOM_DIR" ]; then
	git clone $FO_DICOM_GIT $FO_DICOM_DIR
fi

# Build fo-dicom
echo "Building fo-dicom"
build $FO_CSPROJ $FO_DICOM_DIR

# Create fuzz project
mkdir -p $FUZZ_PROJ_DIR
if [ ! -d "$FUZZ_PROJ_DIR" ]; then
	echo "Directory \"${FUZZ_PROJ_DIR}\" could not be found" 1>&2
	exit 1
fi

# Determine assembly name, to be referenced in include and to name DLL files
RAW_ASSEMBLY=$(cat $FO_CSPROJ | grep AssemblyName | grep -o -E ">.*<")
ASSEMBLY=${RAW_ASSEMBLY:1:-1}

# Copy and instrument FO-DICOM dll lib
ASSEMBLY_DLL=$(find ${FO_DIR} -mindepth 3 -maxdepth 4 -type f -name "$ASSEMBLY.dll" | grep bin | head -n 1)
cp $ASSEMBLY_DLL $FUZZ_PROJ_DIR
./instrument_dll.sh ${FUZZ_PROJ_DIR}${ASSEMBLY}.dll

if [ ! -f "$FUZZ_CSPROJ" ]; then

  FO_CSPROJ_REL=$(realpath --relative-to="$FUZZ_PROJ_DIR" $FO_CSPROJ)

  cat > ${FUZZ_CSPROJ} <<EOF
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>netcoreapp3.1</TargetFramework>
  </PropertyGroup>

  <ItemGroup>
    <Reference Include="${ASSEMBLY}">
      <HintPath>${ASSEMBLY}.dll</HintPath>
    </Reference>
  </ItemGroup>
  
<!--
-->

  <ItemGroup>
    <ProjectReference Include="..\sharpfuzz\src\SharpFuzz\SharpFuzz.csproj" />
<!--
    <ProjectReference Include="${FO_CSPROJ_REL//\//\\}" />
-->
  </ItemGroup>

</Project>

EOF
fi

if [ ! -f "${FUZZ_PROJ_DIR}/Program.cs" ]; then
	cp ${FUZZ_TEMPLATE_DIR}Program.cs ${FUZZ_PROJ_DIR}Program.cs
fi

build $FUZZ_CSPROJ
