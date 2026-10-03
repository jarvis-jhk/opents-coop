# Create and play co-op campaign variants

This fork lets several players share a campaign house or control separate mission houses. One player can also test a multi-force variant alone. [Campaign mission variants](../manual/content/formats/campaign-variants.md) owns the filename, force declaration, matrix, and gameplay rules.

## Create a version

1. Copy a legally obtained mission into your local game-data folder, preserving its relative directory. Keep the original mission available.
2. Name the copy with a tag before its extension, such as `Maps/Missions/GDI1A.coop2.MAP`.
3. Set `[Basic] VariantName=Two bases` and keep `[Basic] Player=GDI` as the main force.
4. Add a second house to `[Houses]`, configure that house's credits and allies, and place its infantry, vehicles, and structures in your map editor. Give those objects the second house as owner.
5. Add `[PlayerForces]` with `1=GDI2`, using the second house's exact identifier. Include only houses people should control.
6. Review mission triggers and team ownership. Decide which bases must survive, which enemies must fall, and which force a reinforcement belongs to. Test success and failure separately.
7. Keep successor filenames pointing to their original missions. Make another tagged copy for every later mission you want to customize.

Do not commit or distribute original game assets or copied mission files with this repository. Share your authoring instructions or an independently permitted map package separately.

## Test alone

Install the fork engine beside your legally obtained game data and start a campaign from the menu. The version chooser lists the original and the loose variants. Choose the co-op version; the single-player row checks every force by default. Select an object from each force and verify its credits and queue. Press F9 to cycle forces if that key is free, or bind `NextForce` in the keyboard settings.

Verify that each house can build and place structures, that its units obey orders, that allies do not fight, and that the mission's win and loss triggers do what you intended. Test the next-mission transition and restart as well. A successful engine build does not establish these runtime results.

## Play with other machines

Use the same fork release, platform, mission files, campaign settings, and seed on every machine. Extract the release's executable, language library, and `ui/` together. Put `SPAWN.INI` in each machine's user directory and launch `Game.exe -SPAWN`. The [client launch-file guide](../manual/content/formats/spawn-ini.md) owns the accepted settings and connection rules.

The host's file can start with:

```ini
[Settings]
Name=Alice
Side=0
Color=0
IsSinglePlayer=yes
CampaignID=0
Scenario=Maps/Missions/GDI1A.MAP
DifficultyModeHuman=1
DifficultyModeComputer=1
GameSpeed=1
Seed=12345
Firestorm=yes
ConnTimeout=36000
Port=1234
Host=yes

[Other1]
Name=Bob
Side=0
Color=1
Ip=192.168.1.100
Port=1234
```

On Bob's machine, put Bob's name, side, and color in `[Settings]`, set `Host=no`, and put Alice in `[Other1]` with Alice's LAN address. Replace the example addresses with the actual addresses. Keep Alice's color lowest so she chooses the mission version and matrix. Allow the chosen UDP port through each machine's firewall.

Start every machine before choosing the version. Set the matrix and accept it; the other machines load that choice. A missing file or mismatched contents stops the launch. Use a writable launch file and keep the original roster connected through each next-mission or replay transition.

## Runtime limits

Shared campaigns disable saves, autosaves, shared resume, and waypoint edits. They refuse missions requesting dropship loadout selection. The stock campaign includes such missions, so this remains a partial campaign implementation. Full campaign completion and real LAN play require separate testing; consult [validation evidence](COOP_VALIDATION.md) for the earlier shared-house baseline.

The engine writes crash reports locally. `tools/co-op/watch-crashes.py` can watch a user directory and forward faults to a privately configured endpoint; keep endpoint credentials outside this public repository. Installing the engine alone does not enable remote reporting.

## Send feedback

Post bug reports and requests for this fork to <https://ntfy.sh/opents-coop-feedback-496c07>, through its web page, the ntfy app, or `curl -d "your message" https://ntfy.sh/opents-coop-feedback-496c07`. Anyone can read the channel, so leave out personal data. Name the release, the mission version, the number of players, and what happened. Accepted reports are fixed on the fork's `main` and shipped in a later fork release.
