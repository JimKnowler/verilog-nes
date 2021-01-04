# verilog-nes

**< WORK IN PROGRESS >**

Building a [Nintendo Entertainment System](https://en.wikipedia.org/wiki/Nintendo_Entertainment_System) in Verilog.

Aim: to play the original NES [Super Mario Bros](https://en.wikipedia.org/wiki/Super_Mario_Bros.) ROM.

# Building

> bazel build //nes:test

> ./bazel-bin/nes/test

# Components

- FPGA
  - [TinyFPGA](https://www.digikey.co.uk/catalog/en/partgroup/tinyfpga-bx-board/81752?utm_adgroup=General&utm_source=google&utm_medium=cpc&utm_campaign=Dynamic%20Search_EN_Product&utm_term=&productid=&gclid=Cj0KCQiAoab_BRCxARIsANMx4S4ueX8bpfZlRSGOc2a1DaLXHSEJ57sAfxrhEePddjsz5_TzLCkDxRgaAlBJEALw_wcB)
    - easy to program via open source tool chain
    - but .. only has 24 pins, and we need 27 (16+8+3) pins just for external memory
  - [CMOD A7](https://reference.digilentinc.com/reference/programmable-logic/cmod-a7/reference-manual)
    - 44 digital I/O pins
    - onboard 12MHz clock
      - can *not* drive PLL
      - can drive MMCM (read: [7 Series FPGAs Clocking Resources User Guide](https://www.xilinx.com/support/documentation/user_guides/ug472_7Series_Clocking.pdf))
- SRAM
  - Note: CMOD A7 has 512 KBytes of [onboard memory](http://www.issi.com/WW/pdf/61-64WV5128Axx-Bxx.pdf?_ga=2.108008578.1610916471.1609185004-1649949881.1605016660)
    - 8ns access time
    - see: Xilinx External Memory Controller IP core
    - For pin-mappings between the FPGA and SRAM, please see the [CmodA7 Master XDC](https://github.com/Digilent/digilent-xdc/blob/master/Cmod-A7-Master.xdc) available on the Cmod A7 Resource Center
    - [using SRAM memory on FPGA](https://www.hackster.io/salvador-canas/a-practical-introduction-to-sram-memories-using-an-fpga-i-3f3992?_ga=2.78132372.1610916471.1609185004-1649949881.1605016660)
  - [LYONTEK LY62256PL-55LLI SRAM](https://www.ebay.co.uk/itm/171951203027)
    - 55ns access time
    - can be driven directly from Arduino without FPGA in between
      - FPGA SPI protocol could be used to disconnect FPGA from memory bus (i.e. set memory bus I/O pins to high-Z)

# Work Remaining

- Draw circuit diagrams
  - Memory Controller
  - Devkit
  - CPU/PPU cores
  - VGA output
- Build devkit
    - Arduino Due (3.3v) with LED screen
    - Arduino acting as SPI controller, FPGA as SPI peripheral
    - see [verilog-spi](https://github.com/JimKnowler/verilog-spi) project for unit tested FPGA implementation of SPI peripheral
    - SPI protocol
      - pause/resume 6502 execution (while PPU continues to run independently)
      - read/write RAW memory addresses (i.e. bypass memory controller)
      - read external state of 6502 (address, data, r/w)
      - read internal state of 6502 (PC, a/x/y register, status flags)
      - read / write memory map configuration
        - location of NES PRG ROM / CHR ROM / RAM in 
- Test external SRAM chips
  - Arduino script that runs memory tests
  - test using ~CS as 16th bit of address so that 2 x 32kB chips act as single 64kB memory
- Build Memory Controller
  - multiplex request from multiple sources to a single external memory bus
    - CPU (R + W)
    - PPU (R)
    - Devkit RAW (R + W)
    - Devkit Mapped (R + W)
- Build Memory Map / TLB
  - combinatorial logic to map addresses of data requests from CPU / PPU to RAW address in external SRAM
    - memory requests pass through directly to Memory Controller
  - handle 'memory mapper' logic of the currently loaded cartridge
    - intercept memory access from CPU that writes to cartridge's bank switcher or timer / interrupt generator
- Build Master Clock
  - run Memory Controller at 100MHz
  - run PPU at 5MHz
  - run CPU at 1.66MHz
  - run VGA at 25 MHz
  - implmentation
    - clock divider? - easy to write, but issues with timings? need to edit XDC manually
    - Vivado clock generator? - need to use vivado wizard but it handles timings for us
- Build 6502 soft core
  - cycle timings are documented in [Synertek hardware manual](http://archive.6502.org/datasheets/synertek_hardware_manual.pdf) in appendix A
  - [6502 opcode reference](http://www.obelisk.me.uk/6502/reference.html)
  - [Visualizing CPU Timing](https://laughtonelectronics.com/Arcana/Visualizing%2065xx%20Timing/Visualizing%2065xx%20CPU%20Timing.html)
  - [Transparent Latches, not registers!](http://forum.6502.org/viewtopic.php?f=4&t=4651)
- Build PPU soft core
- Build VGA output
- Build HDMI output (perhaps?)
- Run + pass 6502 + NES tests, in simulation + on FPGA
- Add NES joypad interface
- Play Super Mario
- Audio synthesis

# Questions

- How are games loaded into SRAM?
  - directly from Arduino to SRAM chips
  - via SPI 
    - Arduino would parse [iNES](https://wiki.nesdev.com/w/index.php/INES) header
    - Arduino would write game + character roms to SRAM via SPI
    - Arudino would write memory map configuration via SPI
    - Concern: this could be slow
      - optimisation: extend SPI to quad-SPI to x4 speed

# References
- Vivado - [XDC Constraints](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2014_1/ug903-vivado-using-constraints.pdf)
- [Program CMOD A7](https://reference.digilentinc.com/learn/programmable-logic/tutorials/cmod-a7-programming-guide/start)