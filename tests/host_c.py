"""Build host executables from whole production translation units.

Host fixtures compile the real ``src/*.c`` file against the real headers and
link it with the harness.  Nothing is sliced out of production text and no
production type is re-declared: the harness only defines symbols that live in
*other* translation units (boundary stubs, with their real prototypes).

Linking dead-strips unreachable code, so only the functions a harness actually
calls must resolve.  When production code grows a new cross-unit dependency the
link fails and ``HostBuildError`` names the missing symbols, instead of the
fixture silently testing a stale copy.
"""
from __future__ import annotations

import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

# Preprocessor configuration of the release ROM (Makefile CPPFLAGS).
DEFINES = {
    "MODERN": "1",
    "TESTING": "0",
    "EMERALD": "1",
    "RELEASE": "1",
    "EC_HEADLESS_FIXTURES": "0",
}

# Forced first include.  It only neutralises GBA linker placement (section
# names are not valid Mach-O/ELF host sections) and supplies the IDE-mode text
# macros global.h already defines for macOS/Cygwin hosts; it declares no
# production type.  The host C library comes first because global.h defines
# libc-named macros (abs) that would break a later <stdlib.h>.
SHIM = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gba/defines.h"
#undef IWRAM_DATA
#undef EWRAM_DATA
#undef IWRAM_INIT
#undef EWRAM_INIT
#undef COMMON_DATA
#undef KEEP_SECTION
#undef ARM_FUNC
#define IWRAM_DATA
#define EWRAM_DATA
#define IWRAM_INIT
#define EWRAM_INIT
#define COMMON_DATA
#define KEEP_SECTION
#define ARM_FUNC
#if !defined(__APPLE__) && !defined(__CYGWIN__) && !defined(__INTELLISENSE__)
#define __INTELLISENSE__ 1
#endif
#define HOST_EXPORT __attribute__((visibility("default")))
'''

# config/general.h defines NDEBUG for RELEASE, which would silently compile out
# every harness assert().  Units re-enable it after the production headers;
# build() refuses a unit that ends with NDEBUG still defined.
ASSERTS = "#undef NDEBUG\n#include <assert.h>\n"
NDEBUG_GUARD = "\n#ifdef NDEBUG\n#error host harness asserts are disabled: add host_c.ASSERTS after the production headers\n#endif\n"

SANITIZE = ("-fsanitize=undefined", "-fno-sanitize-recover=undefined")


class HostBuildError(AssertionError):
    pass


def compiler() -> str:
    found = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    if found is None:
        raise HostBuildError("host C compiler (cc, clang or gcc) required")
    return found


def production(relative: str) -> str:
    """Complete production source text, positioned for diagnostics.

    Read through ``Path.read_text`` so callers can audit mutated source.  The
    unit's own quoted includes resolve against the production directory.
    """
    path = ROOT / relative
    return (f'#line 1 "{relative}"\n' + path.read_text()
            + f'\n#line 1 "<harness after {relative}>"\n' + ASSERTS)


# Symbols the real gItemsInfo only points at (icons, bag-menu callbacks).
ITEM_DATA_INERT = (r"gItemIcon\w*", r"ItemUseOutOfBattle_\w+")


def item_data_unit(*harness_owned: str) -> str:
    """The whole of src/item.c, linked for its real gItemsInfo.

    Functions the harness defines itself (for example the bag query) are
    renamed within this unit only, so its copies are dead-stripped.  Build with
    ``inert=ITEM_DATA_INERT``.
    """
    return "".join(f"#define {name} HostItemC_{name}\n" for name in harness_owned) + production("src/item.c")


def species_form_boundary(writable: bool = False) -> str:
    """pokemon.c boundary unit: gSpeciesInfo form tables of the configured build.

    Real species data needs make-generated learnsets, so the boundary carries
    only ``formSpeciesIdTable`` (read from the configured species headers) and
    GetFormSpeciesId over it.  ``writable`` lets a harness vary other fields
    through this unit; pokemon.h's const declaration is renamed away here only.
    Callers append further boundary definitions to the returned unit text.
    """
    sys.path.insert(0, str(ROOT / "scripts"))
    from verify_trainer_ability_legality import SPECIES_MARKER, preprocess_species_info

    species = preprocess_species_info().split("const struct SpeciesInfo gSpeciesInfo[]", 1)[1]
    markers = list(SPECIES_MARKER.finditer(species))
    pointers = []
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(species)
        form = re.search(r"\.formSpeciesIdTable\s*=\s*(s\w+)", species[marker.end():end])
        if form:
            pointers.append(f"    [{marker[1]}] = {{.formSpeciesIdTable = {form[1]}}},")
    if len(pointers) < 100:
        raise HostBuildError(f"configured species data exposes only {len(pointers)} form tables")
    return (("#define gSpeciesInfo HostConstDeclaration_gSpeciesInfo\n" if writable else "")
            + '#include "global.h"\n#include "pokemon.h"\n'
            + ("#undef gSpeciesInfo\n" if writable else "") + ASSERTS
            + '#include "data/pokemon/form_species_tables.h"\n'
            + ("" if writable else "const ") + "struct SpeciesInfo gSpeciesInfo[NUM_SPECIES] = {\n"
            + "\n".join(pointers) + "\n};\n"
            "enum Species GetFormSpeciesId(enum Species species, u8 formId)\n{\n"
            "    const u16 *forms = species < NUM_SPECIES ? gSpeciesInfo[species].formSpeciesIdTable : NULL;\n"
            "    return forms != NULL ? forms[formId] : species;\n}\n")


def _undefined_symbols(output: str) -> list[str]:
    names = re.findall(r'^\s+"_?([A-Za-z_]\w*)", referenced from:', output, re.M)  # ld64
    names += re.findall(r"undefined reference to [`']([A-Za-z_]\w*)'", output)  # GNU ld / lld
    names += re.findall(r"undefined symbol: ([A-Za-z_]\w*)", output)  # lld
    return sorted(set(names))


def build(directory: Path, units: dict[str, str], *, name: str = "test", shared: bool = False,
          sanitize: bool = True, optimize: str = "-O1", defines: dict[str, str] | None = None,
          flags: tuple[str, ...] = (), inert: tuple[str, ...] = ()) -> Path:
    """Compile each unit separately with production flags; link dead-stripped.

    ``units`` maps a file name to C text.  A unit usually starts with
    ``production("src/foo.c")`` so the harness sees that file's static
    functions, followed by harness code and stubs for other units' symbols.

    ``inert`` lists regexes for symbols that live data merely points at (item
    icons, menu callbacks).  Missing symbols matching them are defined as
    trapping functions, so their address resolves but any use aborts loudly.
    Every other missing symbol fails the build by name.
    """
    directory = Path(directory)
    cc = compiler()
    # A None value removes a default define (e.g. RELEASE for fixture builds).
    config = {key: value for key, value in {**DEFINES, **(defines or {})}.items() if value is not None}
    shim = directory / "host_c_shim.h"
    shim.write_text(SHIM)
    common = [
        "-std=gnu17", optimize, "-g",
        "-iquote", str(ROOT / "include"), "-iquote", str(ROOT / "src"), "-iquote", str(ROOT),
        *(f"-D{key}={value}" for key, value in config.items()),
        "-include", str(shim),
        "-ffunction-sections", "-fdata-sections", "-fno-common",
        "-Werror=implicit-function-declaration", "-Werror=incompatible-pointer-types",
        "-Werror=int-conversion", "-Werror=return-type",
        *(SANITIZE if sanitize else ()), *flags,
    ]
    if shared:
        # Only HOST_EXPORT harness entry points are roots of the library.
        common.append("-fvisibility=hidden")
    objects, compiles = [], []
    for filename, text in units.items():
        source = directory / filename
        source.write_text(text + NDEBUG_GUARD)
        obj = source.with_suffix(".o")
        objects.append(str(obj))
        compiles.append((filename, subprocess.Popen(
            [cc, *common, "-fPIC", "-c", str(source), "-o", str(obj)],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)))
    failures = []
    for filename, process in compiles:
        output, _ = process.communicate(timeout=600)
        if process.returncode:
            failures.append(f"host compile of {filename} failed:\n{output}")
    if failures:
        raise HostBuildError("\n".join(failures))
    output = directory / (name + (".so" if shared else ""))
    strip = ["-Wl,-dead_strip"] if sys.platform == "darwin" else ["-Wl,--gc-sections"]
    if shared:
        link = [cc, "-shared", *strip, *(SANITIZE if sanitize else ()), *objects, "-o", str(output)]
        if sys.platform != "darwin":
            link.insert(2, "-Wl,--no-undefined")
    else:
        link = [cc, *strip, *(SANITIZE if sanitize else ()), *objects, "-o", str(output)]
    result = subprocess.run(link, capture_output=True, text=True, timeout=120)
    missing = _undefined_symbols(result.stdout + result.stderr) if result.returncode else []
    placeholders = [symbol for symbol in missing if any(re.fullmatch(p, symbol) for p in inert)]
    if placeholders:
        source = directory / "host_c_inert.c"
        source.write_text("".join(f"void {symbol}(void) {{ __builtin_trap(); }}\n" for symbol in placeholders))
        subprocess.run([cc, "-fPIC", "-w", "-c", str(source), "-o", str(source.with_suffix(".o"))],
                       check=True, capture_output=True, timeout=120)
        link.insert(link.index("-o"), str(source.with_suffix(".o")))
        result = subprocess.run(link, capture_output=True, text=True, timeout=120)
    if result.returncode:
        missing = _undefined_symbols(result.stdout + result.stderr)
        if missing:
            raise HostBuildError(
                "production code reachable from the host harness depends on symbol(s) "
                f"defined in another translation unit with no boundary stub: {', '.join(missing)}\n"
                + result.stderr)
        raise HostBuildError("host link failed:\n" + result.stdout + result.stderr)
    return output


def run(executable: Path, *args: str, timeout: int = 60) -> subprocess.CompletedProcess:
    """Run a harness; raise with its diagnostics unless it exits 0."""
    result = subprocess.run([str(executable), *map(str, args)], capture_output=True, text=True, timeout=timeout)
    if result.returncode:
        raise AssertionError(f"{Path(executable).name} {' '.join(map(str, args))} exited "
                             f"{result.returncode}:\n{result.stdout}{result.stderr}")
    return result
