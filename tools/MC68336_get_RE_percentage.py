#@category Analysis
# Calculates the percentage of functions that have been manually named
# vs. auto-generated names in the current Ghidra program.

from ghidra.program.model.symbol import SourceType
from ghidra.util.task import ConsoleTaskMonitor

def is_auto_name(name):
    """
    Check if the function name looks like an auto-generated Ghidra name.
    """
    # Ghidra often uses FUN_xxxxxxxx, LAB_xxxxxxxx, sub_xxxxx, etc.
    auto_patterns = ["FUN_", "LAB_", "sub_"]
    for p in auto_patterns:
        if name.startswith(p):
            return True
    return False

fm = currentProgram.getFunctionManager()
funcs = fm.getFunctions(True)

total = 0
named = 0

for func in funcs:
    total += 1
    name = func.getName()
    if not is_auto_name(name):
        named += 1

if total > 0:
    percent_named = (float(named) / total) * 100.0
else:
    percent_named = 0.0

print("Total functions: {}".format(total))
print("Named functions: {}".format(named))
print("Unnamed functions: {}".format(total - named))
print("Percentage named: {:.2f}%".format(percent_named))
