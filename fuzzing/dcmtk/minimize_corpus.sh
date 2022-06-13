#!/bin/bash
#
# minimize_corpus.sh
# Copyright:
#   2019-2021   Matthias Gierlings
#
# This software is released under the Simplified BSD License
# (see license.txt)
#
# Exports the corpus of a fuzzing run. The exported corpus will be
# minimized using afl-cmin (removes superflous inputs) and afl-tmin
# (reduces input length)

BASE_DIR="/mnt/ramdisk/"
DICOM_SRC_DIR="dcmtk_sanitizers"
DCMDICTPATH="${BASE_DIR}${DICOM_SRC_DIR}/dcmdata/data/dicom.dic"
DICOM_BASE_DIR="${BASE_DIR}${DICOM_SRC_DIR}/"
DICOM_BIN_DIR="${DICOM_BASE_DIR}bin/"
# We must not abort on error during corpus minimization or
# afl-cmin will eliminate relevant vectors. Also disable
# leak sanitizer to prevent early crashes.
CMIN_ASAN_OPTIONS="abort_on_error=0:symbolize=0:detect_leaks=0"
TMIN_ASAN_OPTIONS="abort_on_error=1:symbolize=0:detect_leaks=0"
AFL_DATA_DIR="${BASE_DIR}data/"
AFL_VECTOR="out0.vec"
AFL_TIMEOUT=1500 # in ms
AFL_MEMORY=100000000 # in MB
TEMP_DIR="tmp/"
MEDFUZZ_BIN="../../medfuzz"

mkdir -p "${TEMP_DIR}"

# Folder containing the raw corpus files from all afl instances
IMPORT_FOLDER="import_corpus/"
mkdir -p "${IMPORT_FOLDER}"

# Target folder for minimized corpus
EXPORT_FOLDER="minimized_corpus/"
mkdir -p "${EXPORT_FOLDER}"

DICOM_IN_FILE=empty
DICOM_PORT=$(./get_random_unused_port.sh)
CONTROL_PORT=$(./get_random_unused_port.sh)

if [ ! -f "${DICOM_IN_FILE}" ]
then
    echo "Error: Can not access input query file: \"${DICOM_IN_FILE}\"."
    exit 1
fi

declare -A DCMTK_CMDS
DCMTK_CMDS=(\
    [findscu]="${DICOM_BIN_DIR}findscu --cancel 1 -to 1 -td 1 -ta 1 localhost"\
    [movescu]="${DICOM_BIN_DIR}movescu --cancel 1 -to 1 -td 1 -ta 1 localhost"\
    [storescu]="${DICOM_BIN_DIR}storescu -to 1 -td 1 -ta 1 localhost"\
)

# seperate input vector into categories crashing/not crashing.
CATEGORY_FOLDERS=(crashes queue)

# Check if ramdisk is mounted
mountpoint -q "${BASE_DIR}"
if [ ! $? -eq 0 ]
then
    echo "Ramdisk not mounted"
    exit 1
fi

#
CPU_THREADS=$(nproc)
# Set environement variables for AFL and DCMTK
CMIN_ENV_PREFIX="ASAN_OPTIONS=${CMIN_ASAN_OPTIONS} DCMDICTPATH=${DCMDICTPATH}"
TMIN_ENV_PREFIX="ASAN_OPTIONS=${TMIN_ASAN_OPTIONS} DCMDICTPATH=${DCMDICTPATH}"
MEDFUZZ_PIDS=()
for ((t=0; t<CPU_THREADS; t++))
do
    touch "${TEMP_DIR}out${t}.vec"
    MEDFUZZ_CMD="${MEDFUZZ_BIN} -m server -r ${TEMP_DIR}out${t}.vec -p $(($DICOM_PORT + $t)) &"
    echo ${MEDFUZZ_CMD}
    eval ${MEDFUZZ_CMD}
    MEDFUZZ_PIDS+=($!)
done

for CATEGORY in "${CATEGORY_FOLDERS[@]}"; do
    mkdir -p "${EXPORT_FOLDER}${CATEGORY}/"
    IMPORT_CATEGORY_FOLDER="${IMPORT_FOLDER}${CATEGORY}/"
    mkdir -p "${IMPORT_CATEGORY_FOLDER}"
    find ${AFL_DATA_DIR}*/*/out/*/${CATEGORY}/ -maxdepth 0 -type d | xargs -r -I'{}' rsync -avx "{}" "${IMPORT_CATEGORY_FOLDER}"
    if [ -f "${IMPORT_CATEGORY_FOLDER}/README.txt" ]; then
        rm "${IMPORT_CATEGORY_FOLDER}/README.txt"
    fi
    sleep 1;
    for DCM_NAME in "${!DCMTK_CMDS[@]}"; do
        DCM_CMD=${DCMTK_CMDS[$DCM_NAME]}
        WORK_IN_DIR=$(mktemp -d -p ${TEMP_DIR} "${DCM_NAME}_cmin_${CATEGORY}_in_XXXXXXXX")/
        WORK_OUT_DIR=$(mktemp -d -p ${TEMP_DIR} "${DCM_NAME}_cmin_${CATEGORY}_out_XXXXXXXX")/
        for vec in $(find ${IMPORT_CATEGORY_FOLDER} -maxdepth 1 -type f -printf "%f\0" | xargs -0 -L1)
        do
            cp "${IMPORT_CATEGORY_FOLDER}$vec" "${WORK_IN_DIR}${DCM_NAME}_$vec";
        done
        AFL_CMIN="afl-cmin -i ${WORK_IN_DIR} -o ${WORK_OUT_DIR} -f ${TEMP_DIR}${AFL_VECTOR} -t ${AFL_TIMEOUT} -m ${AFL_MEMORY}"
        CMD="${CMIN_ENV_PREFIX} ${AFL_CMIN} -- ${DCM_CMD} ${DICOM_PORT} ${DICOM_IN_FILE}"
        #echo $CMD
        eval $CMD
        if [[ $WORK_IN_DIR == ${TEMP_DIR}${DCM_NAME}* ]];
        then
            rm -rf ${WORK_IN_DIR}
        else
            echo "Invalid working directory name. Skipping cleanup."
        fi
        WORK_IN_DIR=${WORK_OUT_DIR}
        AFL_TMIN="afl-tmin -i ${WORK_IN_DIR}"'{}'" -o ${EXPORT_FOLDER}${CATEGORY}/"'{}'" -t ${AFL_TIMEOUT} -m ${AFL_MEMORY}"
        CMD="${TMIN_ENV_PREFIX} ${AFL_TMIN} "
        find ${WORK_IN_DIR} -maxdepth 1 -type f -printf "%f\0" | xargs -0 -P$(nproc) -I'{}' --process-slot-var=PROCOFFS sh -cx "${CMD} -f ${TEMP_DIR}out"'$PROCOFFS.vec'" ${DCM_CMD}"' $(('"$DICOM_PORT"'+$PROCOFFS))'" ${DICOM_IN_FILE}"
        if [[ $WORK_IN_DIR == ${TEMP_DIR}${DCM_NAME}* ]];
        then
            rm -rf ${WORK_IN_DIR}
        else
            echo "Invalid working directory name. Skipping cleanup."
        fi
    done
done

echo "${MEDFUZZ_PIDS[*]}" | xargs kill
rm ${TEMP_DIR}out*.vec


