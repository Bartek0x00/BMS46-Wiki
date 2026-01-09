#!/usr/bin/env python3
import sys
from collections import defaultdict

# ANSI escape for red
RED = "\033[91m"
RESET = "\033[0m"

def hexdump(data: bytes, base_offset=0, width=16, highlights=None):
    """
    Generate a hexdump string for data.
    highlights = dict {offset: True} marking bytes to color red.
    """
    lines = []
    for i in range(0, len(data), width):
        chunk = data[i:i+width]
        hex_bytes = []
        ascii_repr = []
        for j, b in enumerate(chunk):
            pos = i + j
            if highlights and pos in highlights:
                hex_bytes.append(f"{RED}{b:02X}{RESET}")
                ascii_repr.append(f"{RED}{(chr(b) if 32 <= b < 127 else '.')}{RESET}")
            else:
                hex_bytes.append(f"{b:02X}")
                ascii_repr.append(chr(b) if 32 <= b < 127 else '.')
        hex_part = " ".join(hex_bytes)
        ascii_part = "".join(ascii_repr)
        lines.append(f"{base_offset+i:08X}: {hex_part:<{width*3}} {ascii_part}")
    return "\n".join(lines)


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} start_hex end_hex")
        sys.exit(1)

    start = int(sys.argv[1], 0)
    end   = int(sys.argv[2], 0)
    files = [
		"01119700EF00Q20G.bin",
		"01119700F000O2AG.bin",
		"01119700F000P2AG.bin",
		"01119700F000P2FG.bin",
		"01119700F000Q22I.bin",
		"01119700F000Q28G.bin",
		"01119700F000Q2AG.bin",
		"01119700F000Q2CG.bin",
		"TUNED_01119700F000O2AG.bin",
		"TUNED_01119700F000Q22I.bin"
	]

    # Read the region from each file
    contents = {}
    for fname in files:
        with open(fname, "rb") as f:
            f.seek(start)
            contents[fname] = f.read(end - start)

    # Group files by identical content
    groups = defaultdict(list)
    for fname, data in contents.items():
        groups[data].append(fname)

    # Compare to find differing bytes
    all_data = list(contents.values())
    length = len(all_data[0])
    differing_positions = set()
    for i in range(length):
        values = {data[i] for data in all_data}
        if len(values) > 1:
            differing_positions.add(i)

    # Print grouped dumps
    for data, fnames in groups.items():
        print("="*80)
        print("Files:", ", ".join(fnames))
        dump = hexdump(data, base_offset=start, highlights=differing_positions)
        print(dump)
        print()

if __name__ == "__main__":
    main()

