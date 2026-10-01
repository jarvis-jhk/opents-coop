#!/usr/bin/env python3
"""Watch OpenTS crash artifacts without uploading their contents."""

import argparse
import json
import os
from pathlib import Path
import threading
import time
from urllib.request import Request, urlopen


def artifacts(directory):
    return set(directory.glob("Exceptions/exception-*/except.txt"))


def report(endpoint, build):
    payload = json.dumps({
        "lane": "opents",
        "kind": "bug",
        "title": "OpenTS shared campaign crashed",
        "detail": "A new crash report was written. The report and dumps remain on the device.",
        "source": f"OpenTS {build}, {os.name}",
    }).encode("utf-8")
    try:
        request = Request(endpoint, data=payload, headers={"Content-Type": "application/json"})
        with urlopen(request, timeout=5) as response:
            response.read(4096)
    except Exception as error:
        print(f"Crash notification failed ({type(error).__name__}); local artifacts remain.", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path, help="Directory containing Game.exe")
    parser.add_argument("--build", required=True, help="The tested commit identifier")
    args = parser.parse_args()
    endpoint = os.environ.get("OPENTS_REPORT_URL")
    if not endpoint:
        parser.error("Set OPENTS_REPORT_URL to the private report endpoint.")
    if not args.directory.is_dir():
        parser.error("The game directory does not exist.")
    known = artifacts(args.directory)
    print("Watching new crash reports. Press Ctrl-C after finishing the campaign.", flush=True)
    try:
        while True:
            current = artifacts(args.directory)
            for _ in current - known:
                threading.Thread(target=report, args=(endpoint, args.build), daemon=True).start()
            known |= current
            time.sleep(1)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
