# MC68336_visualize_map.py
# @author Bartek0x00
# @category Visualization
# @keybinding Ctrl-Shift-V

import os, time, urllib
from java.awt import Desktop
from java.net import URI
from java.lang import Runtime

map_data = {
    "label": "0000a0f4_16x1_Byte_Map2D",
    "in": "Engine_RPM",
    "out": "DAT_00ff83bb",
    "size": 16,
	"values": [
        12, 25, 30, 35,
        50, 65, 75, 90,
        100, 105, 115, 125,
        130, 140, 150, 157
    ],
    "data": [
        36, 37, 38, 39,
        44, 51, 57, 68,
        77, 83, 93, 101,
        104, 108, 110, 111
	]
}

def to_json(v):
    if isinstance(v, basestring):
        return '"%s"' % v.replace('"', '\\"')
    if isinstance(v, bool):
        return "true" if v else "false"
    if isinstance(v, (int, long, float)):
        return str(v)
    if isinstance(v, dict):
        items = []
        for k, val in v.items():
            items.append(to_json(k) + ":" + to_json(val))
        return "{" + ",".join(items) + "}"
    if isinstance(v, (list, tuple)):
        return "[" + ",".join(to_json(x) for x in v) + "]"
    return 'null'

json_str = to_json(map_data)

tmp_dir = os.getenv("TMPDIR") or "/tmp"

if not os.path.isdir(tmp_dir):
    os.makedirs(tmp_dir)

fname = "ghidra_map_%d.json" % os.getpid()
fpath = os.path.join(tmp_dir, fname)

with open(fpath, "w") as out:
    out.write(json_str)

viewer_html = os.path.expanduser("/home/manjaro/Projects/BMS46-Wiki/docs/map_viewer.html")

enc = urllib.quote(fpath, safe="")

cmd = ["firefox", "--new-window",
		"file://" + viewer_html + "?map=" + enc]

try:
	Runtime.getRuntime().exec(cmd)
except Exception, e:
    print("[!] Firefox failed to launch: ", e)
    print("    Open this in your browser:")
    print("    file://%s?map=%s" % (viewer_html, enc))

