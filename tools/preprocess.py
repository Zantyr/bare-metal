#!/usr/bin/python3
import os
import re
import sys

DEPTH_OF_INSPECTION = 3

def list_files():
    s = ""
    for file in os.listdir("filesystem"):
        s += "add_txt_file(\"{}\", \"{}\");\n".format(
            file,
            open(file).read()
        )
    return s

def apply_spec(text, macro):
	name, args = re.findall("([A-Za-z0-9_]+)\(([A-Za-z0-9_ ,]+)\)", macro)[0]
	args = [x.strip() for x in args.split(',')]
	print("Applying {}({})".format(name, len(args)))
	macro_text = macro.split('\n', 1)[1]
	for numer, arg in enumerate(args):
		macro_text = macro_text.replace("<{}>".format(arg), "\\" + str(numer + 1))
	catch = "{}\({}\)".format(name, ",".join(["\s*([A-Za-z0-9_]+)\s*"] * len(args)))
	return re.sub(catch, macro_text, text)

with open(sys.argv[1], 'r') as f:
    contents_to_replace = f.read()

for times in range(DEPTH_OF_INSPECTION):
    for subst in sys.argv[2:]:
        with open(subst, 'r') as f:
            for macro in [x for x in f.read().split("#MACRO:\n") if x]:
                contents_to_replace = apply_spec(contents_to_replace, macro)

CBACKS = {"files": list_files}

for match in re.findall("{%(.*)%}", contents_to_replace):
    cb = CBACKS.get(match.strip())
    if cb:
        print(match)
        contents_to_replace = contents_to_replace.replace("{%" + match + "%}", cb())

with open(sys.argv[1].replace(".meta.c", ".c"), 'w') as f:
    f.write(contents_to_replace)
