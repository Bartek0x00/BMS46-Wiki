# Dumps all variables to a json
#@author Bartek0x00
#@category Visualisation

import json
from collections import OrderedDict
from ghidra.program.model.symbol import SymbolType

def find_symbols():
    listing = currentProgram.getListing()
    symbol_table = currentProgram.getSymbolTable()
    addr_factory = currentProgram.getAddressFactory().getDefaultAddressSpace()   

    result = read_json()
    seen = set()
    searched = ["in", "out", "in_z", "in_x", "out_y"]
    
    addr = toAddr(0x8164)
    end_addr = toAddr(0xae78)

    while addr <= end_addr:
        code_unit = listing.getCodeUnitAt(addr)
        if not code_unit:
            addr = addr.next()
            continue
        
        comment = code_unit.getComment(code_unit.PRE_COMMENT)
        if comment and comment.lower() in searched:
            data = listing.getDataAt(addr)
            if data:
                deref = data.getValue()
                label_addr = int("0x{}".format(deref), 16)
                key = "{:x}".format(deref.getOffset())
                symbol = symbol_table.getPrimarySymbol(addr_factory.getAddress(label_addr))
                
		label_name = symbol.getName() if symbol else ""
                data = listing.getDataAt(addr_factory.getAddress(label_addr))
                if data:
                   type_name = data.getDataType().getName()
              
                if key not in seen:
                    seen.add(key)
                    result[key]["name"] = label_name

                    #result[key] = {
                    #    "name": label_name,
                    #    "type": type_name,
                    #    "factor": 1.000,
                    #    "off": 0.000,
                    #    "unit": ""
                    #}

        addr = addr.next()
    sorted_result = OrderedDict(
        sorted(result.items(), key=lambda x: int(x[0], 16))
    )
    return sorted_result

outPath = None

def update_json(json):
    listing = currentProgram.getListing()
    symbol_table = currentProgram.getSymbolTable()
    for key in sorted(json):
        addr = toAddr(key)
        symbol = symbol_table.getPrimarySymbol(addr)
        label = symbol.getName()
        if label != json[key]["name"]:
            new_label = askString("New name for symbol " + "0x" + str(key), json[key]["name"] + " -> ", label)
            print(json[key]["name"] + " -> " + new_label)
            json[key]["name"] = new_label

def read_json():
    global outPath
    outPath = askFile("Select location of variables.json", "Load")
    with open(outPath.absolutePath, "r") as f:
        return json.load(f)

def save_json(data):
    global outPath
    with open(outPath.absolutePath, "w") as f:
        json.dump(data, f, indent=4, sort_keys=True)

def main():
    #save_json(find_symbols())
    variables = read_json()
    update_json(variables)
    save_json(variables)

main()
