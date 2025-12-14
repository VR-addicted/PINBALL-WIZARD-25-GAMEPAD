import os

# Datei für die Build-Nummer
build_file = ".build_number"

# Prüfe, ob die Datei existiert, sonst starte mit 1
if os.path.exists(build_file):
    with open(build_file, "r") as f:
        build_number = int(f.read().strip()) + 1
else:
    build_number = 1

# Schreibe die neue Build-Nummer zurück
with open(build_file, "w") as f:
    f.write(str(build_number))

print(f"Build Number: {build_number}")

# Setze das Makro für PlatformIO
Import("env")
env.Append(CPPDEFINES=[("BUILD_NUMBER", build_number)])
