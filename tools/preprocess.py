#!/usr/bin/python3
import re
import sys

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

for subst in sys.argv[2:]:
    with open(subst, 'r') as f:
    	for macro in [x for x in f.read().split("#MACRO:\n") if x]:
	        contents_to_replace = apply_spec(contents_to_replace, macro)

with open(sys.argv[1].replace(".meta.c", ".c"), 'w') as f:
    f.write(contents_to_replace)
