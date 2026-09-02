#!/usr/bin/env python3
"""Inventory exact visual-source parity against the frozen Verdant checkpoint.

This is deliberately a byte/pixel ledger, not an aesthetic scorer.  Every
selected reference asset is classified exactly once, and every selected
working-tree asset is either its same-path counterpart, a deterministic
relocation match, or an addition.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import subprocess
import sys
import zlib
from collections import Counter, defaultdict, deque
from pathlib import Path, PurePosixPath


ROOT = Path(__file__).resolve().parents[1]
REFERENCE_COMMIT = "81e288b51995c59c1dbc640f77907b8120788bc9"
OUTPUT = ROOT / "work/audits/VERDANT_VISUAL_BYTE_MANIFEST.json"
POLICY_VERSION = 1

GRAPHIC_SUFFIXES = {".png", ".pal", ".gbapal", ".bin", ".tilemap", ".4bpp", ".8bpp", ".inc"}
VISUAL_DATA_PREFIXES = (
    "src/data/graphics/",
    "src/data/object_events/",
    "src/data/region_map/",
    "src/data/tilesets/",
)
VISUAL_DATA_PATHS = {
    "data/battle_anim_scripts.s",
    "data/graphics.s",
    "data/scripts/rival_graphics.inc",
    "src/data/battle_anim.h",
    "src/data/pokemon/species_info/shared_front_pic_anims.h",
}
GROUPS = ("ui", "map", "pokemon", "item", "trainer", "animation", "other")
CLASSIFICATIONS = (
    "same_path_identical",
    "relocated_byte_identical",
    "relocated_pixel_identical",
    "changed",
    "removed",
    "added",
)


def run_git(*args: str, text: bool = False) -> bytes | str:
    result = subprocess.run(
        ("git", *args),
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=text,
        check=False,
    )
    if result.returncode:
        stderr = result.stderr if text else result.stderr.decode("utf-8", "replace")
        raise SystemExit(f"git {' '.join(args)} failed:\n{stderr}")
    return result.stdout


def require_reference() -> None:
    result = subprocess.run(
        ("git", "cat-file", "-e", f"{REFERENCE_COMMIT}^{{commit}}"),
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.returncode:
        raise SystemExit(
            "required Verdant reference commit is missing: " + REFERENCE_COMMIT
        )


def selected_visual_path(path: str) -> bool:
    pure = PurePosixPath(path)
    suffix = pure.suffix.lower()
    if path.startswith("graphics/") and suffix in GRAPHIC_SUFFIXES:
        return True
    if path.startswith("data/tilesets/") and suffix in GRAPHIC_SUFFIXES | {".h"}:
        return True
    if path.startswith("data/layouts/") and suffix in {".bin", ".inc", ".json"}:
        return True
    if path.startswith("data/maps/") and pure.name == "map.json":
        return True
    if any(path.startswith(prefix) for prefix in VISUAL_DATA_PREFIXES):
        return suffix in {".h", ".json", ".txt"}
    return path in VISUAL_DATA_PATHS


def asset_kind(path: str) -> str:
    pure = PurePosixPath(path)
    suffix = pure.suffix.lower()
    if suffix == ".png":
        return "png"
    if suffix in {".pal", ".gbapal"}:
        return "palette"
    if path.startswith("data/layouts/") and suffix == ".bin":
        return "map_binary"
    if path.startswith("data/maps/") or path in {
        "data/layouts/layouts.json", "data/layouts/layouts.inc",
        "data/layouts/layouts_table.inc",
    }:
        return "map_data"
    if suffix in {".4bpp", ".8bpp", ".tilemap"}:
        return "tile_data"
    if suffix == ".bin":
        return "graphic_binary"
    return "visual_data"


def asset_group(path: str) -> str:
    lower = path.lower()
    if path.startswith(("data/layouts/", "data/maps/", "data/tilesets/", "src/data/tilesets/")):
        return "map"
    if "region_map" in lower or "/tilesets/" in lower or "/map_preview/" in lower:
        return "map"
    if any(token in lower for token in (
        "/battle_anims/", "battle_anim", "/battle_transitions/",
        "/field_effects/", "/battle_intro/",
    )):
        return "animation"
    if any(token in lower for token in (
        "/pokemon/", "/pokemon_icons/", "/pokemon_storage/",
        "src/data/graphics/pokemon.h", "shared_front_pic_anims",
    )):
        return "pokemon"
    if any(token in lower for token in (
        "/items/", "/berries/", "/pokeballs/", "graphics/items.h",
        "graphics/berries.h", "graphics/pokeballs.h",
    )):
        return "item"
    if any(token in lower for token in (
        "/trainers/", "/characters/", "object_event", "rival_graphics",
        "graphics/trainers.h",
    )):
        return "trainer"
    if any(token in lower for token in (
        "/interface/", "/menu/", "_menu/", "/summary_screen/",
        "/party_menu/", "/bag/", "/pokedex/", "/pokenav/", "/fonts/",
        "/window/", "/title_screen/", "/trainer_card/", "/shop/",
        "/naming_screen/", "/option_menu/", "/battle_interface/",
        "/pc_screen/", "/starter_choose/", "/move_relearner/",
    )):
        return "ui"
    return "other"


def reference_tree() -> dict[str, str]:
    raw = run_git("ls-tree", "-rz", "--full-tree", REFERENCE_COMMIT)
    assert isinstance(raw, bytes)
    result: dict[str, str] = {}
    for record in raw.split(b"\0"):
        if not record:
            continue
        metadata, encoded_path = record.split(b"\t", 1)
        mode, object_type, object_id = metadata.decode("ascii").split()
        path = encoded_path.decode("utf-8", "surrogateescape")
        if object_type == "blob" and selected_visual_path(path):
            result[path] = object_id
    return result


def git_blob_reader(object_ids: list[str]) -> dict[str, bytes]:
    process = subprocess.Popen(
        ("git", "cat-file", "--batch"),
        cwd=ROOT,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert process.stdin is not None and process.stdout is not None
    result: dict[str, bytes] = {}
    try:
        for object_id in sorted(set(object_ids)):
            process.stdin.write(object_id.encode("ascii") + b"\n")
            process.stdin.flush()
            header = process.stdout.readline().decode("ascii").strip().split()
            if len(header) != 3 or header[1] != "blob":
                raise SystemExit(f"cannot read reference visual blob {object_id}: {header}")
            size = int(header[2])
            payload = process.stdout.read(size)
            separator = process.stdout.read(1)
            if len(payload) != size or separator != b"\n":
                raise SystemExit(f"truncated reference visual blob {object_id}")
            result[object_id] = payload
    finally:
        process.stdin.close()
        process.wait()
    if process.returncode:
        stderr = process.stderr.read().decode("utf-8", "replace") if process.stderr else ""
        raise SystemExit(f"git cat-file --batch failed:\n{stderr}")
    return result


def working_tree_files() -> dict[str, bytes]:
    raw = run_git("ls-files", "-co", "--exclude-standard", "-z")
    assert isinstance(raw, bytes)
    result: dict[str, bytes] = {}
    for encoded in raw.split(b"\0"):
        if not encoded:
            continue
        path = encoded.decode("utf-8", "surrogateescape")
        if not selected_visual_path(path):
            continue
        absolute = ROOT / path
        if absolute.is_symlink():
            result[path] = os.readlink(absolute).encode("utf-8", "surrogateescape")
        elif absolute.is_file():
            result[path] = absolute.read_bytes()
    return result


def fast_working_tree_hashes() -> dict[str, str]:
    """Hash the selected current files without decoding PNG pixels.

    The full writer intentionally performs the more expensive normalized-pixel
    comparison against the frozen Verdant reference.  Release verification only
    needs to prove that the already-reviewed manifest still describes every
    selected current file byte-for-byte, so it can take this much cheaper path.
    """
    return {
        path: hashlib.sha256(payload).hexdigest()
        for path, payload in working_tree_files().items()
    }


def verify_current_manifest_fast(manifest: dict) -> None:
    expected: dict[str, str] = {}
    for row in manifest["reference_assets"]:
        path = row.get("current_path")
        if path is not None:
            expected[path] = row["current_sha256"]
    for row in manifest["added_assets"]:
        expected[row["current_path"]] = row["current_sha256"]

    actual = fast_working_tree_hashes()
    missing = sorted(set(expected) - set(actual))
    added = sorted(set(actual) - set(expected))
    changed = sorted(
        path for path in set(expected) & set(actual)
        if expected[path] != actual[path]
    )
    if missing or added or changed:
        preview = []
        preview.extend(f"missing: {path}" for path in missing[:20])
        preview.extend(f"added: {path}" for path in added[:20])
        preview.extend(f"changed: {path}" for path in changed[:20])
        raise SystemExit(
            "visual parity manifest is stale; run "
            "python3 scripts/audit_verdant_visual_parity.py --write\n"
            + "\n".join(preview)
        )

    # Mutation proof: a one-byte-equivalent digest change must be observable by
    # the same comparison used above.  This guards the release check itself from
    # accidentally degenerating into a path-only inventory.
    if expected:
        sample_path = min(expected)
        mutated = dict(actual)
        mutated[sample_path] = "0" * 64
        if expected[sample_path] == mutated[sample_path]:
            raise SystemExit("visual manifest mutation probe did not alter the sample digest")
        if not any(expected[path] != mutated[path] for path in expected):
            raise SystemExit("visual manifest mutation probe escaped digest comparison")

    print(
        "verdant_visual_manifest_fast=PASS "
        f"current={len(actual)} mutation_probes=1"
    )


def paeth(left: int, above: int, upper_left: int) -> int:
    prediction = left + above - upper_left
    left_distance = abs(prediction - left)
    above_distance = abs(prediction - above)
    upper_left_distance = abs(prediction - upper_left)
    if left_distance <= above_distance and left_distance <= upper_left_distance:
        return left
    if above_distance <= upper_left_distance:
        return above
    return upper_left


def unpack_samples(row: bytes, bit_depth: int, count: int) -> list[int]:
    if bit_depth == 8:
        return list(row[:count])
    if bit_depth == 16:
        return [struct.unpack_from(">H", row, offset)[0] for offset in range(0, count * 2, 2)]
    mask = (1 << bit_depth) - 1
    samples: list[int] = []
    for byte in row:
        for shift in range(8 - bit_depth, -1, -bit_depth):
            samples.append((byte >> shift) & mask)
            if len(samples) == count:
                return samples
    if len(samples) != count:
        raise ValueError("short packed PNG scanline")
    return samples


def png_pixel_digest(data: bytes) -> str:
    if not data.startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError("invalid PNG signature")
    offset = 8
    width = height = bit_depth = color_type = interlace = None
    palette = b""
    transparency = b""
    compressed = bytearray()
    while offset < len(data):
        if offset + 12 > len(data):
            raise ValueError("truncated PNG chunk")
        length = struct.unpack_from(">I", data, offset)[0]
        chunk_type = data[offset + 4:offset + 8]
        payload = data[offset + 8:offset + 8 + length]
        if len(payload) != length:
            raise ValueError("truncated PNG payload")
        if chunk_type == b"IHDR":
            width, height, bit_depth, color_type, _, _, interlace = struct.unpack(">IIBBBBB", payload)
        elif chunk_type == b"PLTE":
            palette = payload
        elif chunk_type == b"tRNS":
            transparency = payload
        elif chunk_type == b"IDAT":
            compressed.extend(payload)
        elif chunk_type == b"IEND":
            break
        offset += 12 + length
    if None in (width, height, bit_depth, color_type, interlace):
        raise ValueError("PNG has no IHDR")
    if interlace != 0:
        raise ValueError("interlaced PNG is unsupported by the deterministic decoder")
    channels = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}.get(color_type)
    if channels is None or bit_depth not in {1, 2, 4, 8, 16}:
        raise ValueError(f"unsupported PNG format color={color_type} depth={bit_depth}")
    if color_type in {2, 4, 6} and bit_depth not in {8, 16}:
        raise ValueError(f"invalid PNG channel depth color={color_type} depth={bit_depth}")
    stride = (width * channels * bit_depth + 7) // 8
    bytes_per_pixel = max(1, (channels * bit_depth + 7) // 8)
    filtered = zlib.decompress(bytes(compressed))
    expected = height * (stride + 1)
    if len(filtered) != expected:
        raise ValueError(f"unexpected PNG scanline bytes {len(filtered)} != {expected}")
    rows: list[bytes] = []
    cursor = 0
    previous = bytearray(stride)
    for _ in range(height):
        filter_type = filtered[cursor]
        source = filtered[cursor + 1:cursor + 1 + stride]
        cursor += stride + 1
        row = bytearray(stride)
        for index, value in enumerate(source):
            left = row[index - bytes_per_pixel] if index >= bytes_per_pixel else 0
            above = previous[index]
            upper_left = previous[index - bytes_per_pixel] if index >= bytes_per_pixel else 0
            if filter_type == 0:
                predictor = 0
            elif filter_type == 1:
                predictor = left
            elif filter_type == 2:
                predictor = above
            elif filter_type == 3:
                predictor = (left + above) // 2
            elif filter_type == 4:
                predictor = paeth(left, above, upper_left)
            else:
                raise ValueError(f"invalid PNG filter {filter_type}")
            row[index] = (value + predictor) & 0xFF
        rows.append(bytes(row))
        previous = row

    maximum = (1 << bit_depth) - 1
    transparent_gray = struct.unpack(">H", transparency)[0] if color_type == 0 and len(transparency) >= 2 else None
    transparent_rgb = struct.unpack(">HHH", transparency[:6]) if color_type == 2 and len(transparency) >= 6 else None
    canonical = bytearray(struct.pack(">II", width, height))

    def scaled(value: int) -> int:
        return (value * 65535) // maximum

    for row in rows:
        samples = unpack_samples(row, bit_depth, width * channels)
        for pixel in range(width):
            start = pixel * channels
            if color_type == 0:
                gray_raw = samples[start]
                gray = scaled(gray_raw)
                alpha = 0 if transparent_gray == gray_raw else 65535
                rgba = (gray, gray, gray, alpha)
            elif color_type == 2:
                raw_rgb = tuple(samples[start:start + 3])
                rgba = tuple(scaled(value) for value in raw_rgb) + (
                    0 if transparent_rgb == raw_rgb else 65535,
                )
            elif color_type == 3:
                palette_index = samples[start]
                palette_offset = palette_index * 3
                if palette_offset + 3 > len(palette):
                    raise ValueError("PNG palette index is out of range")
                red, green, blue = palette[palette_offset:palette_offset + 3]
                alpha = transparency[palette_index] if palette_index < len(transparency) else 255
                rgba = (red * 257, green * 257, blue * 257, alpha * 257)
            elif color_type == 4:
                gray, alpha = samples[start:start + 2]
                rgba = (scaled(gray), scaled(gray), scaled(gray), scaled(alpha))
            else:
                red, green, blue, alpha = samples[start:start + 4]
                rgba = tuple(scaled(value) for value in (red, green, blue, alpha))
            canonical.extend(struct.pack(">HHHH", *rgba))
    return hashlib.sha256(canonical).hexdigest()


def describe(path: str, payload: bytes) -> dict:
    result = {
        "path": path,
        "group": asset_group(path),
        "kind": asset_kind(path),
        "size": len(payload),
        "sha256": hashlib.sha256(payload).hexdigest(),
    }
    if result["kind"] == "png":
        result["pixel_sha256"] = png_pixel_digest(payload)
    return result


def tree_digest(records: dict[str, dict]) -> str:
    digest = hashlib.sha256()
    for path in sorted(records):
        digest.update(path.encode("utf-8", "surrogateescape"))
        digest.update(b"\0")
        digest.update(records[path]["sha256"].encode("ascii"))
        digest.update(b"\n")
    return digest.hexdigest()


def classify(reference: dict[str, dict], current: dict[str, dict]) -> tuple[list[dict], list[dict]]:
    reference_rows: list[dict] = []
    consumed_current: set[str] = set()
    missing_reference: list[str] = []

    for path in sorted(reference):
        old = reference[path]
        if path not in current:
            missing_reference.append(path)
            continue
        new = current[path]
        consumed_current.add(path)
        classification = "same_path_identical" if old["sha256"] == new["sha256"] else "changed"
        row = {
            "classification": classification,
            "group": old["group"],
            "kind": old["kind"],
            "reference_path": path,
            "current_path": path,
            "reference_size": old["size"],
            "current_size": new["size"],
            "reference_sha256": old["sha256"],
            "current_sha256": new["sha256"],
        }
        if old.get("pixel_sha256") is not None:
            row["reference_pixel_sha256"] = old["pixel_sha256"]
            row["current_pixel_sha256"] = new.get("pixel_sha256")
            row["pixel_identical"] = old["pixel_sha256"] == new.get("pixel_sha256")
        reference_rows.append(row)

    available = sorted(set(current) - consumed_current)
    by_bytes: dict[str, deque[str]] = defaultdict(deque)
    by_pixels: dict[str, deque[str]] = defaultdict(deque)
    for path in available:
        by_bytes[current[path]["sha256"]].append(path)
        pixel_hash = current[path].get("pixel_sha256")
        if pixel_hash is not None:
            by_pixels[pixel_hash].append(path)

    for path in missing_reference:
        old = reference[path]
        matched_path: str | None = None
        classification = "removed"
        byte_bucket = by_bytes[old["sha256"]]
        while byte_bucket and byte_bucket[0] in consumed_current:
            byte_bucket.popleft()
        if byte_bucket:
            matched_path = byte_bucket.popleft()
            classification = "relocated_byte_identical"
        elif old.get("pixel_sha256") is not None:
            pixel_bucket = by_pixels[old["pixel_sha256"]]
            while pixel_bucket and pixel_bucket[0] in consumed_current:
                pixel_bucket.popleft()
            if pixel_bucket:
                matched_path = pixel_bucket.popleft()
                classification = "relocated_pixel_identical"
        row = {
            "classification": classification,
            "group": old["group"],
            "kind": old["kind"],
            "reference_path": path,
            "current_path": matched_path,
            "reference_size": old["size"],
            "current_size": current[matched_path]["size"] if matched_path else None,
            "reference_sha256": old["sha256"],
            "current_sha256": current[matched_path]["sha256"] if matched_path else None,
        }
        if old.get("pixel_sha256") is not None:
            row["reference_pixel_sha256"] = old["pixel_sha256"]
            row["current_pixel_sha256"] = current[matched_path].get("pixel_sha256") if matched_path else None
            row["pixel_identical"] = bool(
                matched_path and old["pixel_sha256"] == current[matched_path].get("pixel_sha256")
            )
        reference_rows.append(row)
        if matched_path:
            consumed_current.add(matched_path)

    reference_rows.sort(key=lambda row: row["reference_path"])
    added_rows = [
        {
            "classification": "added",
            "group": current[path]["group"],
            "kind": current[path]["kind"],
            "reference_path": None,
            "current_path": path,
            "reference_size": None,
            "current_size": current[path]["size"],
            "reference_sha256": None,
            "current_sha256": current[path]["sha256"],
            **(
                {"current_pixel_sha256": current[path]["pixel_sha256"]}
                if current[path].get("pixel_sha256") is not None else {}
            ),
        }
        for path in sorted(set(current) - consumed_current)
    ]
    assert len(reference_rows) == len(reference)
    assert len({row["reference_path"] for row in reference_rows}) == len(reference)
    assert {row["reference_path"] for row in reference_rows} == set(reference)
    matched_current = {row["current_path"] for row in reference_rows if row["current_path"]}
    added_current = {row["current_path"] for row in added_rows}
    assert not matched_current.intersection(added_current)
    assert matched_current | added_current == set(current)
    assert len(matched_current) + len(added_current) == len(current)
    return reference_rows, added_rows


def build_manifest() -> dict:
    require_reference()
    reference_objects = reference_tree()
    blobs = git_blob_reader(list(reference_objects.values()))
    reference = {
        path: describe(path, blobs[object_id])
        for path, object_id in sorted(reference_objects.items())
    }
    current_payloads = working_tree_files()
    current = {
        path: describe(path, payload)
        for path, payload in sorted(current_payloads.items())
    }
    reference_rows, added_rows = classify(reference, current)
    all_rows = reference_rows + added_rows
    counts = Counter(row["classification"] for row in all_rows)
    by_group: dict[str, dict[str, int]] = {}
    for group in GROUPS:
        group_counts = Counter(
            row["classification"] for row in all_rows if row["group"] == group
        )
        by_group[group] = {name: group_counts[name] for name in CLASSIFICATIONS}
        by_group[group]["reference_total"] = sum(
            row["group"] == group for row in reference_rows
        )
        by_group[group]["current_total"] = sum(
            row["group"] == group and row["current_path"] is not None
            for row in reference_rows
        ) + sum(row["group"] == group for row in added_rows)
    return {
        "schema_version": 1,
        "policy_version": POLICY_VERSION,
        "reference_commit": REFERENCE_COMMIT,
        "scope": {
            "asset_roots": ["graphics/", "data/tilesets/", "data/layouts/", "data/maps/*/map.json"],
            "visual_data_prefixes": list(VISUAL_DATA_PREFIXES),
            "visual_data_paths": sorted(VISUAL_DATA_PATHS),
            "graphic_suffixes": sorted(GRAPHIC_SUFFIXES),
            "semantics": "byte and normalized PNG-pixel identity only; no subjective quality decision",
        },
        "reference_visual_tree_sha256": tree_digest(reference),
        "current_visual_tree_sha256": tree_digest(current),
        "summary": {
            "reference_total": len(reference),
            "current_total": len(current),
            **{name: counts[name] for name in CLASSIFICATIONS},
            "reference_coverage_exactly_once": True,
            "current_coverage_exactly_once": True,
        },
        "by_group": by_group,
        "reference_assets": reference_rows,
        "added_assets": added_rows,
    }


def render(manifest: dict) -> str:
    return json.dumps(manifest, indent=2, sort_keys=True, ensure_ascii=False) + "\n"


def print_summary(manifest: dict) -> None:
    summary = manifest["summary"]
    print(
        "verdant_visual_manifest=PASS "
        f"reference={summary['reference_total']} current={summary['current_total']}"
    )
    for name in CLASSIFICATIONS:
        print(f"{name}={summary[name]}")
    for group in GROUPS:
        values = manifest["by_group"][group]
        print(
            f"group_{group}=reference:{values['reference_total']},"
            f"current:{values['current_total']},changed:{values['changed']},"
            f"removed:{values['removed']},added:{values['added']}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true", help="write the canonical manifest")
    parser.add_argument(
        "--check-fast",
        action="store_true",
        help="verify current files against the reviewed manifest without decoding PNG pixels",
    )
    args = parser.parse_args()
    if args.check_fast:
        if not OUTPUT.exists():
            raise SystemExit(f"visual parity manifest is missing: {OUTPUT.relative_to(ROOT)}")
        verify_current_manifest_fast(json.loads(OUTPUT.read_text()))
        return
    manifest = build_manifest()
    payload = render(manifest)
    if args.write:
        OUTPUT.parent.mkdir(parents=True, exist_ok=True)
        OUTPUT.write_text(payload)
    else:
        if not OUTPUT.exists():
            raise SystemExit(f"visual parity manifest is missing: {OUTPUT.relative_to(ROOT)}")
        if OUTPUT.read_text() != payload:
            raise SystemExit(
                "visual parity manifest is stale; run "
                "python3 scripts/audit_verdant_visual_parity.py --write"
            )
    print_summary(manifest)


if __name__ == "__main__":
    main()
