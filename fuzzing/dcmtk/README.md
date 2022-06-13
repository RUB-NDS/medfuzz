# MedFuzz DICOM fuzzing utilities

## Setting up the environment for fuzzing
Executing `afl_setup.sh` applies changes to the environment required
for fuzzing.

## Building the test subject
`build_dicom_toolkit.sh` downloads the source code of the specified
DICOM-Toolkit version and extracts it, applies patches required to
create a working Sanitizer build and compiles the patched version
with AFL instrumentation.

## Fuzzing the test subject (fuzz_dcmtk.sh)
Determines free ports and starts an instance of medfuzz to
enable network communication with the test subject (e.g. a
DICOM-Toolkite DIMSE Service implementation).

| Option | Description |
| ------ | ------ |
| b | Name of DCMTK folder containing src/binaries |
| c | Name of DICOM binary to fuzz |
| g | Group folder to write fuzzer output to |
| s | Number of slave processes for master fuzzer |
| i | input file to read DICOM query from, if empty use same data as supplied via medfuzz |

### Seeds
To use seed inputs remove the file `in/default` and
place a set of files containing seeds in that directory.

### Examples
A few examples on how to use the fuzz_dcmtk.sh on various binaries.

```
# Fuzz the findscu with sanitizers
./fuzz_dcmtk.sh -c findscu -g findscu_asan -b dcmtk_sanitizers/

# Fuzz the storescu without sanitizers
./fuzz_dcmtk.sh -c storescu -g storescu_noasan -b dcmtk_sanitizers/

# Fuzz movescu with sanitizers and 15 slave instances (16-CPU cores recommended)
./fuzz_dcmtk.sh -c movescu -g movescu_corpus -b dcmtk_nosanitizers/ -s 15
```

## Filtering and deduplicating results
Fuzzing may produce large sets of redundant input vectors for a
single bug. An automated screening process implemented in
`filter_duplicates.sh` and `minimize_corpus.sh` may help to reduce
findings prior to manual analysis. Filtering is performed in two
steps:

1. `minimize_corpus.sh` executes all vectors stored on the ramdisk
    using the DICOM-Toolkit implementations of the DIMSE services storescu,
    findscu and movescu in conjunction with afl-cmin and afl-tmin. The
    minimized set of vectors is stored to the sub-directory
    `minimized_corpus`.
2. `filter_duplicates` merges duplicates that may have been generated in
    step one or exist from prior fuzzing runs and writes a corpus of
    unique vectors to the subs-directory `export_corpus`. The filename
    of each vector inside the `export_corpus` directory is equal to the
    SHA1 hash of its contents.

