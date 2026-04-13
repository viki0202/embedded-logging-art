#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import json
import argparse
import hashlib
import colorsys
from datetime import datetime

if sys.stdout.encoding is None or 'utf' not in sys.stdout.encoding.lower():
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
if sys.stdin.encoding is None or 'utf' not in sys.stdin.encoding.lower():
    sys.stdin.reconfigure(encoding='utf-8', errors='replace')

RESET = "\x1b[0m"

LEVEL_COLORS = {
    "DEBUG": "\x1b[38;2;120;120;120m",   # szary
    "INFO":  "\x1b[38;2;90;140;220m",    # spokojny niebieski
    "WARN":  "\x1b[38;2;200;160;40m",    # musztardowy
    "ERROR": "\x1b[38;2;220;70;70m",     # czerwony
}

NAMED_FG = {
    "black": "\x1b[30m", "red": "\x1b[31m", "green": "\x1b[32m",
    "yellow": "\x1b[33m", "blue": "\x1b[34m", "magenta": "\x1b[35m",
    "cyan": "\x1b[36m", "white": "\x1b[37m",
    "bright_black": "\x1b[90m", "bright_red": "\x1b[91m", "bright_green": "\x1b[92m",
    "bright_yellow": "\x1b[93m", "bright_blue": "\x1b[94m", "bright_magenta": "\x1b[95m",
    "bright_cyan": "\x1b[96m", "bright_white": "\x1b[97m",
    "brown": "\x1b[38;5;172m", "darkbrown": "\x1b[38;5;130m",
}

def rgb_fg(r, g, b):  
    return f"\x1b[38;2;{r};{g};{b}m"

def rgb_bg(r, g, b):  
    return f"\x1b[48;2;{r};{g};{b}m"

def stable_hue(key: str) -> int:
    """Wyznacz odcień H (0..359) deterministycznie na podstawie stringa."""
    h = hashlib.sha1(key.encode("utf-8")).hexdigest()
    return int(h[:2], 16) * 360 // 255  # 0..359

def hsl_to_rgb_int(h, s, l):
    """Konwersja HSL -> RGB (0..255). Używamy H∈[0..360], S/L∈[0..1]."""
    r, g, b = colorsys.hls_to_rgb(h/360.0, l, s)
    return int(r*255), int(g*255), int(b*255)

def pastel_rgb_for_key(key: str):
    """Pastel na podstawie klucza: stałe S/L, zmienne H."""
    h = stable_hue(key)
    r, g, b = hsl_to_rgb_int(h, 0.45, 0.78)  
    return r, g, b

def luminance(r, g, b):
    return 0.299*r + 0.587*g + 0.114*b

def best_fg_for_bg(r, g, b):
    return "\x1b[30m" if luminance(r, g, b) > 186 else "\x1b[97m"

def parse_args():
    p = argparse.ArgumentParser(
        description="Wyciąganie ważnych stringów z JSONL logów (bez generowania treści)."
    )
    p.add_argument("path", nargs="?", default="-",
                   help="Ścieżka do pliku JSONL lub '-' dla STDIN")
    p.add_argument("--fields", default="narrative",
                   help=("Lista pól do wypisania, rozdzielona przecinkami (np. 'ts,actor,narrative'). "
                         "Dostępne: ts,level,actor,action,corr_id,trace_id,narrative,details"))
    p.add_argument("--separator", default=" — ",
                   help="Separator między polami (domyślnie: ' — ').")
    p.add_argument("--exclude-heartbeat", action="store_true",
                   help="Wyklucz wpisy SYS/Heartbeat (szum).")
    p.add_argument("--level", action="append",
                   help="Filtruj po poziomie (można podać wiele razy), np. --level INFO --level ERROR")
    p.add_argument("--actor", action="append",
                   help="Filtruj po actor (można podać wiele razy), np. --actor CAN.Tx --actor CAN.Rx")
    p.add_argument("--corr", action="append", dest="corr_id",
                   help="Filtruj po corr_id (można podać wiele razy).")
    p.add_argument("--sort-ts", action="store_true", default=True,
                   help="Sortuj po ts (domyślnie włączone).")
    p.add_argument("--no-sort", action="store_true",
                   help="Nie sortuj po ts; zachowaj kolejność wejścia.")
    p.add_argument("--output-encoding", default="utf-8",
                   help="Kodowanie wyjścia (domyślnie utf-8).")
    p.add_argument("--trace", action="append", dest="trace_id",
                   help="Filtruj po trace_id (można podać wiele razy).")

    p.add_argument("--color", choices=["auto", "never", "always"], default="auto",
                   help="Kolorowanie: auto (TTY), never, always. Domyślnie: auto.")
    p.add_argument("--color-by", default="actor,trace_id,canid",
                   help=("Kolejność kluczy używanych do doboru pastelowego 'akcentu'. "
                         "Obsługiwane aliasy CAN-ID: canid|can_id|id."))
    p.add_argument("--keyword", action="append",
                   help='Podświetl słowa/wyrażenia, np. --keyword "timeout:yellow" (można wiele razy).')
    p.add_argument("--preset", choices=["sched", "timeline", "narrative"],
                   help="Gotowe presety wyświetlania logów.")
    return p.parse_args()

