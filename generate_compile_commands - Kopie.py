import subprocess

def before_build(source, target, env):
    subprocess.call(["pio", "run", "-t", "compiledb"])
