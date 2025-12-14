import sys
import re
import os

filename = sys.argv[1] if len(sys.argv) > 1 else "<unknown>"


#protected_single = re.compile(r'//!\s*', re.UNICODE)
protected_single = re.compile(r'//!')                  # findet in ganzer Zeile
#protected_single = re.compile(r'^\s*//!')              # findet nur am Anfang


protected_multi_start = re.compile(r'^\s*/\*!')   # /*!   = start block
protected_multi_end = re.compile(r'\*/')          # */    = ende block

# Normale Kommentare
single_comment = re.compile(r'//.*')
multi_comment = re.compile(r'/\*.*?\*/', re.DOTALL)

def remove_comments(contents):
    lines = contents.splitlines(keepends=True)
    result = []
    in_protected_block = False

    for line in lines:

        # 1) Schutz: Multi-Zeilenblock (/*! ... */)
        if protected_multi_start.match(line):
            in_protected_block = True
            result.append(line)
            continue

        if in_protected_block:
            result.append(line)
            if protected_multi_end.search(line):
                in_protected_block = False
            continue

        # 2) Schutz: //! einzelne Zeile darf NICHT verändert werden
        #if protected_single.match(line):
        if protected_single.search(line):
            result.append(line)
            continue

        # 3) Normale Kommentare entfernen
        cleaned = multi_comment.sub('', line)
        cleaned = single_comment.sub('', cleaned)

        result.append(cleaned)

    return ''.join(result)

original = sys.stdin.read()
cleaned = remove_comments(original)

orig_size = len(original)
new_size = len(cleaned)
diff = orig_size - new_size

sys.stderr.write(
    f"[stripcomments] Datei: {filename} | Original: {orig_size} Bytes → Neu: {new_size} Bytes | -{diff} Bytes\n"
)

sys.stdout.write(cleaned)



