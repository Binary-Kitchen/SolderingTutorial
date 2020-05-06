import os
import argparse
import glob
import jinja2
import re

img_formats = ["jpg", "jpeg", "gif", "png"]

projects = dict()

def get_project(name):
    if name not in projects:
        projects[name] = dict()
    return projects[name]

def main(args):
    dirs = args.dirs.split()

    for d in dirs:
        for fmt in img_formats:
            f = os.path.join(d, "manual/images/thumbnail.{}".format(fmt))
            get_project(d)["thumbnail"] = None
            if os.path.exists(f):
                get_project(d)["thumbnail"] = f
                break

        if get_project(d)["thumbnail"] == None:
            del projects[d]
            continue

        manuals = glob.glob(os.path.join(d,"manual/*.pdf"))
        get_project(d)["manuals"] = dict()

        for manual in manuals:
            name = os.path.basename(manual)
            name = os.path.splitext(name)[0]
            match = re.fullmatch(".*_(.*)$", name)
            lang = match.group(1)
            get_project(d)["manuals"][lang] = manual


        f = os.path.join(d, "ibom.html")
        get_project(d)["ibom"] = None
        if os.path.exists(f):
            get_project(d)["ibom"] = f

    templateLoader = jinja2.FileSystemLoader(searchpath=".")
    templateEnv = jinja2.Environment(loader=templateLoader)
    main_template = templateEnv.get_template("index_template.html")
    out = main_template.render(projects = projects)

    with open("index.html", "w") as f:
        f.write(out)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dirs")

    args = parser.parse_args()
    main(args)
