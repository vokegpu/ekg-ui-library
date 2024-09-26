from typing import List, Dict
import sys, os, zipfile, shutil


def create_release_zip_file(zip_filename: str, lib_directory: str) -> str:
    zip_output: str = f"./{zip_filename}.zip"
    to_zip_directory: str = f"./{zip_filename}/"

    os.system(f"mkdir ./{zip_filename} && cd ./{zip_filename} && mkdir ./lib && cd ..")

    shutil.copytree("./include", f"./{zip_filename}/include/")
    shutil.copy("./faq.txt", f"./{zip_filename}/")
    shutil.copy("./LICENSE.md", f"./{zip_filename}/")
    shutil.copy(f"./lib/{lib_directory}/libekg.a", f"./{zip_filename}/lib/libekg.a")

    print("zipping: ", zip_filename)

    with zipfile.ZipFile(zip_output, "w") as zipf:
        for root, dirs, files in os.walk(to_zip_directory):
            for file in files:
                zipf.write(
                    os.path.join(root, file),
                    os.path.relpath(os.path.join(root, file), to_zip_directory),
                )
    return zip_output


if __name__ == "__main__":
    print("initialising the most meow meow of live :3")
    print("mumu mumu meow meow")

    metadata: Dict[str, str] = {"-m": "", "-d": ""}
    store: str = ""

    for it in range(1, len(sys.argv)):
        if sys.argv[it] == "-m":
            store = "-m"
        elif sys.argv[it] == "-d":
            store = "-d"
        elif store:
            metadata[store] += sys.argv[it]
            metadata[store] += " "

    if metadata["-d"].endswith(" "):
        metadata["-d"] = metadata["-d"][:-1]

    print("reading metadata:")
    print(metadata)

    file = open("./devlog/commit.txt", "r")
    content: List[str] = file.read().split("\n")
    file.close()

    news_descriptor: str = ""
    version_descriptor: str = ""

    for it, element in reversed(list(enumerate(content))):
        if element.startswith("-"):
            news_descriptor += element
            news_descriptor += "\n"
        elif element.find(".") > 0:
            version_descriptor = element
            break

    release: str = f"""
# EKG {version_descriptor}

{metadata["-d"]}

News:
{news_descriptor}

---

For complete previously released logs, check [here](https://github.com/vokegpu/ekg-ui-library/blob/version-core/devlog/commit.txt).

"""

    print("making a new release")
    print(release)

    # Create tag and release.
    # But before pack/zip the faq, license, lib, and the include headers file.

    tag: str = version_descriptor.split(" ")[0]
    windows: str = f"ekg-ui-library-{tag}-win32"
    linux: str = f"ekg-ui-library-{tag}"

    create_release_zip_file(windows, "windows")
    create_release_zip_file(linux, "linux")

    os.system(f'git tag -a "{tag}" -m "{metadata["-d"]}"')
    os.system(f"git push origin {tag}")
    os.system(
        f'gh release create {tag} ./{windows}.zip ./{linux}.zip --latest --title "EKG {version_descriptor}" --notes "{release}"'
    )

    print("ok done >< mumu! :blush: ")
