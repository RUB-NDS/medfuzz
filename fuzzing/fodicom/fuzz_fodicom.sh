#!/bin/bash
#
# fuzz_fodicom.sh
# Copyright:
#   2019-2021	Matthias Gierlings
#   2020,2021	Marius Merten
#
# This software is released under the Simplified BSD License
# (see license.txt)
#
# This script checks the requirements for a sane afl-fuzz process
# for the fo-dicom library.

TIMESTAMP=$(date +%Y%m%d_%H%M%S)

BASE_DIR="/mnt/ramdisk/"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/"

FUZZING_GROUP="default"
#FODICOM_IN_FILE="" # ${AFL_INSTANCE_DIR}out.vec, if empty
#FODICOM_DEFAULT_IN_FILE="README.md"

FODICOM_FUZZ_PROJ_NAME="fo-fuzz"
SLAVES=0

# Since we are fuzzing /usr/bin/dotnet <someDLL>
# we must skip the instrumentation check.
AFL_OPTS="AFL_SKIP_BIN_CHECK=1"

# And maybe remove fork server because it crashes
# the fuzzing process at some point
#AFL_OPTS+=" AFL_NO_FORKSRV=1"

# GC Settings
AFL_OPTS+=" COMPlus_gcServer=1"					# Use server - not desktop - gargabe collection
AFL_OPTS+=" COMPlus_gcConcurrent=1" 			# Use background gargabe collection
AFL_OPTS+=" COMPlus_GCHeapHardLimit=0x80000000" # Set 2 GiB memory limit for gargabe collection

MEDFUZZ_BIN="../../medfuzz"

MAX_SEED_SIZE=100 # in bytes

while getopts "g:i:p:s:" o; do
	case "$o" in
       		g) FUZZING_GROUP=$OPTARG;;  		# group folder name to write fuzzer output to
        	i) FODICOM_IN_FILE=$OPTARG;;  		# inputfile to read DICOM query from
        	p) FODICOM_FUZZ_PROJ_NAME=$OPTARG;; # name of project folder containing fuzzing application
        	s) SLAVES=$OPTARG;;         		# number of slave processes for master fuzzer
        	*) exit 1;;
	esac
done

# extend to full path
FODICOM_FUZZ_PROJ_DIR="$(realpath ${FODICOM_FUZZ_PROJ_NAME})/"

# Use AFL vector as input if $FODICOM_IN_FILE is not provided
#USE_AFL_QUERY_INPUT=1
#if [ -n ${FODICOM_IN_FILE} ]; then
#	unset USE_AFL_QUERY_INPUT
#fi

# Check if ramdisk is mounted
mountpoint -q "${BASE_DIR}"
if [ ! $? -eq 0 ];
then
	echo "Ramdisk not mounted" 1>&2
	exit 1
fi

# More path vars
AFL_DATA_DIR="${BASE_DIR}data/${FUZZING_GROUP}/"
mkdir -p $AFL_DATA_DIR
AFL_INSTANCE_DIR="$(mktemp -p ${AFL_DATA_DIR} -d fo-dicom_${TIMESTAMP}_XXXXXXXXXX)/"

AFL_VECTOR="${AFL_INSTANCE_DIR}out.vec"
touch $AFL_VECTOR

# Set to AFL_VECTOR if FODICOM_IN_FILE is absent
FODICOM_IN_FILE="${AFL_INSTANCE_DIR}${FODICOM_IN_FILE}"
if [ -n $USE_AFL_QUERY_INPUT ]; then
	FODICOM_IN_FILE=$AFL_VECTOR
fi

if [ ! -f "${FODICOM_IN_FILE}" ]; then
	cp ${FODICOM_DEFAULT_IN_FILE} ${DICOM_IN_FILE}
	if [ $? != 0 ];
	then
		echo "Could not create \"${DICOM_IN_FILE}\"." 1>&2
		exit 1
	fi
fi

