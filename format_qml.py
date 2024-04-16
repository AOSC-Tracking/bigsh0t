# -*- coding: utf-8 -*-
import sys

def main():
    for filename in sys.argv[1:]:
        process(filename)

def process(filename):
    lines = []
    with open(filename, "rb") as f:
        for line in f:
            line = line.decode("utf-8").rstrip()
            for part in line.split("$"):
                lines.append(part)

    new_lines = []
    BLOCK_OUTDENT = "}"
    INDENT = "[{("
    OUTDENT = "])"
    current_indent = 0
    for line in lines:
        for c in line:
            if c in BLOCK_OUTDENT:
                current_indent -= 1
        new_lines.append((" " * (4 * current_indent)) + line.strip())
        for c in line:
            if c in INDENT:
                current_indent += 1
            if c in OUTDENT:
                current_indent -= 1

    lines = new_lines
    new_lines = []
    for line in lines:
        line = line.rstrip()
        if line == "":
            if len(new_lines) > 0 and new_lines[-1] != "":
                new_lines.append(line)
        else:
            new_lines.append(line)

    lines = new_lines

    with open(filename, "wb") as f:
        for line in lines:
            f.write("{}\n".format(line).encode("utf-8"))

if __name__ == "__main__":
    main()