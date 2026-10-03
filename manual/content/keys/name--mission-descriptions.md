---
key: Name
scope: mission-descriptions
label: Mission description
summary: Replaces the campaign mission description with the mission metadata title.
when_omitted:
  kind: inherited
  note: The map's mission description is kept.
---

`Name=` in a mission's `MISSION.INI` section replaces the description read from `[Basic] Name=` in the map. `MISSION1.INI` supplies expansion mission metadata when the mission requests it. The section uses the scenario filename, including its path. A variant without this assignment in its own section reads the original mission's section. Omitting it keeps the map's description; an empty assignment clears the description.
