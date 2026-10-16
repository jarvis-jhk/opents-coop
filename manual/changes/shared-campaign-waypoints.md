---
title: Plot waypoint paths in shared campaigns
category: feature
release: 0.6.0
targets:
- type: system
  id: waypoint-paths
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
credit:
- jarvis-jhk
---

Every player in a shared-house campaign can now plot [waypoint paths](/systems/waypoint-paths/) and send units along them. Each player sees only their own paths, and an order from another player takes a unit off its path. Waypoint mode used to be unavailable in shared campaigns.

In a campaign with several forces, a click on a waypoint now sends only the selected objects of the shown force along its path; the others get the order the click gives them without a path. A selection that mixed forces used to send an object along its own force's path of the same number, or crash the game when that path was shorter or unused.
