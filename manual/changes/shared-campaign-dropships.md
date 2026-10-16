---
title: Fill dropships in shared campaigns
category: feature
release: 0.8.0
targets:
- type: key
  id: StartingDropships
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
credit:
- jarvis-jhk
---

Shared-house campaigns now play missions with dropship loadouts. The host fills the dropships on the loadout screen, and every other player starts the mission with the same loadouts. Such missions used to refuse to start in a shared campaign.

A dropship reinforcement sent for any of a mission's human forces now carries the player's next loadout, and its units belong to that force. A dropship sent for a force other than the player's house used to crash the game while the player's next loadout held units.
