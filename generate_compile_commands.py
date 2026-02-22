import os
import json
Import("env")

def update_clangd_config():
    project_dir = env.get("PROJECT_DIR").replace("\\", "/")
    clangd_path = os.path.join(project_dir, ".clangd")
    
    # Pfade dynamisch vom System holen
    cc_path = env.get("CC").replace("\\", "/")
    toolchain_path = os.path.dirname(os.path.dirname(cc_path))
    
    # Die Pfad-Liste für die .clangd
    includes = [
        f"-I{toolchain_path}/xtensa-esp32-elf/include",
        f"-I{toolchain_path}/lib/gcc/xtensa-esp32-elf/8.4.0/include",
        f"-I{project_dir}/.platformio/packages/framework-arduinoespressif32/cores/esp32",
        f"-I{project_dir}/.platformio/packages/framework-arduinoespressif32/variants/esp32"
    ]
    
    # Die .clangd Datei komplett neu schreiben (Template)
    config_content = f"""CompileFlags:
  CompilationDatabase: "."
  Add:
    - "-std=gnu++17"
    - "-DESP32"
    - "-DARDUINO=10812"
    - "-DARDUINO_ARCH_ESP32"
    - "-DSOC_GPIO_PIN_COUNT=40"
"""
    # Includes hinzufügen
    for inc in includes:
        config_content += f"    - \"{inc}\"\n"
        
    config_content += """
  Remove: ["-mlongcalls", "-fstrict-volatile-bitfields", "-fstack-protector*", "-m64"]

Diagnostics:
  UnusedIncludes: None
  MissingIncludes: None

Index:
  Background: true
"""

    with open(clangd_path, "w") as f:
        f.write(config_content)
    print(f"--- [Clangd-Fix] .clangd wurde für System-Pfade aktualisiert ---")

# Trigger beim Laden des Skripts
update_clangd_config()












# import os
# import json
# Import("env")

# def patch_compile_commands(source, target, env):
#     # Pfad zur Datei im Projekt-Root
#     database_path = os.path.join(env.get("PROJECT_DIR"), "compile_commands.json")
    
#     if os.path.exists(database_path):
#         with open(database_path, "r") as f:
#             db = json.load(f)
        
#         # Hol den Compiler-Pfad vom System
#         cc_path = env.get("CC").replace("\\", "/")
#         toolchain_path = os.path.dirname(os.path.dirname(cc_path))
#         sys_include = f"{toolchain_path}/xtensa-esp32-elf/include"
        
#         print(f"--- [Clangd-Fix] Optimiere {len(db)} Einträge ---")
        
#         for entry in db:
#             # Backslashes killen
#             entry["command"] = entry["command"].replace("\\", "/")
#             entry["directory"] = entry["directory"].replace("\\", "/")
#             entry["file"] = entry["file"].replace("\\", "/")
            
#             # Driver und System-Header injizieren, falls noch nicht da
#             if "--query-driver" not in entry["command"]:
#                 entry["command"] += f" --query-driver={cc_path}* -I\"{sys_include}\""

#         with open(database_path, "w") as f:
#             json.dump(db, f, indent=4)
#         print("--- [Clangd-Fix] Datenbank ist jetzt bereit für clangd ---")

# # Wir führen das Skript aus, sobald der Build-Prozess startet
# patch_compile_commands(None, None, env)