#Dumps all maps to a json
#@author Bartek0x00
#@category Visualisation
#@runtime Jython

import json
import re
import os
from collections import OrderedDict

def export_map_symbols():
    symbolTable = currentProgram.getSymbolTable()
    map_dict = {}
    target_suffixes = ("Byte_Map2D", "Word_Map2D", "Byte_Map3D")
    
    map_pattern = re.compile(r"^(.*)_(\d+x\d+)_(Byte|Word)_Map([23]D)$")

    for symbol in symbolTable.getAllSymbols(True):
        name = symbol.getName()
        if name.endswith(target_suffixes):
            match = map_pattern.match(name)
            
            address = symbol.getAddress()
            print("Found: {} at {}".format(name, address))
            
            x_size, z_size = [int(a) for a in match.group(2).split("x")]

            #map_dict["{:06x}".format(address.getOffset())] = {
            #    "name": match.group(1) if match else "",
            #    "size_x": x_size,
            #    "size_z": z_size,
            #    "datatype": match.group(3) if match else "",
            #    "dim": match.group(4) if match else "",
            #    "partial_file_off": "",
            #    "summary": ""
            #}
     
    try:    
        out_file = askFile("Select output maps.json", "Save").getAbsolutePath()
        with open(out_file, "w") as f:
            json.dump(map_dict, f, indent=4, sort_keys=True)
        
        print("Successfully exported {} map entries to: {}".format(len(map_dict), out_file))
    except Exception as e:
        print("Export cancelled or failed: {}".format(str(e)))

def main():
    export_map_symbols()
main()
