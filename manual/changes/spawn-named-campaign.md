---
title: Name a launch file's campaign by its identifier
category: feature
release: 0.7.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- jarvis-jhk
---

A launch file can name its campaign with [`Campaign`](/formats/spawn-ini/#a-campaign-mission), such as `Campaign=GDI1`, instead of numbering it with `CampaignID`. Without a `Scenario`, the launch starts that campaign's first mission. The same file then starts the same campaign on installations whose battle files number the campaigns differently.
