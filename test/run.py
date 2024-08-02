import sys
import os
import shutil
import subprocess
import time
import ctypes


#-------------------------------------------------------------------------------

def main():
    project_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(project_dir)
    build_dir = os.path.join(project_dir, "build")
    
    print(f"Removing build directory: {build_dir}")
    shutil.rmtree(build_dir, ignore_errors=True)

    print(f"Creating build directory: {build_dir}")
    os.makedirs(build_dir, exist_ok=True)

    build_system = '\"Unix Makefiles\"'
    if os.name == "nt":
        build_system = '\"MinGW Makefiles\"'
    
    # Run cmake configure
    if not run_command(f"cmake -S \"{parent_dir}\" -B \"{build_dir}\" -G {build_system} -DBUILD_TEST=ON -DCMAKE_BUILD_TYPE=Debug"):
        return

    # Run cmake build
    if not run_command(f"cmake --build \"{build_dir}\""):
        return

    # Find generated executable
    executable = os.path.join(build_dir, "test", "test")
    if os.name == "nt":
        executable += ".exe"
    
    # run it
    run_command(f"\"{executable}\" {int(time.time() * 1000 * 1000)}")

    print("\nReached end of run.py")
    
#-------------------------------------------------------------------------------

def run_command(command):
    print(f"\nRunning command {command}\n")
    process = subprocess.Popen(
        command,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        bufsize=1,
        universal_newlines=True
    )
    
    with process.stdout:
        for line in process.stdout:
            print(line, end='')

    with process.stderr:
        for line in process.stderr:
            print(line, end='', file=sys.stderr)
    
    returncode = process.wait()

    if returncode != 0:
        print(f"ERROR: command terminated with exit code {os.strerror(returncode)}\n", file=sys.stderr)
        return False
    print("")
    return True

#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main()

#-------------------------------------------------------------------------------