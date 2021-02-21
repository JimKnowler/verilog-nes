# parse iNes format rom files

import argparse

def parse_args():
    parser = argparse.ArgumentParser(description='parse iNes format ROM')
    parser.add_argument('--file', '-f', required=True, dest='file', help='filepath of .nes ROM file to parse')
    args = parser.parse_args()

    return args

def has_bit(value, index):
    return (value & (1 << index)) != 0

def parse_ines_header(header):
    MSDOS_EOF = 0x1A
    EXPECTED_CONSTANT = [ord('N'), ord('E'), ord('S'), MSDOS_EOF]
    for i in range(4):
        assert(header[i] == EXPECTED_CONSTANT[i])
    
    size_prg_rom = header[4]
    print("PRG ROM %d x 16 KB units" % (size_prg_rom))

    size_chr_rom = header[5]
    print("CHR ROM %d x 8 KB units" % (size_chr_rom))

    flags = header[6]
    mirroring = "horizontal" if has_bit(flags, 0) else "vertical"
    has_battery_backed_prg_ram = has_bit(flags, 1)
    has_trainer = has_bit(flags, 2)
    ignore_mirroring_control = has_bit(flags, 3)
    mapper_number = (flags >> 4) & 0x0f

    print("mirroring: ", mirroring)
    print("has batter backed prg rom: ", has_battery_backed_prg_ram)
    print("has trainer: ", has_trainer)
    print("ignore mirroring control: ", ignore_mirroring_control)
    print("mapper number: ", mapper_number)

    parsed_header = {
        "size_prg_rom" : size_prg_rom,
        "size_chr_rom" : size_chr_rom,
        "mirroring" : mirroring,
        "has_battery_backed_prg_rom": has_battery_backed_prg_ram,
        "has_trainer" : has_trainer,
        "ignore_mirroring_control": ignore_mirroring_control,
        "mapper_number": mapper_number
    }

    return parsed_header

def parse_ines(data):
    SIZE_HEADER = 16
    header = parse_ines_header(data[0:SIZE_HEADER])
    
    assert(not header['has_trainer'])
    size_prg_rom = header['size_prg_rom']
    size_chr_rom = header['size_chr_rom']
    
    expected_file_size = SIZE_HEADER + (size_prg_rom * 16 * 1024) + (size_chr_rom * 8 * 1024)
    assert(expected_file_size == len(data))

def main():
    args = parse_args()

    with open(args.file, 'rb') as file:
        data = file.read()

    parse_ines(data)

if __name__ == "__main__":
    main()
