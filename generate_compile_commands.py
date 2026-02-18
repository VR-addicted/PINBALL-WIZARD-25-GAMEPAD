import os
import shutil

def before_build(source, target, env):
    cc_path = os.path.join(env['PROJECT_BUILD_DIR'], 'compile_commands.json')
    if os.path.exists(cc_path):
        shutil.copy(cc_path, env['PROJECT_DIR'])
