---
key: Briefing
scope: mission-descriptions
label: Mission briefing text block
summary: Selects the text block used for a campaign mission's briefing.
when_omitted:
  kind: inherited
  note: The briefing already read from the map is kept.
---

`Briefing=` in a mission's `MISSION.INI` section names the text block used for its briefing. `MISSION1.INI` supplies expansion mission metadata when the mission requests it. The section uses the scenario filename, including its path. A variant without this assignment in its own section reads the original mission's section. An omitted assignment keeps the briefing from the map; an empty assignment also leaves it in place.
