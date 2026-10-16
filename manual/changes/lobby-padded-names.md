---
title: Ignore spaces around lobby names in shared campaigns
category: fix
release: 0.10.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- jarvis-jhk
---

A shared campaign started from the network lobby now ignores spaces and tabs before and after a player's name. A name such as ` Bob ` used to put the padded name into the launch file, where the game could not find the player's own seat and returned to the main menu. Two names that differ only in those spaces count as the same name, so the host cannot start the campaign until one player renames.
