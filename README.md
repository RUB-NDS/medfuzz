**MedFuzz is experimental software, use is not recommended.
If you proceed to use MedFuzz you do so at your own risk!**

# MedFuzz
The MedFuzz platform automates creating a fuzzing capable build
of a software under test (e.g. DICOM Toolkit), manages the fuzzing
process and performs network communication with the target software
using the component medfuzz-net and automates filtering redundant
results. It supports DICOM-Toolkit/AFL (see `fuzzing/dcmtk`) and
Fo-Dicom/SharpFuzz (see `fuzzing/fodicom`, experimental).

## Important:
_Medfuzz is experimental software and must only be run in
strictly isolated test enviroments dedicated to fuzzing only.
Under no cirumstances should MedFuzz be operated on production
systems or connected to networks_

## Operating environment
MedFuzz was developed and tested for Ubuntu 20.04 LTS. Operation
on other platforms is may require changes. The following
instructions are specific to an Ubuntu 20.04 LTS plattform.

## Build
MedFuzz is written in C++ and bash script and requires various
packages to be installed in order to compile the medfuzz binary.

To install required packages execute:
```
sudo apt install afl++ build-essential cmake wget libgtest-dev libgmock-dev
```

Download dependencies into the `medfuzz/include` subdirectory:

```
cd include
# Download CLI 1.8.0 (or newer)
wget https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp
# Download googletest 1.10
wget -q -O - https://github.com/google/googletest/archive/refs/tags/release-1.10.0.tar.gz | tar -xvz --one-top-level=googletest --strip-components=1 && cd googletest && cmake . && make -j$(nproc)
 ```

To build medfuzz execute:
```
make && make preload
```

## Usage
MedFuzz is a command-line utility and supports the following
options. The mode option `-m` is mandatory.

| Short | Long                   | Values                         | Description                                                                                                           |
|-------|------------------------|--------------------------------|-----------------------------------------------------------------------------------------------------------------------|
|-m     |--mode                  | client, server                 | Act as client / server.                                                                                               |
|-h     |--help                  | -                              | Print instructions and exit                                                                                           |
|-v     |--verbosity             | normal, verbose, debug, trace  | Sets the output verbosity level.                                                                                      |
|-c     |--control-port          | 0 - 65535                      | Port used to exchange control information. (Default: 12345)                                                           |
|-p     |--target-port           | 0 - 65535                      | Port used for communication with the fuzzing target.                                                                  |
|-e     |--echo                  | -                              | Echos back any data received by the target.                                                                           |
|-i     |--initial-data          | [TEXT-FILE]                    | File containing data to be sent to the target immediately after a connection has been established with the peer.      |
|-r     |--reply-data            | [TEXT-FILE]                    | File contating data to be sent to the target in reply to data received.                                               |
|-H     |--host                  | [TEXT-FILE]                    | Name of the host the target is running on (Default : localhost).                                                      |
|-d     |--diconnect-after-send  | -                              | Closes the network connection to the peer after the reply to the first incoming message from the peer has been sent.  |
|-D     |--disconnect-on-receive | -                              | Immediately closes the network connection to the peer once data has been received from peer.                          |

For details on automation of the Fuzzing process and deduplication
of results please refer to the documentation in `fuzzing/dcmtk` and
`fuzzing/fo-dicom`



















