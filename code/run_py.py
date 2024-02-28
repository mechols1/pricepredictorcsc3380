import sys
import subprocess
from get_path import *

if len(sys.argv) < 2:
    print("Usage: python run_py.py <script_name> [args...]")
    sys.exit(1)

#user_path = "/Users/donovanharrison/github-classroom/hkaiserteaching/"
#user_path = "/Users/rileyoest/VS_Code/"
user_path = get_path()

env_name = f"{user_path}csc3380-fall-2023-project-group-3/env"
script_name = sys.argv[1]
script_args = " ".join(sys.argv[2:])

path_to_program = f"{user_path}csc3380-fall-2023-project-group-3/code/{script_name}"

run_command = f"conda run -p {env_name} python {path_to_program} {script_args}"

# Run the command
subprocess.run(run_command, shell=True, check=True)
