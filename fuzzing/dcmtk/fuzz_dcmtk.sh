#!/bin/bash
#
# fuzz_dcmtk.sh
# Copyright:
#   2019,2020   Matthias Gierlings
#   2021   	    Marius Merten
#
# This software is released under the Simplified BSD License
# (see license.txt)
#
# This script checks the requirements for a sane afl-fuzz process
# for the dicom toolkit (DMCTK).

BASE_DIR="/mnt/ramdisk/"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/"
DICOM_DEFAULT_IN_FILE="README.md"
DICOM_IN_FILE=""
DICOM_SRC_DIR="dcmtk"
FUZZING_GROUP="default"
SLAVES=0
MAX_SEED_SIZE=100 # in bytes
MEDFUZZ_BIN='../../medfuzz'

# check if target binary was passed
while getopts "c:b:i:g:i:s:" o;
do
   case "$o" in
        b) DICOM_SRC_DIR=$OPTARG;;  # name of dcmtk folder containing src/binaries
        c) DICOM_BIN=$OPTARG;;      # name of dicom binary to fuzz
        i) DICOM_IN_FILE=$OPTARG;;  # inputfile to read DICOM query from
        g) FUZZING_GROUP=$OPTARG;;  # group folder name to write fuzzer output to
        s) SLAVES=$OPTARG;;         # number of slave processes for master fuzzer
        *) exit 1;;
   esac
done

# Use AFL vector as input if $DICOM_IN_FILE is provided
if [ -z ${DICOM_IN_FILE} ]
then
    USE_AFL_QUERY_INPUT=true;
else
    USE_AFL_QUERY_INPUT=false;
fi

if [ -z ${DICOM_BIN} ]; then
    echo "Fuzzing target binary must be specified"
    exit 1;
fi

DCMDICTPATH="${BASE_DIR}${DICOM_SRC_DIR}/dcmdata/data/dicom.dic"
ASAN_OPTIONS="abort_on_error=1:symbolize=0:detect_leaks=0"
DICOM_BASE_DIR="${BASE_DIR}${DICOM_SRC_DIR}/"
DICOM_BIN_DIR="${DICOM_BASE_DIR}bin/"
DICOM_PORT=$(./get_random_unused_port.sh)
CONTROL_PORT=$(./get_random_unused_port.sh)

# Check if ramdisk is mounted
mountpoint -q "${BASE_DIR}"
if [ ! $? -eq 0 ]
then
    echo "Ramdisk not mounted"
    exit 1;
fi


AFL_DATA_DIR="${BASE_DIR}data/${FUZZING_GROUP}/"
mkdir -p $AFL_DATA_DIR
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
AFL_INSTANCE_DIR="$(mktemp -p ${AFL_DATA_DIR} -d dcmtk_${DICOM_BIN}_${TIMESTAMP}_XXXXXXXXXX)/"
AFL_VECTOR="${AFL_INSTANCE_DIR}out.vec"
touch $AFL_VECTOR


DICOM_IN_FILE="${AFL_INSTANCE_DIR}${DICOM_IN_FILE}"
if [ "${DICOM_IN_FILE}" == '##' ]; then
    DICOM_IN_FILE=$AFL_VECTOR
fi

if [ ! -f "${DICOM_IN_FILE}" ]; then
    cp ${DICOM_DEFAULT_IN_FILE} ${DICOM_IN_FILE}
    if [ $? != 0 ]; then
        echo "Could not create \"${DICOM_IN_FILE}\"."
        exit 1;
    fi
fi

if [ ! $(echo $DICOM_SRC_DIR | tr -d [:space:]) == "/" ]; then
    DICOM_SRC_DIR="${DICOM_SRC_DIR}/"
fi

# Copy DICOM toolkit to ramdisk in case if not present yet
if [ ! -d "$DICOM_BASE_DIR" ]; then
    cp -R ${DICOM_SRC_DIR} ${DICOM_BASE_DIR}
    if [ $? != 0 ]; then
        echo "Could not copy DICOM tool kit to ramdisk."
        exit 1;
    fi
