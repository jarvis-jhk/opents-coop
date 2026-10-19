# Fork roadmap

This roadmap covers the `jarvis-jhk/opents-coop` fork. Upstream OpenTS keeps
its own plans; nothing here is an upstream commitment.

## Current priorities

The co-op items through the 0.11.0 release are done: shared houses and the
force matrix, co-op saves, shared waypoints and control groups, dropship
missions, the LAN playtest guide, and the Warpzone descriptor. Their records
are in the manual's change records and the fork releases.

## macOS support

Goal: a macOS build that a player can install and start a campaign with.

- Build for Apple Silicon and Intel, either as two archives or as one
  universal binary, on a GitHub `macos` runner in CI.
- Smoke-test the build on that runner without game data: the executable
  starts, opens its SDL window offscreen or headless, and exits cleanly.
- Attach the macOS archive to fork releases next to the Win32 and x64 ZIPs.
- Add macOS install and start steps to the player docs, including where the
  Tiberian Sun data goes. Unsigned builds are acceptable; document the
  Gatekeeper workaround (right-click Open, or
  `xattr -dr com.apple.quarantine <folder>`).

Known risks:

- The supported build is MSVC only, and `docs/BUILDING.md` defines what is
  supported. A Clang/AppleClang build needs CMake and source changes, and
  about a dozen sources still include `windows.h` or use `HWND`.
- macOS has no 32-bit target, so the build follows the x64 data widths.
  Saves, replays and network packets that differ between Win32 and x64 stay
  incompatible across those widths.
- Rendering goes through bgfx; macOS needs its Metal backend, which the
  Windows builds have not exercised.
- Audio uses miniaudio, which needs its CoreAudio backend enabled.
- File access needs `file_posix.cpp` on macOS, case handling for game data
  file names, and a data location under `~/Library/Application Support`
  instead of the game folder when the app bundle is read-only.
- Networking needs `netsocket_posix.cpp`; the macOS firewall may prompt on
  the first LAN game.
- Controllers are out of scope: the SDL joystick subsystem is not built, and
  the game has no controller input.
- The Warpzone launcher currently starts `Game.exe` and supports Windows only;
  online co-op on macOS needs a macOS launcher build or manual `SPAWN.INI`
  setup.
