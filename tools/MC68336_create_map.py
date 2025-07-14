# MC68336_create_map.py
# @author Bartek0x00
# @category Analysis
# @keybinding Ctrl-Shift-M

from __future__ import print_function

from ghidra.program.model.data import PointerDataType, ByteDataType, WordDataType, ArrayDataType
from ghidra.program.model.symbol import SourceType
from ghidra.program.model.listing import CodeUnit
from ghidra.util.exception import CancelledException

map2D_comments = (
	"in", 
	"out", 
	"size", 
	"values", 
	"data"
)

map3D_comments = (
	"in_y", 
	"in_x", 
	"out", 
	"y", 
	"x", 
	"y_values",
	"x_values",
	"data"
) 

def clear_labels_and_types(addr, size = 1):
	currentProgram.getListing().clearCodeUnits(addr, addr.add(size - 1), False)

def create_data_with_comments(addr, data, comment):
	if not isinstance(data, ArrayDataType):
		data = data.dataType

	clear_labels_and_types(addr, data.getLength())
	createData(addr, data)
	codeUnit = currentProgram.getListing().getCodeUnitAt(addr)
	if codeUnit:
		codeUnit.setComment(codeUnit.PRE_COMMENT, comment)
	return addr.add(data.getLength())
	
def map2D(addr):
	lbl_addr = addr

	listing = currentProgram.getListing()
	mem = currentProgram.getMemory()
	
	addr = create_data_with_comments(addr, PointerDataType, map2D_comments[0])
	addr = create_data_with_comments(addr, PointerDataType, map2D_comments[1])
	size = mem.getShort(addr)
	addr = create_data_with_comments(addr, WordDataType, map2D_comments[2])
	addr = create_data_with_comments(addr, ArrayDataType(ByteDataType.dataType, size, 1), map2D_comments[3])
	addr = create_data_with_comments(addr, ArrayDataType(ByteDataType.dataType, size, 1), map2D_comments[4])
	
	setReusePreviousChoices(False)
	try:
		lbl_text = askString("Map2D label name", "Give this Map2D a meaningful name or leave the default (address)", str(lbl_addr))
	except CancelledException:
		lbl_text = str(lbl_addr)

	createLabel(lbl_addr, "{}_{}x1_Map2D".format(lbl_text, size), True)

def map3D(addr):
	lbl_addr = addr

	listing = currentProgram.getListing()
	mem = currentProgram.getMemory()
	
	addr = create_data_with_comments(addr, PointerDataType, map3D_comments[0])
	addr = create_data_with_comments(addr, PointerDataType, map3D_comments[1])
	addr = create_data_with_comments(addr, PointerDataType, map3D_comments[2])
	y_size = mem.getByte(addr)
	addr = create_data_with_comments(addr, ByteDataType, map3D_comments[3])
	x_size = mem.getByte(addr)
	addr = create_data_with_comments(addr, ByteDataType, map3D_comments[4])
	addr = create_data_with_comments(addr, ArrayDataType(ByteDataType.dataType, y_size, 1), map3D_comments[5])
	addr = create_data_with_comments(addr, ArrayDataType(ByteDataType.dataType, x_size, 1), map3D_comments[6])
	addr = create_data_with_comments(addr, ArrayDataType(ByteDataType.dataType, y_size * x_size, 1), map3D_comments[7])	
	
	setReusePreviousChoices(False)
	try:
		lbl_text = askString("Map3D label name", "Give this Map3D a meaningful name or leave the default (address)", str(lbl_addr))
	except CancelledException:
		lbl_text = str(lbl_addr)

	createLabel(lbl_addr, "{}_{}x{}_Map3D".format(lbl_text, y_size, x_size), True)

def pointer_starts_with(addr, prefix=0x00FF):
	mem = currentProgram.getMemory()
	return ((mem.getInt(addr) & 0xFFFF0000) >> 16) == prefix

def run():
	addr = currentAddress
	
	if not pointer_starts_with(addr):
		print("1st Pointer does not start with 0x00FF, aborting...")
		return
	
	if not pointer_starts_with(addr.add(PointerDataType.dataType.getLength())):
		print("2nd Pointer does not start with 0x00FF, aborting...")
		return
	
	if not pointer_starts_with(addr.add(2 * PointerDataType.dataType.getLength())):
		map2D(addr)
	else:
		map3D(addr)

run()

