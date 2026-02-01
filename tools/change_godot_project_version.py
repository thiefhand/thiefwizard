import sys
import os
import re

def main():
	if len(sys.argv) < 2:
		print("Usage: {} [project_path]".format(sys.argv[0]))
		return
	project_path = sys.argv[1]

	new_version = input("")
	
	# Modify project file
	proj_file = open(os.path.join(project_path, "project.godot"), "r")
	proj_text = proj_file.read()
	proj_file.close()

	proj_text = re.sub(r"config/version=\"(.*)\"", "config/version=\"{}\"".format(new_version), proj_text)

	proj_file = open(os.path.join(project_path, "project.godot"), "w")
	proj_file.write(proj_text)
	proj_file.close()

if __name__ == "__main__":
	main()