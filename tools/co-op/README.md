# Shared campaign crash notifications

Run the watcher beside a shared campaign to notify a private report endpoint when OpenTS writes a new crash report:

```sh
OPENTS_REPORT_URL='<private report endpoint>' python3 tools/co-op/watch-crashes.py /path/to/game --build <tested-commit>
```

On Windows, set `OPENTS_REPORT_URL` in the process environment and run the same Python command. Keep the endpoint out of public files. The watcher reads crash reports from the game directory’s `Exceptions/` folder.

Start the watcher before playing and stop it with Ctrl-C after the campaign. It continues watching across mission process restarts. Existing crash reports are ignored. Notifications contain the build identifier and operating-system family; reports, dumps, paths and player names remain on the device. A failed notification is printed once, without retrying or blocking the game.

[Client launch file](../../manual/content/formats/spawn-ini.md#a-shared-house-campaign) describes launch settings and gameplay limits.

Run the asset-free notification check with:

```sh
python3 tests/co-op/test_crash_watcher.py
```
