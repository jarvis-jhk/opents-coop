---
title: Keep the menu colors in lobby-started shared campaigns
category: fix
release: 0.11.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- jarvis-jhk
---

A shared campaign started from the network lobby gives its players the eight colors the menu offers again. The launch file now numbers a color by its position in the rules' `[Colors]` list, so the lobby writes positions `1`, `10`, `13`, `16`, `19`, `23`, `27` and `36` in seat order instead of `0` to `7`. Seat order, the host's first seat and the session identity are unchanged.