def load_entries(path):
    entries = []
    if path == "-" or path is None:
        fh = sys.stdin
    else:
        fh = open(path, "r", encoding="utf-8-sig", errors="replace")
    with fh:
        for line in fh:
            line = line.strip()
            if not line:
                continue
            try:
                obj = json.loads(line)
                entries.append(obj)
            except json.JSONDecodeError:
                continue
    return entries

def normalize_ts(ts):
    return ts

def select_field(entry, field):
    if field == "details":
        d = entry.get("details", {})
        try:
            return json.dumps(d, ensure_ascii=False, separators=(",", ":"))
        except Exception:
            return str(d)
    else:
        val = entry.get(field)
        return "" if val is None else str(val)

def parse_color_keywords(args):
    """Zbuduj mapę {keyword: ANSI} z --keyword 'słowo:kolor'."""
    if not args.keyword:
        return {}
    m = {}
    for item in args.keyword:
        if ":" in item:
            k, c = item.split(":", 1)
            k = k.strip()
            c = c.strip().lower()
            m[k] = NAMED_FG.get(c, NAMED_FG["magenta"])
        else:
            m[item.strip()] = NAMED_FG["magenta"]
    return m

def pick_accent_key(entry, color_by_list):
    """Wybierz pierwszy dostępny klucz do nadania 'akcentu' pastelowego."""
    CAN_ALIASES = ("canid", "can_id", "id")
    for key in color_by_list:
        if key.lower() in ("canid", "can_id"):  # obsłuż aliasy
            for real_key in CAN_ALIASES:
                if real_key in entry and entry.get(real_key) not in (None, ""):
                    return f"{real_key}:{entry.get(real_key)}"
        else:
            if key in entry and entry.get(key) not in (None, ""):
                return f"{key}:{entry.get(key)}"
    return None

def colorize_parts(entry, fields, separator, keyword_map, use_color, color_by_list):
    if not use_color:
        parts = []
        for f in fields:
            if f == "ts":
                parts.append(normalize_ts(entry.get("ts", "")))
            else:
                parts.append(select_field(entry, f))
        return separator.join(parts)

    accent_key = pick_accent_key(entry, color_by_list)
    accent_fg = ""
    if accent_key:
        r, g, b = pastel_rgb_for_key(accent_key)
        accent_fg = rgb_fg(r, g, b)

    parts = []
    for f in fields:
        raw = normalize_ts(entry.get("ts", "")) if f == "ts" else select_field(entry, f)
        seg = raw

        if f == "level" and raw:
            seg = f"{LEVEL_COLORS.get(raw, '')}{raw}{RESET}"

        elif f == "actor" and raw and accent_fg:
            seg = f"{accent_fg}{raw}{RESET}"

        elif f in ("trace_id", "corr_id") and raw:
            r, g, b = pastel_rgb_for_key(f"{f}:{raw}")
            fg = best_fg_for_bg(r, g, b)
            seg = f"{rgb_bg(r, g, b)}{fg}{raw}{RESET}"

        elif f.lower() in ("id", "can_id", "canid") and raw:
            r, g, b = pastel_rgb_for_key(f"{f}:{raw}")
            seg = f"{rgb_fg(r, g, b)}{raw}{RESET}"

        if seg and keyword_map:
            for kw, col in keyword_map.items():
                if kw and kw in seg:
                    seg = seg.replace(kw, f"{col}{kw}{RESET}")

        parts.append(seg)

    return separator.join(parts)

def sort_key_sched(e):
    ts = e.get("ts", "")
    try:
        dt = datetime.datetime.strptime(ts, "%Y-%m-%d %H:%M:%S.%f")
        return dt.timestamp()
    except Exception:
        return 0

def main():
    args = parse_args()

    try:
        sys.stdout.reconfigure(encoding=args.output_encoding)
    except Exception:
        pass

    if args.preset == "sched":
        args.actor = (args.actor or []) + ["SCHED"]
        args.fields = "ts,action,narrative,details"
        args.separator = "  \u2192  "  # "  →  "
        args.keyword = (args.keyword or []) + [
            "RunTask:green",
            "SkipTask:yellow",
            "Idle:bright_black",
            "QuantumEnd:cyan",
            "{ TickBegin:brown",
            "} TickEnd:darkbrown",
        ]

    is_tty = hasattr(sys.stdout, "isatty") and sys.stdout.isatty()
    use_color = (args.color == "always") or (args.color == "auto" and is_tty)

    separator = args.separator.replace('\\t', '\t').replace('\\n', '\n').replace('\\r', '\r')

    fields = [f.strip() for f in args.fields.split(",") if f.strip()]
    entries = load_entries(args.path)

    filtered = []
    for e in entries:
        if args.exclude_heartbeat and e.get("actor") == "SYS" and e.get("action") == "Heartbeat":
            continue
        if args.level and e.get("level") not in args.level:
            continue
        if args.actor and e.get("actor") not in args.actor:
            continue
        if args.corr_id and e.get("corr_id") not in args.corr_id:
            continue
        if args.trace_id and e.get("trace_id") not in args.trace_id:
            continue
        filtered.append(e)

    if not args.no_sort:
        if args.preset == "sched":
            filtered.sort(key=sort_key_sched)
        else:
            filtered.sort(key=lambda x: x.get("ts", ""))

    keyword_map = parse_color_keywords(args)
    color_by_list = [s.strip() for s in args.color_by.split(",") if s.strip()]

    for e in filtered:
        line = colorize_parts(e, fields, separator, keyword_map, use_color, color_by_list)
        print(line)


if __name__ == "__main__":
    main()