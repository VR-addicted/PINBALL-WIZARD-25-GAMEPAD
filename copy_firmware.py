import shutil
from pathlib import Path
import sys


    # --- Parameter auslesen ---
    
def after_build(source, target, env):
    mode_map = {
        "esp32dev": "dev",
        "esp32dbg": "dbg",
        "esp32stable": "stable"
    }
    pioenv = env.get("PIOENV", "")
    mode = mode_map.get(pioenv)

    if mode is None:
        print(f"[copy_firmware] ERROR: Unbekanntes Environment '{pioenv}' – Abbruch")
        return


    if mode not in ["dev", "dbg", "stable"]:
        print("[copy_firmware] ERROR: Ungültiger oder fehlender Parameter. Nutze --dev, --dbg oder --stable")
        return

    build_dir = Path(env.subst("$BUILD_DIR"))
    out_dir = Path("FIRMWARE-DOWNLOAD")
    out_dir.mkdir(exist_ok=True)

    webtool_dir = Path("../PINBALL-WIZARD-25-WEB-TOOL")
    if not webtool_dir.exists():
        print(f"[copy_firmware] WARNUNG: Webtool-Verzeichnis {webtool_dir} nicht gefunden")
    else:
        print(f"[copy_firmware] Webtool-Ziel: {webtool_dir.resolve()}")

    # --- Buildnummer lesen ---
    build_num_file = Path(".build_number")
    if not build_num_file.exists():
        print("[copy_firmware] ERROR: .build_number fehlt")
        return
    build_num = build_num_file.read_text().strip()

    # --- Firmware kopieren ---
    fw_src = build_dir / "firmware.bin"
    if not fw_src.exists():
        print(f"[copy_firmware] ERROR: Firmware nicht gefunden: {fw_src}")
        return

    fw_name = f"pbw25-{mode}.bin"
    version_file = f"pbw25-{mode}.{build_num}"
    version_txt = f"pbw25-{mode}.txt"

    # Ziel 1: FIRMWARE-DOWNLOAD
    dst1 = out_dir / fw_name
    shutil.copy(fw_src, dst1)
    print(f"[copy_firmware] Kopiert {fw_src} -> {dst1}")

    (out_dir / version_file).touch()
    print(f"[copy_firmware] Erstellt {version_file} in {out_dir}")

    # Ziel 2: Webtool-Projekt
    if webtool_dir.exists():
        dst2 = webtool_dir / fw_name
        shutil.copy(fw_src, dst2)
        print(f"[copy_firmware] Kopiert {fw_src} -> {dst2}")

        (webtool_dir / version_file).touch()
        print(f"[copy_firmware] Erstellt {version_file} in Webtool")

        (webtool_dir / version_txt).write_text(build_num + "\n")
        print(f"[copy_firmware] Version geschrieben in {version_txt}")

# Hook registrieren
Import("env")
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", after_build)
















