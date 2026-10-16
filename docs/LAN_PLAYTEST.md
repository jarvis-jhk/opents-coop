# Two-machine LAN playtest

This checklist tests a shared campaign on two machines in one local network, from the lobby to the second mission. Each step names what both players should see and which files to keep when they see something else. [Create and play co-op campaign variants](COOP_CAMPAIGNS.md) explains the lobby, saves and limits in more detail.

## Prepare both machines

- Install the same fork release and the same platform, Win32 or x64, on both machines, beside a legally obtained Tiberian Sun installation. A Win32 and an x64 machine in one game can go out of sync.
- Put the same mission files on both machines. A co-op variant such as `GDI1A.coop2.MAP` must be present on each machine.
- Allow UDP port 1234 through both firewalls.
- Give the two players different names. Spaces before and after a name do not count.

The game writes a debug log for every run to `Debug/DEBUG_<date>_<time>.LOG` beside `Game.exe`. Each next-mission restart is a new run with its own log. When a step fails, keep the logs from both machines and note the step number.

## Steps

1. **Lobby.** Both players open **Multiplayer Game**, then **Network**. Player A clicks **New**; player B selects A's game and clicks **Join**. Expected: both see both names in the game setup.
2. **Campaign start.** A clicks **Campaign**, chooses GDI, the first mission and a difficulty, and clicks **OK**. B clicks **Accept**, and A clicks **Go!**. Expected: A sees the version chooser and control matrix; B sees "Waiting for A to choose". After A accepts, both load the same mission. If a machine returns to the main menu instead, keep `SPAWN.INI` from each machine's user directory together with the logs.
3. **Shared play, at least ten minutes.** Both players give orders to units and build from the sidebar. Expected: both see the same credits, the same production queue and the same units in the same places, and no out-of-sync dialog appears. An out-of-sync game writes `Debug/SYNC_*.LOG` on each machine; keep those files.
4. **Control groups and waypoints.** A selects units and presses Ctrl+1; B presses 1. Expected: B selects the same units, and both screens show the number 1 on them. A plots a waypoint path and sends a unit along it. Expected: the unit follows it on both screens; B does not see A's path.
5. **Save and in-game load.** B saves from the options menu with **Save Game**. Expected: both machines write a save with the same number. A loads that save from the options menu. Expected: both machines continue from the saved moment.
6. **Resume after quitting.** Both players quit to the desktop and meet in the network lobby again. A clicks **Load Game** instead of **Campaign** and picks the save, B accepts, and A clicks **Go!**. Expected: both continue the saved mission.
7. **Next mission.** Win the mission. Expected: both games restart into the next mission; A chooses the map and version, and B waits for that choice. If either machine stays on the desktop or waits forever, keep the newest debug log from each machine.

A crash writes a folder under `Exceptions/` beside `Game.exe`. Keep the whole folder. Its `debug-tail.log` can contain player names and network addresses, so read it before posting it anywhere public.

## Report the result

Post the release, the mission version, each step's result and the step where something went wrong to the [feedback channel](COOP_CAMPAIGNS.md#send-feedback). Anyone can read that channel, so describe the problem there and keep the files until someone asks for them.
