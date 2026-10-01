---
key: Group
scope: teamtype
label: Team recruitment group
see_also: ["system:ai-team-production", Recruiter]
when_omitted:
  kind: value
  value: "-1"
  note: A TeamType left at -1 takes the group of its TaskForce, which is itself -1 unless that section sets one.
---

A team [recruits](/systems/ai-team-production/#recruitment) only objects in its group. Each object that joins is put in the team's group, so the next team to look for members finds it there.

Two settings widen the search:

- `Group=-2` lets the team consider every object, but any object not already in group `-2` counts as 50 cells farther away than it is. Its members are put in group `-2` when they join.
- [`Recruiter=yes`](/keys/recruiter/) also lets the team consider every object, but it still prefers objects in its group. Any other candidate counts as 50 cells farther away than it is.

An object's group starts at `-1`. A map's [placed-object record](/formats/scenario-objects/) and the [Set Group ID](/mapping/actions/taction-set-group-id/) trigger action can change it. In a campaign or skirmish played alone, assigning a control group also changes it; the first control group is group `0`. In networked games, each player's control groups are local and leave the object's recruitment group unchanged. Unless it is `Recruiter=yes`, a TeamType that leaves both its own and its TaskForce's `Group` unset recruits only objects still in group `-1`.

The group is the same number that Set Group ID assigns and that [Wakeup group](/mapping/actions/taction-wakeup-group/) matches. Joining a team replaces a group a trigger gave the object. In a game played alone, it also changes the object's control group. In a networked game, the local control group remains unchanged.

In networked games, starting a scenario or loading a saved game clears the local control groups. Saved object group values still apply to recruitment and trigger actions; recreate local control groups after loading.