RAM_PROJ_DIR="${BASE_DIR}${FODICOM_FUZZ_PROJ_NAME}"
# Copy FODICOM_FUZZ_PROJ_DIR to ramdisk in case if not present yet
if [ ! -d "$RAM_PROJ_DIR" ]; then
	cp -r ${FODICOM_FUZZ_PROJ_DIR} ${BASE_DIR}
	if [ $? != 0 ];
	then
		echo "Could not copy ${FODICOM_FUZZ_PROJ_NAME} to ramdisk." 1>&2
		exit 1
	fi
fi

# create AFL input and output folders
GLOBAL_IN_DIR="${SCRIPT_DIR}in"
mkdir -p "$GLOBAL_IN_DIR"
if [ ! -d "$GLOBAL_IN_DIR" ]; then
	echo "AFL input directory \"${GLOBAL_IN_DIR}\" does not exist." 1>&2
	exit 1
fi

OUT_DIR="${AFL_INSTANCE_DIR}out/"
mkdir -p "$OUT_DIR"
if [ ! -d "$OUT_DIR" ]; then
	echo "AFL output directory \"${OUT_DIR}\" does not exist." 1>&2
	exit 1
fi

# Create link to input folder inside the folder
# for the current AFL run.
IN_DIR="${AFL_INSTANCE_DIR}in/"
ln -sf "$GLOBAL_IN_DIR" "$AFL_INSTANCE_DIR"

# Copying small enough test cases from export_corpus
# to input folder
for SEED in $(find export_corpus/queue/ -type f -size -${MAX_SEED_SIZE}c); do
    cp "${SCRIPT_DIR}$SEED" "$GLOBAL_IN_DIR"
done

# Command building
MEDFUZZ_PIDS=()
FODICOM_PORT=$(./get_random_unused_port.sh)

AFL_MASTER_CMD="afl-fuzz -m 100000000 -i ${IN_DIR} -o ${OUT_DIR} -f ${AFL_VECTOR} -M ${FUZZING_GROUP}_$$_0 -t 5000"


FODICOM_FUZZ_DLL=$(find ${RAM_PROJ_DIR} -mindepth 3 -maxdepth 4 -type f -name "${FODICOM_FUZZ_PROJ_NAME}.dll" | grep bin | head -n 1)
FODICOM_CMD="dotnet ${FODICOM_FUZZ_DLL}"

# Start network fuzzing service
MEDFUZZ_CMD="nohup ${MEDFUZZ_BIN} -m server -r ${AFL_VECTOR} -p $FODICOM_PORT &"
echo $MEDFUZZ_CMD > ${AFL_INSTANCE_DIR}cmd.txt
eval $MEDFUZZ_CMD
MEDFUZZ_PIDS+=($!)

#if [[ ${USE_AFL_QUERY_INPUT} == true ]]; then DICOM_IN_FILE=${AFL_VECTOR}; fi
AFL_CMD="$AFL_OPTS $AFL_MASTER_CMD $FODICOM_CMD $FODICOM_PORT &"
echo $AFL_CMD >> ${AFL_INSTANCE_DIR}cmd.txt
eval $AFL_CMD

for ((SID=1; SID<=SLAVES; SID++)); do
	FODICOM_PORT=$(./get_random_unused_port.sh)

	AFL_VECTOR="${AFL_INSTANCE_DIR}out${SID}.vec"
	#if [[ ${USE_AFL_QUERY_INPUT} == true ]]; then DICOM_IN_FILE=${AFL_VECTOR}; fi
	#touch $AFL_VECTOR
	AFL_SLAVE_CMD="afl-fuzz -m 100000000 -i ${IN_DIR} -o ${OUT_DIR} -f ${AFL_VECTOR} -S ${FUZZING_GROUP}_$$_${SID} -t 5000"

	MEDFUZZ_CMD="nohup ${MEDFUZZ_BIN} -m server -r ${AFL_VECTOR} -p $FODICOM_PORT &"
	eval $MEDFUZZ_CMD
	MEDFUZZ_PIDS+=($!)

	AFL_CMD="AFL_NO_UI=1 $AFL_OPTS $AFL_SLAVE_CMD $FODICOM_CMD $FODICOM_PORT &"
	eval $AFL_CMD
done

