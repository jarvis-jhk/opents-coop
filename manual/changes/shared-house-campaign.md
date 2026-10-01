---
title: Play campaign missions with one shared house
category: feature
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
- type: command
  id: fixed:leave-shared-campaign
  effect: added
credit:
- jarvis-jhk
---

Campaign launches with several human seats let every player command the mission’s player house. The host chooses the next mission or replay, and each machine restarts with its own updated launch file. See [Client launch file](/formats/spawn-ini/#a-shared-house-campaign) for setup and limits.

Use matching fork builds and target platforms for every peer. Shared campaigns disable saving and waypoint editing and refuse dropship loadout selection. Campaign launches with one human and ordinary multiplayer matches keep their launch modes.
