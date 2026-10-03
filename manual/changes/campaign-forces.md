---
title: Select campaign variants and assign players to forces
category: feature
release: 0.2.0
targets:
- type: format
  id: campaign-variants
  effect: added
- type: command
  id: NextForce
  effect: added
- type: format
  id: spawn-ini
  effect: changed
- type: key
  id: VariantName
  effect: added
- type: key
  id: Briefing
  effect: added
- type: key
  id: Name
  scope: mission-descriptions
  effect: added
credit:
- jarvis-jhk
---

Campaign missions offer the original and loose variants, with a player-by-force checkbox matrix before loading. Players can command several forces or share one. Selecting an owned object switches its force's sidebar; `NextForce` cycles the forces a player controls.

Use matching updated fork builds and target platforms for every peer. Earlier shared-house snapshots cannot exchange the new setup packet or force-switch event. Finish or abandon a running session before updating all machines together. See [Campaign mission variants](/formats/campaign-variants/) for authoring and play.