fi

# create AFL input and output folders

mkdir -p "in"
if [ ! -d "in" ]; then
    echo "AFL input directory \"${IN_DIR}\" does not exist."
    exit 1;
fi

# Copy small enough test cases from export_corpus to input
# folder
for SEED in $(find export_corpus/queue/ -type f -size "-${MAX_SEED_SIZE}c");
do
    cp "${SCRIPT_DIR}$SEED" "${SCRIPT_DIR}in/"
done

# Create link to input folder inside the folder for the current
# AFL run.
IN_DIR="${AFL_INSTANCE_DIR}in/"
ln -sf "${SCRIPT_DIR}in" "${AFL_INSTANCE_DIR}in"

OUT_DIR="${AFL_INSTANCE_DIR}out/"
mkdir -p "$OUT_DIR"
if [ ! -d "${OUT_DIR}" ]; then
    echo "AFL output directory \"${OUT_DIR}\" does not exist."
    exit 1;
fi

AFL_MASTER_CMD="afl-fuzz -m 100000000 -i ${IN_DIR} -o ${OUT_DIR} -M ${DICOM_BIN}_$$_0 -f ${AFL_VECTOR} -t 1500"

case ${DICOM_BIN} in
    "findscu"|"movescu")
        DICOM_CMD="${DICOM_BIN_DIR}${DICOM_BIN} --cancel 1 -to 1 -td 1 -ta 1 localhost";;
    "storescu")
        DICOM_CMD="${DICOM_BIN_DIR}${DICOM_BIN} -to 1 -td 1 -ta 1 localhost";;

    *) echo "Unknown command \"${DICOM_BIN}\"."
       exit 1;;
esac

# Start network fuzzing service
MEDFUZZ_PIDS=()
MEDFUZZ_CMD="nohup ${MEDFUZZ_BIN} -m server -r ${AFL_VECTOR} -p $DICOM_PORT &"
echo $MEDFUZZ_CMD > ${AFL_INSTANCE_DIR}cmd.txt
eval $MEDFUZZ_CMD
MEDFUZZ_PIDS+=($!)

if [[ ${USE_AFL_QUERY_INPUT} == true ]]; then DICOM_IN_FILE=${AFL_VECTOR}; fi
AFL_CMD="ASAN_OPTIONS=${ASAN_OPTIONS} DCMDICTPATH=${DCMDICTPATH} $AFL_MASTER_CMD $DICOM_CMD $DICOM_PORT $DICOM_IN_FILE &"
echo $AFL_CMD >> ${AFL_INSTANCE_DIR}cmd.txt
eval $AFL_CMD
if [ ${SLAVES} -gt 0 ]; 2>/dev/null
then
    for ((SID=1; SID<=SLAVES; SID++));
    do

        DICOM_PORT=$(./get_random_unused_port.sh)
        AFL_VECTOR="${AFL_INSTANCE_DIR}out${SID}.vec"
	if [[ ${USE_AFL_QUERY_INPUT} == true ]]; then DICOM_IN_FILE=${AFL_VECTOR}; fi
        touch $AFL_VECTOR
        AFL_SLAVE_CMD="afl-fuzz -m 100000000 -i ${IN_DIR} -o ${OUT_DIR} -S ${DICOM_BIN}_$$_${SID} -f ${AFL_VECTOR} -t 1500"
        MEDFUZZ_CMD="nohup ${MEDFUZZ_BIN} -m server -r ${AFL_VECTOR} -p $DICOM_PORT &"
        eval $MEDFUZZ_CMD
        MEDFUZZ_PIDS+=($!)
        AFL_CMD="AFL_NO_UI=1 ASAN_OPTIONS=${ASAN_OPTIONS} DCMDICTPATH=${DCMDICTPATH} $AFL_SLAVE_CMD $DICOM_CMD $DICOM_PORT $DICOM_IN_FILE &"
        eval $AFL_CMD
    done
fi


