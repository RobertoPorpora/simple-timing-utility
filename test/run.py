import os
import subprocess
import time

# ------------------------------------------------------------------------------

def main():
    root_folder = os.path.dirname(os.path.abspath(__file__))
    build_folder = os.path.join(root_folder, 'build')
    bin_folder = os.path.join(root_folder, 'bin')
    executable = os.path.join(bin_folder, 'test')
    if os.name == 'nt':
        executable += '.exe'

    clean_directory(build_folder)
    clean_directory(bin_folder)

    # configure
    root_folder_str = stringify_path(root_folder)
    build_folder_str = stringify_path(build_folder)
    build_system = '"Unix Makefiles"'
    if os.name == 'nt':
        build_system = '"MinGW Makefiles"'
    build_type = '-DCMAKE_BUILD_TYPE=Debug'
    run(['cmake', '-S', root_folder_str, '-B', build_folder_str, '-G', build_system, build_type])

    # build
    run(['cmake', '--build', build_folder_str])

    # run
    run([stringify_path(executable), stringified_time_micros()])


def stringified_time_micros() -> str:
    output = time.time() * 1000 * 1000
    output = int(output)
    return str(output)


# ------------------------------------------------------------------------------

def run(cmd: list[str]) -> None:
    command = None
    if isinstance(cmd, str):
        command = cmd
    elif isinstance(cmd, list) and all(isinstance(item, str) for item in cmd):
        command = ' '.join(cmd)
    else:
        raise ValueError("Argument type invalid, must be 'str' or 'list[str]'")
    
    START = '-----START-----'
    END = '-----END-----'
    
    print()
    print(f"Run: {command}")
    
    print(START)
    try:
        completed_process = subprocess.run(command, shell=True, check=True)
        print(END)
        print(f"Return code: {completed_process.returncode}")
    except subprocess.CalledProcessError as cpe:
        print(END)
        print(cpe)
    except Exception as e:
        print(END)
        print(f"Generic error: {e}")

    print()

# ------------------------------------------------------------------------------

def clean_directory(path: str) -> None:
    if os.path.exists(path):
        for root, dirs, files in os.walk(path, topdown=False):
            for name in files:
                file_path = os.path.join(root, name)
                os.remove(file_path)
            for name in dirs:
                dir_path = os.path.join(root, name)
                os.rmdir(dir_path)
        os.rmdir(path)
    os.makedirs(path, exist_ok=True)


def stringify_path(path: str) -> str:
    return f'"{path}"'

# ------------------------------------------------------------------------------

main()