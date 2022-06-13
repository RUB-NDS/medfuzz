#!/bin/bash
#
# build_dicom_toolkit.sh
# Copyright:
#   2019,2020   Matthias Gierlings
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# Downloads or pulls and builds the dicom toolkit
# in multiple directories with multiple compile flag.

BASE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
DICOM_TK_GIT="https://github.com/DCMTK/dcmtk.git"
DICOM_TK_URL="ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk365/"
DICOM_TK_ARC="dcmtk-3.6.5.tar.gz"
DICOM_FOLDER="dcmtk"

BASE_FLAGS="-funroll-loops"
DBG_FLAGS="-O0 -ggdb"
COV_FLAGS="-lgcov --coverage"
SANITIZER_FLAGS="-fsanitize=address,undefined -DDCMTK_UNDEF_SANITIZER"

function is_dcmtk_git_present()
{
    if [ -d "$DICOM_FOLDER" ]; then
        cd ${DICOM_FOLDER}
        REMOTE=$(git remote get-url origin)
        if [ "$REMOTE" = "$DICOM_TK_GIT" ]; then
            echo "true"
            return
        fi
    fi
    echo "false"
}

function patch_dcmtk()
{
    PATCH_VERSION="$1"
    patch ${DICOM_FOLDER}/dcmnet/apps/findscu.cc dcmtk_patches/$PATCH_VERSION/findscu_afl.patch
    patch ${DICOM_FOLDER}/dcmnet/apps/movescu.cc dcmtk_patches/$PATCH_VERSION/movescu_afl.patch
    patch ${DICOM_FOLDER}/dcmnet/apps/storescu.cc dcmtk_patches/$PATCH_VERSION/storescu_afl.patch
}

while getopts "g" o; do
    case "$o" in
        g) USE_GIT=1;;
        *) ;;
    esac
done

if [ -z $USE_GIT ]; then
    if [ ! -f "$DICOM_TK_ARC" ]; then
        wget "${DICOM_TK_URL}${DICOM_TK_ARC}"
        if [ $? != 0 ]; then
            echo "Unable to fetch \"${DICOM_TK_URL}${DICOM_TK_ARC}\"."
    	exit 1
        fi
    fi

    if [ ! -d "$DICOM_FOLDER" ]; then
        mkdir ${DICOM_FOLDER}
        tar -xvzf "$DICOM_TK_ARC" -C "$DICOM_FOLDER" --strip-components 1
        patch_dcmtk dcmtk$(cut -d'/' -f1 <<< ${DICOM_TK_URL##*/dcmtk})
    fi
else
    if [ "$(is_dcmtk_git_present)" = "false" ]; then
        git clone ${DICOM_TK_GIT} ${DICOM_FOLDER}
        patch_dcmtk git
    else
        pushd .
        cd ${DICOM_FOLDER}
        if [ "$(git remote get-url)" = "$DICOM_TK_GIT" ]; then
            git checkout -- .
            git pull
        else
            echo "CWD is: $(pwd)"
            echo "DCMTK repository not present, skipping pull.";
        fi
        popd
    fi
fi

cp -R ${DICOM_FOLDER} ${DICOM_FOLDER}_sanitizers
cp -R ${DICOM_FOLDER} ${DICOM_FOLDER}_nosanitizers
cp -R ${DICOM_FOLDER} ${DICOM_FOLDER}_debug
cp -R ${DICOM_FOLDER} ${DICOM_FOLDER}_coverage

cd "${BASE_DIR}/${DICOM_FOLDER}_sanitizers"
pwd
FLAGS="${BASE_FLAGS} ${SANITIZER_FLAGS}"
CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} cmake -DCMAKE_C_COMPILER=afl-clang-fast -DCMAKE_CXX_COMPILER=afl-clang-fast++
CC=afl-clang-fast CXX=afl-clang-fast++ CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} LDFLAGS=${FLAGS} nice -n 19 make -j$(nproc)

cd "${BASE_DIR}/${DICOM_FOLDER}_nosanitizers"
pwd
FLAGS="${BASE_FLAGS}"
CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} cmake -DCMAKE_C_COMPILER=afl-clang-fast -DCMAKE_CXX_COMPILER=afl-clang-fast++
CC=afl-clang-fast CXX=afl-clang-fast++ CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} LDFLAGS=${FLAGS} nice -n 19 make -j$(nproc)

cd "${BASE_DIR}/${DICOM_FOLDER}_debug"
pwd
FLAGS="${BASE_FLAGS} ${DBG_FLAGS} ${SANITIZER_FLAGS}"
CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} cmake -DCMAKE_C_COMPILER=afl-clang-fast -DCMAKE_CXX_COMPILER=afl-clang-fast++
CC=afl-clang-fast CXX=afl-clang-fast++ CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} LDFLAGS=${FLAGS} nice -n 19 make -j$(nproc)

cd "${BASE_DIR}/${DICOM_FOLDER}_coverage"
pwd
FLAGS="${BASE_FLAGS} ${COV_FLAGS}"
CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} cmake -DCMAKE_C_COMPILER=afl-clang-fast -DCMAKE_CXX_COMPILER=afl-clang-fast++
CC=afl-clang-fast CXX=afl-clang-fast++ CFLAGS=${FLAGS} CXXFLAGS=${FLAGS} LDFLAGS=${FLAGS} nice -n 19 make -j$(nproc)

