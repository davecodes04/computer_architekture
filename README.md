# Software Repository for Lecture Computerarchitektur

## Getting started

Clone this repository using the command
```
git clone --recurse https://gitlab.hs-esslingen.de/rakeller/computerarchitektur_arm.git
```

In order to also get the latest of the required sub-projects (among them pico-sdk and picotool),
afterwards recursively checkout these sub-modules:
```
git -c submodule."lib/mbedtls".update=none submodule update --init --recursive
```

This will populate the sub-directory `external`.
This will also skip to include Pico-SDK's mbedtls implementation, as it has compile-time errors.


## Windows Users (others skip to the next step)
On Windows, it's best to use Windows Subsystem for Linux (WSL) for compiling and configuration under Linux, and editing with Visual-Studio under Windows.
1. Install and start it using the Windows command line cmd.exe:
```
wsl.exe --install
wsl.exe --install Ubuntu-24.04
```
When launching WSL, you may require Administrator permissions, therefore start `cmd.exe` with Administrator rights.

2. Inside of WSL install several packages, some using the default package installer `apt` and one as SNAP package (as these are more uptodate):
```
sudo snap cmake
sudo apt -y gcc g++ make ninja
```

3. Under Windows, you may want to install a Serial line monitor (reading from the COM ports) like 

## MacOS Users (others skip to the next step)
For developing on MacOS, you need to also get acquinted to the command line, the so-called "Terminal" program.
By default it uses the Z-Shell (or short `zsh`), however, you should either switch to Bash altogether or just call `bash`
prior to executing any scripts.

Packages are generally installed using the [Homebrew installer](https://brew.sh/). After installation of Homebrew, please add the
following "formula":
```
brew install cmake gcc ninja
```

Afterwards, you will be able to find the packages, all linked into a sub-directory under `/opt/homebrew/Cellar`.

Like Windows-Users, you may want to install Minicom for serial terminal (COM-port) debugging.


## Linux Users (others skip to the next step)
Again, any scripts run within this lecture is based on Bash -- please take care, if You employ any other Shell (like `ash`).
Please also install packages listed under Windows Subsystem for Linux (WSL), so for a Debian/Ubuntu package manager:
```
sudo apt -y gcc g++ make ninja
```

Like Windows-Users, you may want to install Minicom for serial terminal (COM-port) debugging.


## Install the cross-compiler for Your operating system
A compiler like `gcc` translates source code into binary form on Your host system, i.e. regarding CPU and OS. We need a cross-compiler,
that is one translating to the RISC-V CPU architecture (in particular *RISC-V 32Bit*), running inside your host Operating System (Windows, MacOS or Linux).

1. Download the [EmbeCOSM RISC-V Embedded stable Cross-compiler with GCC](https://embecosm.com/downloads/tool-chain-downloads/#risc-v-embedded-stable-release-compilers).
Inside of our project's `external` directory, you may unpack this compiler into a `usr` directory, which we will
install our host-tools picotool, too.

2. Install the compiler by unpacking it and rename this to a new `external/usr/` directory name. Here for an Ubuntu Linux System:
```
cd external/
tar xf ~/Downloads/riscv32-embecosm-ubuntu2204-gcc13.2.0.tar.gz
mv riscv32-embecosm-ubuntu2204-gcc13.2.0 usr
```
where the tilde represents your HOME-directory (environment variable HOME),
while on MacOS you may run:
```
hdiutil attach ~/Downloads/riscv-embecosm-embedded-macos-20250309.dmg
cp -r /Volumes/riscv-embecosm-embedded-macos-20250309/riscv-embecosm-embedded-macos-20250309 usr
```


The Unix Shell needs to *find* this `gcc` cross-compiler, named `riscv32-unknown-elf-gcc`, i.e. it needs to be in the `PATH` environment variable.
So *always* set it using:
```
export PATH=$PWD/usr/bin/:$PATH
```
Only *then* will `which riscv32-unknown-elf-gcc` find this executable and hence `cmake` in the following step.


## Compile `pico-sdk` and `picotool`
The Pico-SDK is the main Software Devolpment Kit by the Raspberry PI foundation. It's open source and like any Hardware Abstraction Layer (HAL), a very thin and light-weight layer on top of RP2040 and RP2350.

Picotool allows interacting with RP2040 and RP2350, e.g. uploading files or converting the binary/hexadecimal executable
from `.hex` into a `.uf2` format. The `picotool` therefore is compiled with the host's standard gcc compiler into
an executable on/for the host.

1. Configure and compile the software `picotool` using the commands:
```
cd external/picotool
# In case the BUILD directory already exists, remove it forcefully
test -d BUILD && rm -fr BUILD
# Configure with Generator Ninja (not Makefile), with a decent assumption of CMake-Version in Release mode, install into the existing usr-directory.
cmake -G Ninja -BBUILD -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../usr -DPICO_SDK_PATH=$PWD/../pico-sdk/ .
cmake --build BUILD
cmake --install BUILD
```

2. Now build `pico-sdk` for the RP2350 microcontroller (using the RISC-V cores) for our WaveShare RP2350 LCD-1.28 board, using:
```
cd external/pico-sdk
test -d BUILD && rm -fr BUILD
cmake -G Ninja -BBUILD -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DPICO_PLATFORM=rp2350-riscv -DPICO_BOARD=waveshare_rp2350_lcd_1.28 .
cmake --build BUILD
cmake --install BUILD # Actually, this does nothing
```

# Compilation of projects
Now You may compile the various projects, e.g. head to directory `01_flash_leds` and build this project:
```
cd 01_flash_leds
cmake -G Ninja -BBUILD .
cmake --build BUILD
```

This will create an executable (using the gcc compiler and linker) and the `.uf2` file (using the picotool):
```
ls -al ./BUILD/src/01_flash_all_leds.uf2
```

By _pressing_ the BOOT *and* RELEASE Button and letting go the RELEASE button _first_, the WaveShare will go into USB-Flash mode:
You may now copy this `.uf2` file onto the newly attached USB-Volume RP2350.
You may do so by drag-and-drop using the File Explorer, or copy it using the Shell, e.g. on MacOS:
```
cp ./BUILD/src/01_flash_all_leds.uf2 /Volumes/RP2350/
```

