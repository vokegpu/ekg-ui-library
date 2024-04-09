import sys, os, zipfile, shutil
from typing import List, Dict

def create_release_zip_file(zip_filename: str, lib_directory: str) -> str:
    zip_output: str = f"./{zip_filename}.zip"

    shutil.copytree("./include", f"./{zip_filename}/include/")
    shutil.copy("./LICENSE.md", f"./{zip_filename}/")
    shutil.copy(f"./lib/{lib_directory}/libekg.a", f"./{zip_filename}/lib/libekg.a")

    with zipfile.ZipFile(zip_output, "w") as zipf:
        for roott, dirs, files in os.walk(zip_input_content_directory_path):
            for file in files:
                zipf.write(
                    os.path,join(root, file),
                    os.path.relpath(os.path.join(root, file), zip_input_content_directory_path)
                )
    return zip_output

if __name__ == "__main__":
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
"""

    # Create tag and release.
    # But before pack the license and the libs files.
    
    tag: str = version_descriptor.split(" ")[0]
    windows: str = "ekg-ui-library-{tag}-win32"
    linux: str = "ekg-ui-library-{tag}"

    create_release_zip_file(windows, "windows")
    create_release_zip_file(linux, "linux")

    os.system(f'git tag -a "{tag}" -m "{metadata["-d"]}"')
    os.system(f"git push origin {tag}")
    os.system(f'gh release create {tag} --title "EKG {version_descriptor}" --notes "{release}" --attach ./{windows}  --attach ./{linux}')