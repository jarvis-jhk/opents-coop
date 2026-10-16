---
title: Read the game data a launcher found
category: feature
release: 0.9.0
targets:
- type: command
  id: launch:data-directory
  effect: changed
credit:
- jarvis-jhk
---

Without `-DATADIR`, the game now reads its data from the folder in the `WARPZONE_DATA_TIBSUN` environment variable, which the Warpzone launcher sets to the Tiberian Sun installation it found. A player who installs the game through the launcher no longer copies the data or names its folder. A folder in the variable that does not exist is ignored, and the game starts with the data beside the executable.
