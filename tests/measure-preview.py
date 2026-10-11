#!/usr/bin/env python3
"""Measure a bounded offscreen preview run using Linux process accounting."""

import argparse
import json
import os
import resource
import subprocess
import sys
import tempfile
import time
from pathlib import Path


def rss_kib(pid: int) -> int:
    try:
        for line in Path(f"/proc/{pid}/status").read_text().splitlines():
            if line.startswith("VmRSS:"):
                return int(line.split()[1])
    except FileNotFoundError:
        pass
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("binary")
    parser.add_argument("--duration", type=float, default=4.0)
    parser.add_argument("--max-rss-mib", type=float, default=256.0)
    parser.add_argument("--max-cpu-percent", type=float, default=50.0)
    args = parser.parse_args()
    if args.duration <= 0:
        parser.error("duration must be positive")

    with tempfile.TemporaryDirectory(prefix="prism-perf-") as config_dir:
        environment = os.environ.copy()
        environment.update(QT_QPA_PLATFORM="offscreen", XDG_CONFIG_HOME=config_dir)
        start = time.monotonic()
        process = subprocess.Popen(
            [args.binary], env=environment, stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        peak_rss = 0
        while time.monotonic() - start < args.duration and process.poll() is None:
            peak_rss = max(peak_rss, rss_kib(process.pid))
            time.sleep(0.2)
        died_early = process.poll() is not None
        if not died_early:
            process.terminate()
        try:
            _, stderr = process.communicate(timeout=3)
        except subprocess.TimeoutExpired:
            process.kill()
            _, stderr = process.communicate()
            died_early = True

    elapsed = time.monotonic() - start
    usage = resource.getrusage(resource.RUSAGE_CHILDREN)
    cpu_seconds = usage.ru_utime + usage.ru_stime
    cpu_percent = 100.0 * cpu_seconds / elapsed
    peak_rss = max(peak_rss, usage.ru_maxrss)
    result = {
        "elapsed_seconds": round(elapsed, 2),
        "cpu_seconds": round(cpu_seconds, 3),
        "cpu_percent_one_core": round(cpu_percent, 1),
        "peak_rss_kib": peak_rss,
        "exit_code": process.returncode,
        "stderr": stderr.decode(errors="replace")[:500],
    }
    print(json.dumps(result, sort_keys=True))
    if died_early:
        print("Preview exited before measurement completed", file=sys.stderr)
        return 1
    if peak_rss > args.max_rss_mib * 1024:
        print("Preview exceeded the RSS guardrail", file=sys.stderr)
        return 1
    if cpu_percent > args.max_cpu_percent:
        print("Preview exceeded the CPU guardrail", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
