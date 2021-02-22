# parse iNes format rom files

import argparse
from PIL import Image

SIZE_INES_HEADER = 16
SIZE_PRG_ROM_BANK = 16 * 1024
SIZE_CHR_ROM_BANK = 8 * 1024

def parse_args():
    parser = argparse.ArgumentParser(description='parse iNes format ROM')
    parser.add_argument('--file', '-f', required=True, dest='file', help='filepath of .nes ROM file to parse')
    parser.add_argument('--chr', '-c', dest='export_chr_rom', action='store_true', help='export character ROM as PNG files' )
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

def validate_ines(data, header):
    assert(not header['has_trainer'])
    size_prg_rom = header['size_prg_rom']
    size_chr_rom = header['size_chr_rom']
    
    expected_file_size = SIZE_INES_HEADER + (size_prg_rom * SIZE_PRG_ROM_BANK) + (size_chr_rom * SIZE_CHR_ROM_BANK)
    assert(expected_file_size == len(data))

def export_ines_chr_rom_bank(index, chr_data):
    BYTES_PER_TILE = 16         # 8 bits per row * 8 rows * 2 bits per pixels
    NUM_TILES = int(len(chr_data) / BYTES_PER_TILE)

    NUM_TILES_X = 32
    NUM_TILES_Y = 16
    assert(NUM_TILES == (NUM_TILES_X * NUM_TILES_Y))

    image = Image.new("L", (8 * NUM_TILES_X, 8 * NUM_TILES_Y), 0)

    tile_data_offset = 0
    for tile_x in range(NUM_TILES_X):
        for tile_y in range(NUM_TILES_Y):
            tile_data = chr_data[tile_data_offset: tile_data_offset + BYTES_PER_TILE]
            chr_x = tile_x * 8
            chr_y = tile_y * 8
            for x in range(8):
                for y in range(8):
                    bit0 = (tile_data[y] >> (7-x)) & 0x1
                    bit1 = (tile_data[8+y] >> (7-x)) & 0x1

                    pixel = (bit0 << 6) | (bit1 << 7)
                    image.putpixel((chr_x + x,chr_y + y), pixel)

            tile_data_offset += BYTES_PER_TILE
    
    filename = "chr_rom_bank_%d.png" % (index)
    image.save(filename)

def export_ines_chr_rom(data, header):
    size_prg_rom = header['size_prg_rom']
    offset_chr_rom = SIZE_INES_HEADER + (size_prg_rom * SIZE_PRG_ROM_BANK)
    for i in range(header['size_chr_rom']):
        bank_offset = offset_chr_rom + (i * SIZE_CHR_ROM_BANK)
        export_ines_chr_rom_bank(i, data[bank_offset: bank_offset + SIZE_CHR_ROM_BANK])

def parse_ines(data, export_chr_rom):
    header = parse_ines_header(data[0:SIZE_INES_HEADER])
    
    validate_ines(data, header)

    if export_chr_rom:
        export_ines_chr_rom(data, header)

def main():
    args = parse_args()

    with open(args.file, 'rb') as file:
        data = file.read()

    parse_ines(data, args.export_chr_rom)

if __name__ == "__main__":
    main()
