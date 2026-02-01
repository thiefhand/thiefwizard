import toml
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: {} [meta_toml]".format(sys.argv[0]))
        return
    toml_path = sys.argv[1]

    meta = toml.load(toml_path)

    version_str: str = meta["meta"]["version"]
    version_comps = version_str.split(".")

    new_comp = int(version_comps[2]) + 1
    new_comps = version_comps[:-1]
    new_comps.append(str(new_comp))

    meta["meta"]["version"] = ".".join(new_comps)

    with open(toml_path, "w") as toml_file:
        toml.dump(meta, toml_file)

    print(".".join(new_comps))

if __name__ == "__main__":
    main()