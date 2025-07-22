# coding=utf-8

#TODO write a description for this script
#@author 
#@category _NEW_
#@keybinding 
#@menupath 
#@toolbar 


#------------------------------------------------------------------------------#
# Script: ListExternalCallees.py
# Purpose: From all functions in [0x0000..0x3FFF], find calls to functions
#          outside that range, and print a unique list.
#------------------------------------------------------------------------------#

from ghidra.program.model.symbol import RefType
from ghidra.util.task import ConsoleTaskMonitor

# 1) Define your bootloader region
BOOT_START = toAddr(0x0000)
BOOT_END   = toAddr(0xffff)

def isInBoot(addr):
    return BOOT_START <= addr <= BOOT_END

# 2) Collect all functions in the bootloader region
func_mgr   = currentProgram.getFunctionManager()
boot_funcs = [f for f in func_mgr.getFunctions(True) if isInBoot(f.getEntryPoint())]

ref_mgr = currentProgram.getReferenceManager()
external = set()

# 3) Walk instructions in each bootloader function
for func in boot_funcs:
    listing = currentProgram.getListing()
    # get instructions in the body of the function
    for instr in listing.getInstructions(func.getBody(), True):
        # for each outgoing reference from this instruction
        for ref in ref_mgr.getReferencesFrom(instr.getMinAddress()):
            if not ref.getReferenceType().isCall():
                continue
            tgt = ref.getToAddress()
            if not isInBoot(tgt):
                # find the function at that target (if any)
                ext_func = func_mgr.getFunctionAt(tgt)
                if ext_func:
                    external.add((ext_func.getName(), ext_func.getEntryPoint()))

# 4) Print sorted results
println("Found %d external callees from 0x0000 - 0xffff:" % len(external))
for name, addr in sorted(external, key=lambda x: x[1].getOffset()):
    println("  %s @ %s" % (name, addr))
