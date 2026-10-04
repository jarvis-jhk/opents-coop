---
title: Host a network game without multiplayer maps
category: fix
release: 0.4.0
targets:
- type: key
  id: Description
  scope: map-packets
  effect: changed
credit:
- jarvis-jhk
---

Hosting a network game no longer crashes when the game finds no multiplayer maps. The setup opens without a map, so a campaign can still be chosen.
