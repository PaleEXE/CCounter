# List struct generator for CLang with a specific type

from sys import argv, stdout
from functools import partial


def usage() -> None:
    print(f"Usage: py {argv[0]} <TYPE> [FILE] [MODE]")
    print("MODES:\n" 
          "    - [w]rite  (will overwrite your files)\n"
          "    - [a]ppend (will add in end your files)")
    exit(1)


def main() -> int:
    func_map: dict[str, list[str]] = {}

    if len(argv) <= 1:
        print("Plz add struct items type")
        usage()
    
    
    if len(argv) == 3:
        print("Add mode")
        usage()

    if len(argv) > 4:
        print(f"Bad arg: `{argv[4]}`")
        usage()

    c_file = stdout
    h_file = stdout

    if len(argv) == 4:
        mode: str = argv[3]
        if mode != "a" and mode != "w":
            print(f"Bad mode: `{mode}`")
            usage()
        
        c_file = open(argv[2] + ".c", mode=mode)
        h_file = open(argv[2] + ".h", mode=mode)

    cprint = partial(print, file=c_file)
    hprint = partial(print, file=h_file)

    struct_name: str = "List" + argv[1].title()
    struct_snake: str = "list_" + argv[1]
    items_type: str = argv[1]

    header_guard = f"{struct_name.upper()}_H"

    # === Header file preamble ===
    hprint(f"#ifndef {header_guard}")
    hprint(f"#define {header_guard}\n")
    hprint("#include <stddef.h>")
    hprint("#include <stdlib.h>")
    hprint(f"\ntypedef struct {struct_name} {{")
    hprint(f"    {items_type} *items;")
    hprint("    size_t capacity, count;")
    hprint(f"}} {struct_name};\n")

    # === C file preamble ===
    cprint("#include <stdio.h>")
    cprint("#include <stdlib.h>")
    cprint("#include <string.h>")
    if len(argv) > 2:
        cprint(f'#include "{argv[2]}.h"')
    cprint("")

    # Function declarations for header
    declarations = []

    # List creation
    func_map["new"] = [f"{struct_name} {struct_snake}_new(size_t capacity)"]
    func_map["new"].append(
        f"    {items_type} *items = ({items_type} *) malloc(capacity * sizeof({items_type}));"
    )
    func_map["new"].append(
        f"    {struct_name} list = ({struct_name}) {{.items = items, .capacity = capacity, .count = 0}};"
    )
    func_map["new"].append("    return list;")

    # Resize
    func_map["resize"] = [f"void {struct_snake}_resize({struct_name} *list)"]
    func_map["resize"].append(
        f"    {items_type} *new_items = ({items_type} *) malloc(list->capacity * 2 * sizeof({items_type}));"
    )
    func_map["resize"].append(
        f"    memcpy(new_items, list->items, list->count * sizeof({items_type}));"
    )
    func_map["resize"].append("    free(list->items);")
    func_map["resize"].append("    list->items = new_items;")
    func_map["resize"].append("    list->capacity *= 2;")

    # Append
    func_map["append"] = [
        f"void {struct_snake}_append({struct_name} *list, {items_type} value)"
    ]
    func_map["append"].append(
        f"    if (list->count == list->capacity) {struct_snake}_resize(list);"
    )
    func_map["append"].append("    list->items[list->count++] = value;")

    # Insert
    func_map["insert"] = [
        f"void {struct_snake}_insert({struct_name} *list, size_t index, {items_type} value)"
    ]
    func_map["insert"].append(
        f"    if (list->count == list->capacity) {struct_snake}_resize(list);"
    )
    func_map["insert"].append("    if (index > list->count) return;")
    func_map["insert"].append("    for (size_t i = list->count; i > index; --i)")
    func_map["insert"].append("        list->items[i] = list->items[i - 1];")
    func_map["insert"].append("    list->items[index] = value;")
    func_map["insert"].append("    list->count++;")

    # Remove
    func_map["remove"] = [
        f"void {struct_snake}_remove({struct_name} *list, size_t index)"
    ]
    func_map["remove"].append("    if (index >= list->count) return;")
    func_map["remove"].append("    for (size_t i = index; i < list->count - 1; ++i)")
    func_map["remove"].append("        list->items[i] = list->items[i + 1];")
    func_map["remove"].append("    list->count--;")

    # Print
    func_map["print"] = [f"void {struct_snake}_print(const {struct_name} *list)"]
    func_map["print"].append("    for (size_t i = 0; i < list->count; ++i)")
    func_map["print"].append('        printf("%d\\n", list->items[i]);')

    # Write both function declarations and definitions
    for fname, lines in func_map.items():
        signature = lines[0]
        declarations.append(signature + ";")

        cprint(signature)
        cprint("{")
        for line in lines[1:]:
            cprint(line)
        cprint("}\n")

    # Write declarations to header
    for decl in declarations:
        hprint(decl)
    hprint(f"\n#endif // {header_guard}")

    if c_file is not stdout:
        c_file.close()
    if h_file is not stdout:
        h_file.close()

    return 0


if __name__ == "__main__":
    main()
