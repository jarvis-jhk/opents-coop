---
key: StartingDropships
summary: How many dropships the player fills before a campaign mission begins.
see_also: [AllowableUnits, AllowableUnitMaximums]
when_omitted:
  kind: value
  value: "0"
---

```ini title="map file"
[Basic]
StartingDropships=2
```

Any value above zero opens the dropship loadout screen after the briefing and before the action movie. Each dropship holds five units, and the units the player picks fill the player's three dropship loadouts in order. Zero skips the screen, which is what nearly every mission does.

[`AllowableUnits`](/keys/allowableunits/) decides what the screen offers.

:::danger[Keep the value at 3 or below]
The screen has pictures and slot positions for one to three dropships only. A value of 4 or more reads past the end of both tables, and any units chosen for a fourth dropship are written past the end of the player's three loadouts.
:::

The mission's triggers deliver the loadouts as reinforcements. A reinforcement whose task force is a single `DSHP` entry arrives as a loaded dropship. The loadouts are the player's, and a dropship sent for the player or for another of the mission's [human forces](/formats/campaign-variants/#declare-the-forces) uses them up in order. The units it carries belong to the house it is sent for.

- While the player's next loadout is empty, the reinforcement is refused and the next dropship tries the same loadout.
- Otherwise the dropship carries the units of the player's next loadout, and the player moves on to the following loadout, even when the team then cannot be created and nothing arrives.
- After the player's third delivery, a `DSHP` reinforcement is sent as an ordinary reinforcement with no cargo.

A dropship sent for a house that is not one of the mission's human forces keeps a delivery count of its own. It is refused while the player's next loadout is empty and otherwise takes its cargo from that house's own loadouts, which are always empty.

:::danger[Send dropship reinforcements only for the player's forces]
A `DSHP` reinforcement sent for a house that is not one of the mission's human forces crashes the game while the player's next loadout holds units.
:::

In a [shared-house campaign](/formats/spawn-ini/#a-shared-house-campaign), the host fills the dropships on the loadout screen. The other players wait for at most `ConnTimeout` ticks and start the mission with the same loadouts.danger[Send dropship reinforcements only for the player]
A `DSHP` reinforcement sent for any other house while the player's next loadout holds units crashes the game.
:::
