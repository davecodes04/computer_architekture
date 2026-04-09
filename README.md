# Software Repository for Lecture Computerarchitektur
This repository contains example code for Waveshare RP2350 LCD-1.28 employing RISC-V and best practices
with regard to usage with Visual Studio code, CMake and debugging (on top of OpenOCD and GDB).
Please follow this README closely, so that all platform tools are installed.

## Getting started

Clone this repository using the command
```
git clone --recurse-submodules --shallow-submodules https://gitlab.hs-esslingen.de/rakeller/computerarchitektur_arm.git
```

In order to also get the latest of the required sub-projects (among them pico-sdk and picotool),
afterwards recursively checkout these sub-modules:
```
git -c submodule."lib/mbedtls".update=none submodule update --init --remote --recursive --recommend-shallow
or
git submodule update --init --remote --recommend-shallow
```

This will populate the sub-directory `external`.
This will also skip to include Pico-SDK's mbedtls implementation, as it has compile-time errors.
(maybe this submodule is needed however to compile picotool)...

*First*, we will run from the command line, later You may setup VSCode with extensions)

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
sudo apt install -y gcc g++ make automake autoconf libtool pkg-config ninja-build
```

3. Under Windows, you may want to install a Serial line monitor (reading from the COM ports) like 

## MacOS Users (others skip to the next step)
For developing on MacOS, you need to also get acquinted to the command line, the so-called "Terminal" program.
By default it uses the Z-Shell (or short `zsh`), however, you should either switch to Bash altogether (please Google how to do that on MacOS)
or just always call `bash` prior to executing any scripts.

Packages are generally installed using the [Homebrew installer](https://brew.sh/). After installation of Homebrew, please add the
following "formula":
```
brew install cmake gcc automake autoconf libtool pkgconf ninja
```

Afterwards, you will be able to find the packages, all linked into a sub-directory under `/opt/homebrew/Cellar`.

Like Windows-Users, you may want to install Minicom for serial terminal (COM-port) debugging:
```
brew install minicom
```


## Linux Users (others skip to the next step)
Again, any scripts run within this lecture is based on Bash -- please take care, if You employ any other Shell (like `ash`).
Please also install packages listed under Windows Subsystem for Linux (WSL), so for a Debian/Ubuntu package manager:
```
sudo apt install -y gcc g++ make automake autoconf libtool pkg-config ninja-build
```

Like Windows and MacOS-Users, you may want to install Minicom for serial terminal (COM-port) debugging.


## Install the cross-compiler for Your operating system
A compiler like `gcc` translates source code into binary form on Your host system, i.e. regarding CPU and OS. We need a cross-compiler,
that is one translating to the RISC-V CPU architecture (in particular *RISC-V 32Bit*), running inside your host Operating System (Windows, MacOS or Linux).

1. Download the [EmbeCOSM RISC-V Embedded stable Cross-compiler with GCC](https://embecosm.com/downloads/tool-chain-downloads/#risc-v-embedded-stable-release-compilers).
Inside of our project's `external` directory, you may unpack this compiler into the below `external/usr` directory, which we will
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


Please note: if You want to work from the *Shell* (or the Windows ```cmdline.exe```) instead of VS Code,
this Unix Shell needs to *find* the `gcc` cross-compiler, named `riscv32-unknown-elf-gcc`,
i.e. it needs to be in the `PATH` environment variable.
You may set this using:
```
export PATH=$PWD/usr/bin/:$PATH
```
Only *then* will `which riscv32-unknown-elf-gcc` find this executable and other executables (like `riscv32-unknown-elf-objdump`)


## Compile `pico-sdk` and `picotool`, as well as `openocd`
The Pico-SDK is the main Software Devolpment Kit by the Raspberry PI foundation. It's open source and like any Hardware Abstraction Layer (HAL), a very thin and light-weight layer on top of RP2040 and RP2350.

Picotool allows interacting with RP2040 and RP2350, e.g. uploading files or converting the binary/hexadecimal executable
from `.hex` into a `.uf2` format. The `picotool` therefore is compiled with the host's standard gcc compiler into
an executable on/for the host.

1. Configure and compile the software `picotool` using the commands:
```
cd external/picotool
# In case the build directory already exists, remove it forcefully
test -d build && rm -fr build
# Configure with Generator Ninja (not Makefile), with a decent assumption of CMake-Version in Release mode, install into the existing usr-directory.
cmake -G Ninja -Bbuild -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PWD/../usr -DPICO_SDK_PATH=$PWD/../pico-sdk/ .
cmake --build build
cmake --install build
```

2. Now build `pico-sdk` for the RP2350 microcontroller (using the RISC-V cores) for our WaveShare RP2350 LCD-1.28 board, using:
```
cd external/pico-sdk
test -d build && rm -fr build
cmake -G Ninja -Bbuild -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DPICO_PLATFORM=rp2350-riscv -DPICO_BOARD=waveshare_rp2350_lcd_1.28 .
cmake --build build
cmake --install build # Actually, this does nothing
```

3. Finally build the `openocd` binary for connecting and using RPI's DebugProbe as Debugger HW. This software compiles using Autoconf and Automake,
i.e. it needs to be called with configure prior to calling make. Pay attention to disable the "Warnings-as-Errors" flag for GCC:
```
cd external/openocd
test -d build && rm -fr build ; mkdir build && cd build
../configure --prefix=$PWD/../../usr --disable-werror --enable-internal-jimtcl
make
make install
```
_Attentention_: In case the ```make``` step produces errors like ```build-aux/missing```, You have to install the ```autoconf```, ```automake``` and ```libtool``` packages -- after first trying to run ```autoreconf -f -i```. This reinstalls these missing Shell scripts. (Thanks to Mia Miebach)
If the Host-compiler throws errors when building the internal JIMTCL library, then best reinstall the development package of JIMTCL on Your machine:
```
# On Linux: sudo apt install -y libjim-dev
# On MacOS: brew install jimtcl
```

# Compilation of projects from the command line
Now You may compile the various projects, e.g. head to directory `01_flash_leds` and build this project:
```
cd 01_flash_leds
cmake -G Ninja -Bbuild .
cmake --build build
```
Please note, that the Visual Studio Code extension "CMake Tools" will build into a directory called `build` (aka all lower-case).
This will create an executable (using the gcc compiler and linker) and the `.uf2` file (using the picotool):
```
ls -al ./build/src/01_flash_all_leds.uf2
```

By _pressing_ the BOOT *and* RELEASE Button and letting go the RELEASE button _first_, the RP2350 will go into USB-Flash mode:
You may now copy this `.uf2` file onto the newly attached USB-Volume RP2350.
You may do so by drag-and-drop using the File Explorer, or copy it using the Shell, e.g. on MacOS:
```
cp ./build/src/01_flash_all_leds.uf2 /Volumes/RP2350/
```

# Compilation of projects from VSCode
Editing and compiling from VSCode requires a few extensions. Please install the following:
* C/C++ from Microsoft to allow editing C- and Header-files
* RISC-V Support from zwhu95 which allows syntax-highlighting on RISC-V Assembler-files.
* CMake Tools from Microsoft to allow building projects (based on the `tasks.json` and `settings.json` files)
* Cortex-Debug from marus25, which allows the Debugging as described below (installs various sub-projects)

# Debugging
There's multiple ways to debug programs running on the RP2350. The dumbest of them all is `printf`-debugging, which requires
to set in `CMakeLists.txt`
```pico_enable_stdio_usb(executable_name 1)```
and then attaching a UART-program (like minicom) to the serial interface, showing the output of `stdout`.

Better alternatives are real debuggers. Listed in terms of diminishing convenience
- Segger IDE: This interactive IDE features a debugger, which will also use the JTAG/SWD Interface exposed using RPI's DebugProbe hardware. We will __not__ use this.
- VScode using Plattform IO: The Platform IO extension offers a interactive debugger within VScode. This requires a bit of setup, e.g. adding a PIO file to the project. We will __not__ use this extension for now.
- VScode using Extension "Cortex-Debug": This extension offers a interactive debugger within VScode. It just requires 3 setup files `tasks.json` with `settings.json` for building the project and `launch.json` to launch and attach the `gdb` debugger to the RPI DebugProbe.
- GDB using OpenOCD: Running the GNU Debugger on the console. This may look awkward at first, but is a very handy tool. 


## Segger IDE
We will not use this for now -- we will use VSCode using Cortex-Debug (see below).


## VSCode using Plattform IO
We will not use this for now -- we will use VSCode using Cortex-Debug (see below).


## GDB using OpenOCD (for reference)
We will use VSCode using Cortex-Debug, however it's good to know the background of tools employed.
OpenOCD allows connecting to the RPI's DebugProbe HW (which itselve is just a PICO with a RP2040) over USB.
This DebugProbe is attached to WaveShare's SWCLK and SWDIO pins (as well as GND!).
The DebugProbe's red LED must be lit, otherwise do *reconnect* the DebugProbe by re-plugging the Micro-USB cable!
OpenOCD when started detects the DebugProbe:
```
openocd -s INSTALL_DIR/share/ -f interface/cmsis-dap.cfg -f target/rp2350-riscv.cfg -c "adapter speed 5000" -c "set USE_CORE 0" 
```
where `INSTALL_DIR` is the directory where OpenOCD was installed into: this is just required to find the `.cfg` files in the
subdirectories below the `INSTALL_DIR`.

The output should look like this:
```
Open On-Chip Debugger 0.12.0+dev-00002-gcd4873400 (2025-12-08-16:09)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : [rp2350.rv0] Hardware thread awareness created
Info : [rp2350.rv1] Hardware thread awareness created
ocd_process_reset_inner
adapter speed: 5000 kHz
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : Using CMSIS-DAPv2 interface with VID:PID=0x2e8a:0x000c, serial=E661640843775933
...
Info : CMSIS-DAP: Interface ready
Info : clock speed 5000 kHz
Info : [rp2350.rv0] Examined RISC-V core
...
Info : Listening on port 3333 for gdb connections
```
(some lines cut)

Now, one may start the debugger GDB (configured in the RISC-V toolchain):
```
riscv32-unknown-elf-gdb src/PROJECT_BINARY.elf
```
where PROJECT_BINARY is the name of the output execuable in ELF-format analysed by GDB,
while the file copied the RP2350 is of container format UF2 (created by `picotool`).

All the commands however need to go to the DebugProbe, which will relay these to the RP2350.
So we need to tell GDB to connect using `target extended-remote localhost:3333`.
Most often, we execute these commands from the `build` directory (hence the `src/` prefix above).
And most often, we want the code to break on the `main` function.
We may supply gdb with these commands upon startup:
```
riscv32-unknown-elf-gdb src/PROJECT_BINARY.elf --ex "target extended-remote localhost:3333" --ex "break main"
```

## VSCode using Cortex-Debug
*Please* note, in order to use this, you have to reopen the example directory in a new VSCode Window:
`File` -> `New Window` -> `File` -> `Open Folder`.
Only then You will have the possibility to start the Cortex-Debug extension's debugger using
the left-hand RUN-AND-DEBUG Symbol using the "Debug RP2350 (RiscV)" target, or `F5` or in VSCode's Command Pallette
(get there using CTRL+SHIFT+P or on MacOS Cmd+Shift+P) and type Debug.
The VSCode extension Cortex-Debug will start an `openocd` server, which attaches to the RPI DebugProbe over USB.
Then, the extension will start a `riscv32-unknown-elf-gdb` which loads the CMake Target binary (the target with extension .elf) and
attaches to the `openocd` server and will issue gdb commands, such as `break main`.
In the opening debugger view, You will be able to view registers (left-hand pane under "Local Variables"), single-step through
the application (top-most buttons and arrows) and the like.


## GDB commands
GDB is very powerful using the command line:
1. Setting Breakpoints
    1. `break main` sets a breakpoint on function main
    2. `break LINE_NUMBER` sets a breakpoint on said line. One may combine with file-name and line: `break FILE.C:LINE_NUMBER`
    3. `break *0x12345` sets a breakpoint on said address, like when disassembling the source (see `disas` below).
    4. `del 1` removes the breakpoint number one -- as the hardware may only support 5 breakpoints, one may need to shuffle around
2. Execution
    1. `run` starts the execution from the beginning (make sure to set a break-point prior)
    2. `step` and `next`: step for one (source code) line even into function calls (see `finish`), next executes over function calls.
    3. `cont` continues until the next breakpoint (or until an error)
    4. `finish` finished this function, returning to the caller
3. Displaying
    1. `print VAR` printing variables
    2. `info reg` Show all registers
    3. `info reg a0 t0 s0` Show values of these three specific registers
    4. `x/16 0x40000` Show memory values, here 16 Bytes at address 0x40000
    5. `disas` disassembles the current function, 
For all above commands, there is help available using `help COMMAND`.
