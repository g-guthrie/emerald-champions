"""Verify the existing Makefile's ELF -> binary -> FF-padded ROM relationship."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
MAX_ROM_BYTES = 32 * 1024 * 1024


def verify_rom_bytes(rom: bytes, elf_binary: bytes) -> None:
    if not 0 < len(elf_binary) <= MAX_ROM_BYTES:
        raise ValueError("ELF-derived binary has invalid GBA ROM size")
    padded_size = 1 << (len(elf_binary) - 1).bit_length()
    if len(rom) != padded_size:
        raise ValueError(f"ROM size {len(rom)} does not match ELF's padded size {padded_size}")
    if rom[:len(elf_binary)] != elf_binary:
        raise ValueError("ROM bytes do not match the supplied ELF")
    padding = rom[len(elf_binary):]
    if padding.count(0xFF) != len(padding):
        raise ValueError("ROM padding differs from the Makefile's gbafix FF padding")


def verify_rom_elf_pair(rom: Path, elf: Path, *, objcopy: str | None = None) -> None:
    """Check a built pair without changing either file or requiring new metadata.

    The Makefile fixes the ELF header before objcopy and then pads the ROM.
    This verifies that relationship, not source freshness or toolchain origin.
    """
    if not rom.is_file() or not elf.is_file():
        raise ValueError(f"ROM/ELF pair is missing: {rom}, {elf}")
    if not 0 < rom.stat().st_size <= MAX_ROM_BYTES:
        raise ValueError("ROM has invalid GBA size")
    if objcopy is None:
        objcopy = shutil.which("arm-none-eabi-objcopy")
        if objcopy is None:
            for relative in ("tools/binutils/bin/arm-none-eabi-objcopy", "tools/agbcc/bin/arm-none-eabi-objcopy"):
                candidate = ROOT / relative
                if candidate.is_file():
                    objcopy = str(candidate)
                    break
    if objcopy is None:
        raise ValueError("arm-none-eabi-objcopy is required to verify ROM/ELF correspondence")
    with tempfile.TemporaryDirectory(prefix="ec-rom-elf-") as directory:
        output = Path(directory) / "derived.bin"
        result = subprocess.run([objcopy, "-O", "binary", str(elf), str(output)],
                                text=True, capture_output=True, timeout=30)
        if result.returncode != 0:
            raise ValueError(f"cannot derive ROM from ELF: {result.stderr.strip()}")
        if not output.is_file() or not 0 < output.stat().st_size <= MAX_ROM_BYTES:
            raise ValueError("objcopy did not produce a valid-sized GBA binary")
        verify_rom_bytes(rom.read_bytes(), output.read_bytes())
