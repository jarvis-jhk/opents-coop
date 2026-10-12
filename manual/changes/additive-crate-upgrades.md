---
title: Let armor and firepower crates add to the multiplier
category: feature
release: 0.5.0
targets:
- type: key
  id: ArmorCrateStacksAdditively
  effect: added
- type: key
  id: FirepowerCrateStacksAdditively
  effect: added
- type: system
  id: crates
  effect: changed
credit: [ZivDero]
---

`ArmorCrateStacksAdditively` and `FirepowerCrateStacksAdditively` in `[CrateRules]` of `rules.ini` make each armor or firepower upgrade add the corresponding `[Powerups]` entry's third field to the current multiplier at `yes`, including on the first pickup. With the additive settings disabled, each upgrade multiplies by that field.
