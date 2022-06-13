#!/bin/bash
#
# import_unique_imports.sh
# Copyright:
#   2019   Matthias Gierlings
#
# This software is released under the Simplified BSD License 
# (see license.txt)
#
# Imports newly found crashing inputs into ermunaz-web.

# The folder to read AFL input vectors from
IMPORT_FOLDER="/home/ermunaz/corpus/crashes/"

# Logfile for known SHA1 sums - if we encounter input vectors
# with previously seen SHA1 sum we can skip
# Directory for meta information (i.e. known sha sums)
META_FOLDER="meta/"

# Logfile for known SHA1 sums - if we encounter input vectors
# with previously seen SHA1 sum we can skip
CHKSUM_LOG="${META_FOLDER}known_inputs.log"

# Create output direcories and files if they do not exist.
mkdir -p "${META_FOLDER}"
touch "${CHKSUM_LOG}"

declare -A DCMTK_CMDS
DCMTK_CMDS=(\
[findscu]="ermunaz-db input 'dcmtk-findscu' add --env='DCMDICTPATH=/home/ermunaz/dcmtk/dcmdata/data/dicom.dic' --args='--cancel' --args='1' --args='-to' --args='1' --args='-td' --args='1' --args='-ta' --args='1' --args='localhost' --args='1234' --argi /home/ermunaz/empty --stdin ${IMPORT_FOLDER}{}" \
[movescu]="ermunaz-db input 'dcmtk-movescu' add --env='DCMDICTPATH=/home/ermunaz/dcmtk/dcmdata/data/dicom.dic' --args='--cancel' --args='1' --args='-to' --args='1' --args='-td' --args='1' --args='-ta' --args='1' --args='localhost' --args='1234' --argi /home/ermunaz/empty --stdin ${IMPORT_FOLDER}{}" \
[storescu]="ermunaz-db input 'dcmtk-storescu' add --env='DCMDICTPATH=/home/ermunaz/dcmtk/dcmdata/data/dicom.dic'--args='-to' --args='1' --args='-td' --args='1' --args='-ta' --args='1' --args='localhost' --args='1234' --argi /home/ermunaz/empty --stdin ${IMPORT_FOLDER}{}" \
)

DATABASE_URL="postgres:///ermunaz"

# Import all new files from the import folder into ermunaz-web.
# Hashes of imported files are logged in the checksum log file.
for DCMTK_CMD in "${!DCMTK_CMDS[@]}"; do
    find ${IMPORT_FOLDER} -maxdepth 1 -type f -printf "%f\n" |\
        # Sort entries by file name
        sort -u -k1,1 | \
        # remove entries for all known hashes based on the CHEKSUM_LOG
        join -v2 -11 "${CHKSUM_LOG}" - | \
        # Add unknown hashes to the CHKSUM_LOG (we expect filename to be
        # the hash value here. Execute the ermunaz-web import command
        # for each tool on every new input vector.
        tee >(sed '/^$/d' >> "${CHKSUM_LOG}") | \
        xargs -I'{}' sudo -u ermunaz -i bash -c "${DCMTK_CMDS[${DCMTK_CMD}]}" && \
        sort -o "${CHKSUM_LOG}" "${CHKSUM_LOG}"
done

