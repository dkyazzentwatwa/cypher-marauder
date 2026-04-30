#!/usr/bin/env python3
"""Generate embedded HID payload tables from .ino payload scripts."""

from __future__ import annotations

import argparse
import ast
import re
from pathlib import Path

DEFAULT_SOURCE = Path(__file__).resolve().parents[1] / "payloads"
DEFAULT_OUTPUT = Path(__file__).resolve().parents[1] / "src" / "HidPayloadsGenerated.h"
# No default allowlist - we'll scan all .ino files except those in creator-pack
PANIC_RESET = "echo 'PANIC RESET: pkill -f reel_ || true; killall Terminal >/dev/null 2>&1 || true'"


def parse_cmd_arg(arg: str) -> str:
    arg = arg.strip()
    if arg.startswith('R"(') and arg.endswith(')"'):
        return arg[3:-2]
    if arg.startswith('"') and arg.endswith('"'):
        return ast.literal_eval(arg)
    raise ValueError(f"Unsupported typeCmd argument: {arg}")


def parse_script(path: Path, source_root: Path) -> dict | None:
    """Parse a script file and return payload data, or None if not valid."""
    text = path.read_text(encoding="utf-8")
    
    # Look for Title in various formats (commented or not)
    title_match = re.search(r"(?://|/\*|\s)Title:\s*(.+)", text)
    if not title_match:
        # Also look for Title at start of line
        title_match = re.search(r"^\s*Title:\s*(.+)", text, re.MULTILINE)
    
    # Look for RuntimeSec in various formats (commented or not)
    runtime_match = re.search(r"(?://|/\*|\s)RuntimeSec:\s*(\d+)", text)
    if not runtime_match:
        # Also look for RuntimeSec at start of line
        runtime_match = re.search(r"^\s*RuntimeSec:\s*(\d+)", text, re.MULTILINE)
    
    if not title_match:
        # Skip files without Title metadata
        return None
    
    platform_match = re.search(r"(?://|/\*|\s)Platform:\s*([a-zA-Z_]+)", text)
    if not platform_match:
        platform_match = re.search(r"^\s*Platform:\s*([a-zA-Z_]+)", text, re.MULTILINE)

    focused_match = re.search(r"(?://|/\*|\s)RequiresFocusedTextField:\s*(true|false|yes|no|1|0)", text, re.IGNORECASE)
    if not focused_match:
        focused_match = re.search(r"^\s*RequiresFocusedTextField:\s*(true|false|yes|no|1|0)", text, re.IGNORECASE | re.MULTILINE)

    # Use default runtime of 30 seconds if not specified
    runtime_sec = int(runtime_match.group(1)) if runtime_match else 30
    platform = platform_match.group(1).strip().lower() if platform_match else "macos"
    requires_focused = False
    if focused_match:
        requires_focused = focused_match.group(1).strip().lower() in {"true", "yes", "1"}

    commands = []
    for line in text.splitlines():
        stripped = line.strip()
        # Look for typeCmd() calls
        m = re.search(r"typeCmd\((.+)\);\s*$", stripped)
        if m:
            commands.append(parse_cmd_arg(m.group(1)))
        # Look for Keyboard.print() calls (but not inside function definitions or other contexts)
        elif stripped.startswith("Keyboard.print(") and stripped.endswith(");"):
            # Extract the argument to Keyboard.print
            match = re.search(r'Keyboard\.print\((.+)\);', stripped)
            if match:
                arg = match.group(1)
                # Handle both quoted and raw string cases
                if arg.startswith('"') and arg.endswith('"') and len(arg) >= 2:
                    # Quoted string - remove quotes
                    arg = arg[1:-1]
                elif arg.startswith('R"(') and arg.endswith(')"'):
                    # Raw string literal
                    arg = arg[3:-2]
                commands.append(arg)
        if "panicResetHint();" in line:
            commands.append(PANIC_RESET)

    if not commands:
        # Skip files without typeCmd or Keyboard.print commands
        return None

    rel_path = path.resolve().relative_to(source_root.resolve())
    category = rel_path.parts[0] if len(rel_path.parts) > 1 else "misc"

    return {
        "id": path.stem,
        "title": title_match.group(1).strip(),
        "runtime_sec": runtime_sec,
        "category": category,
        "platform": platform,
        "requires_focused_text_field": requires_focused,
        "commands": commands,
    }


def cpp_escape(value: str) -> str:
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def collect_ino_files(source: Path) -> list[Path]:
    """Recursively collect all .ino files under source."""
    source = source.resolve()
    if not source.exists():
        raise ValueError(f"Source directory not found: {source}")

    files: list[Path] = []
    # Recursively walk through all directories
    for path in source.rglob("*.ino"):
        files.append(path.resolve())
    
    return sorted(files)  # Sort for consistent ordering


def render(payloads: list[dict]) -> str:
    out: list[str] = []
    out.append("#pragma once")
    out.append("")
    out.append("#include <Arduino.h>")
    out.append("")
    out.append("struct HidPayloadDef {")
    out.append("  const char *id;")
    out.append("  const char *title;")
    out.append("  uint16_t runtimeSec;")
    out.append("  const char *category;")
    out.append("  const char *platform;")
    out.append("  bool requiresFocusedTextField;")
    out.append("  const char *const *commands;")
    out.append("  uint8_t commandCount;")
    out.append("};")
    out.append("")
    out.append("// Generated by tools/generate_hid_payloads.py.")
    out.append("// Source includes all .ino files under payloads/.")
    out.append("")

    for payload in payloads:
        symbol = payload["id"].replace("-", "_")
        out.append(f"static const char *const kHidCmds_{symbol}[] = {{")
        for cmd in payload["commands"]:
            out.append(f"    {cpp_escape(cmd)},")
        out.append("};")
        out.append("")

    out.append("static const HidPayloadDef kHidPayloads[] = {")
    for payload in payloads:
        symbol = payload["id"].replace("-", "_")
        out.append(
            f'    {{"{payload["id"]}", "{payload["title"]}", {payload["runtime_sec"]}, '
            f'"{payload["category"]}", '
            f'"{payload["platform"]}", '
            f'{"true" if payload["requires_focused_text_field"] else "false"}, '
            f'kHidCmds_{symbol}, static_cast<uint8_t>(sizeof(kHidCmds_{symbol}) / '
            f"sizeof(kHidCmds_{symbol}[0]))}},"
        )
    out.append("};")
    out.append("")
    out.append("constexpr size_t kHidPayloadCount = sizeof(kHidPayloads) / sizeof(kHidPayloads[0]);")
    out.append("")
    return "\n".join(out)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    # --allowlist argument is no longer used but kept for backward compatibility
    parser.add_argument(
        "--allowlist",
        nargs="+",
        default=[],
        help="Argument kept for backward compatibility but ignored.",
    )
    args = parser.parse_args()

    files = collect_ino_files(args.source)
    payloads = []
    for path in files:
        payload_data = parse_script(path, args.source)
        if payload_data is not None:
            payloads.append(payload_data)
        else:
            print(f"Skipping {path} (missing required metadata or commands)")
    
    rendered = render(payloads)
    args.output.write_text(rendered, encoding="utf-8")
    print(f"Generated {args.output} with {len(payloads)} payloads.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
