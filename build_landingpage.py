import os
import argparse
import glob
import jinja2
import re

img_formats = ["jpg", "jpeg", "gif", "png", "webp"]

# Every kit keeps its card image here (this is the repo-wide convention).
# Formats that already occur in the repo: jpg, jpeg, png, gif, webp.
thumbnail_candidates = [
    "manual/images/thumbnail.{fmt}",
]

projects = dict()

def get_project(name):
    if name not in projects:
        projects[name] = dict()
    return projects[name]


def find_thumbnail(d):
    for pattern in thumbnail_candidates:
        for fmt in img_formats:
            f = os.path.join(d, pattern.format(fmt=fmt))
            if os.path.exists(f):
                return f.replace(os.sep, "/")
    return None


def auto_discover():
    """All top-level folders that look like a project (i.e. have a card image)."""
    found = []
    for entry in sorted(os.listdir(".")):
        if not os.path.isdir(entry) or entry.startswith("."):
            continue
        if entry.lower() in ("resources", "misc", "node_modules"):
            continue
        if find_thumbnail(entry):
            found.append(entry)
    return found


def anchor(name):
    """Slug used for the deep link (#anchor) of a project card."""
    return re.sub(r"[^A-Za-z0-9_-]+", "-", name).strip("-")


def main(args):
    dirs = args.dirs.split() if args.dirs else []
    for d in auto_discover():
        if d not in dirs:
            dirs.append(d)
    # de-duplicate, alphabetical order keeps the page stable between builds
    dirs = sorted(set(dirs))

    for d in dirs:
        get_project(d)["thumbnail"] = find_thumbnail(d)
        if get_project(d)["thumbnail"] is None:
            del projects[d]
            continue

        manuals = sorted(glob.glob(os.path.join(d, "manual/*.pdf")))
        get_project(d)["manuals"] = dict()

        for manual in manuals:
            name = os.path.splitext(os.path.basename(manual))[0]
            match = re.fullmatch(".*_(.*)$", name)
            if match:
                lang = match.group(1)
            else:
                print(f"Warning: No language code found in the name of the file '{name}'")
                lang = name
            get_project(d)["manuals"][lang] = manual.replace(os.sep, "/")

        f = os.path.join(d, "ibom.html")
        get_project(d)["ibom"] = None
        if os.path.exists(f):
            get_project(d)["ibom"] = f.replace(os.sep, "/")

        get_project(d)["anchor"] = anchor(d)

    templateLoader = jinja2.FileSystemLoader(searchpath=".")
    templateEnv = jinja2.Environment(loader=templateLoader)
    main_template = templateEnv.get_template("index_template.html")
    out = main_template.render(projects = projects)

    with open("index.html", "w", encoding="utf-8", newline="\n") as f:
        f.write(out)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dirs", nargs="?", default=None,
                        help="optional space separated list of project folders; "
                             "when omitted every folder with a card image is used")

    args = parser.parse_args()
    main(args)
