# verilog-nes

**< WORK IN PROGRESS >**

Building a Nintendo Entertainment System in Verilog



# Progress so far


# Work Remaining

- Build devkit
    - Arduino Due with LED screen
    - Arduino acting as SPI controller, FPGA as SPI peripheral
    - see [verilog-spi](https://github.com/JimKnowler/verilog-spi) project for unit tested FPGA implementation of SPI peripheral
- Test SRAM chips
- Build 6502 soft core
- Build PPU soft core
- Build VGA output
- Build HDMI output (perhaps?)
- Run + pass 6502 + NES tests, in simulation + on FPGA
