#!/bin/bash
#
# filter_duplicates.sh
# Copyright:
#   2019   Matthias Gierlings
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# After minimiziation we might have duplicate inputs again.
# Filter all identical inputs based on the vectors SHA1 sum.

# The folder to read AFL input vectors from
IMPORT_FOLDER=minimized_corpus

# The folder to write the unique vectors to
EXPORT_FOLDER=export_corpus

# Directory for meta information (i.e. known sha sums)
META_FOLDER=meta

# Logfile for known SHA1 sums - if we encounter input vectors
# with previously seen SHA1 sum we can skip
CHKSUM_LOG="${META_FOLDER}/known_inputs.log"

# seperate input vector into categories crashing/not crashing.
CATEGORY_FOLDERS=(crashes queue)

# Create output direcories iand files if they do not exist.
mkdir -p "${EXPORT_FOLDER}"
mkdir -p "${META_FOLDER}"
touch "${CHKSUM_LOG}"

# For all new input vectors (files inside IMPORT_FOLDER modified
# within the last IMPORT_INTERVAL minutes) calculate the SHA1 sum
# of each file. If the SHA1 is new output the input vector file
# name to stdout and append the SHA1 sum to CHKSUM_LOG.
for CATEGORY in "${CATEGORY_FOLDERS[@]}"; do
    IMPORT_CATEGORY_FOLDER="${IMPORT_FOLDER}/${CATEGORY}/"
    EXPORT_CATEGORY_FOLDER="${EXPORT_FOLDER}/${CATEGORY}/"
    mkdir -p "${EXPORT_CATEGORY_FOLDER}"
    # find all inputfiles from the import category folder
    find "${IMPORT_CATEGORY_FOLDER}" -maxdepth 1 -type f | \
    # calculate the sha1 for each file and only select one file
    # per hash, disregard all other files with same hash.
    xargs sha1sum | sort -u -k1,1 | \
    # remove entries for all known hashes based on the CHEKSUM_LOG
    join -v2 -11 "${CHKSUM_LOG}" - | \
    # Add unknown hashes to the CHKSUM_LOG
    tee >(cut -f 1 -d " " | sed '/^$/d' >> "${CHKSUM_LOG}") | \
    # Copy all files containing unseen inputs to the export folder.
    cut -f2 -d " " | xargs -r -L1 basename | xargs -I'{}' sh -cx "cp ${IMPORT_CATEGORY_FOLDER}{} ${EXPORT_CATEGORY_FOLDER}"'$('"sha1sum ${IMPORT_CATEGORY_FOLDER}/{} | cut -f1 -d' '"')' && \
    # Once we are done sort the CHKSUM_LOG in place.
    sort -o "${CHKSUM_LOG}" "${CHKSUM_LOG}"
done
