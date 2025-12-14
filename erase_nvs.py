Import("env")
import os, csv, subprocess, sys

# --- NVS-Offset/Size aus partitions.csv ---
proj_dir = env['PROJECT_DIR']
partitions = env.subst("$BOARD_BUILD_PARTITIONS") or "partitions.csv"
if not os.path.isabs(partitions):
    partitions = os.path.join(proj_dir, partitions)

nvs_off = 0x9000
nvs_size = 0x5000
try:
    with open(partitions, newline='') as f:
        for row in csv.reader(f):
            if not row or row[0].strip().startswith("#"): continue
            row += ['']*(6-len(row))
            name, ptype, subtype, off, size, _ = [c.strip() for c in row[:6]]
            if name == "nvs" and ptype == "data" and subtype == "nvs":
                nvs_off = int(off, 0) if off else nvs_off
                nvs_size = int(size, 0) if size else nvs_size
                break
except Exception as e:
    print("erase_nvs.py: Warnung:", e, "-> nutze Defaults 0x%X/0x%X" % (nvs_off, nvs_size))

esptool_py = os.path.join(env['PROJECT_PACKAGES_DIR'], "tool-esptoolpy", "esptool.py")
cmd = [env.subst("$PYTHONEXE"), esptool_py,
       "--chip", env.BoardConfig().get("build.mcu","esp32"),
       "--port", env.subst("$UPLOAD_PORT") or "",
       "--baud", env.subst("$UPLOAD_SPEED") or "",
       "erase_region", "0x%X" % nvs_off, "0x%X" % nvs_size]
cmd = [c for c in cmd if c]  # leere Einträge raus

def _erase_nvs_action(*args, **kwargs):
    print("Erasing NVS 0x%X..+0x%X" % (nvs_off, nvs_size))
    res = subprocess.run(cmd)
    if res.returncode != 0:
        sys.exit(res.returncode)
    print("NVS erase OK")

env.AddPreAction("upload", _erase_nvs_action)
