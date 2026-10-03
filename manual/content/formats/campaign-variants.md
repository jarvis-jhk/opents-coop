---
format_id: campaign-variants
title: Campaign mission variants
summary: Campaign mission files declare human forces and provide selectable versions for different player counts.
kind: file
source_files:
  - code/campaignforces.cpp
  - code/forcecontrol.cpp
  - code/scenario.cpp
  - code/sharedcampaign.cpp
filenames:
  - <mission>.<tag>.MAP
  - <mission>.<tag>.INI
related:
  - type: format
    id: spawn-ini
  - type: command
    id: NextForce
---

Every campaign mission offers a version chooser before loading, including when played alone. The original mission is listed first; loose variants appear in filename order. The chooser presets the version with the force count closest to the player count. A tie favors the larger force count, then the earlier version. All listed versions remain selectable.

## Name a variant

Copy the original mission to a loose file beside it and insert a tag before its extension. `GDI1A.coop2.MAP` is a variant of `GDI1A.MAP`. Keep the original filename available so players can select it. For shared play, keep the complete scenario filename, including its directory and tag, below 64 characters. The original may be in a game archive; variants must be loose files in the game's search folders. A launch naming a variant finds the same version list as a launch naming its original.

Use `[Basic] VariantName=` for the chooser's version label. Without it, the original is labeled `Original` and a variant uses its filename tag. A variant without its own name or briefing entry in `MISSION.INI` uses the original mission's entry for that field.

## Declare the forces

A force is one mission house controlled by people. Force 1 is the house named by `[Basic] Player=`. Add further houses in `[PlayerForces]`, in entry order:

```ini
[Basic]
Player=GDI
VariantName=Two bases

[PlayerForces]
1=GDI2
```

Declare `GDI2` in the mission's `[Houses]` list and configure its house, units, structures, and allies as for any other mission house. Listing a house does not create its base, resources, or objectives. Repeated house names count once, ignoring case; empty entries are ignored. Existing campaign `PlayerControl=yes` houses retain their single-player command behavior; declaring them as forces adds their own sidebar. Use at most eight distinct forces, including the main house. Further entries are ignored. A listed house absent from the loaded mission refuses loading.

The chooser presents players as rows and forces as columns. Each checked box lets that player command that force. The preset assigns player 1 to force 1, player 2 to force 2, and wraps: extra forces return to player 1; extra players return to force 1. Every force needs at least one player, and every player needs at least one force. Several players may share a force, and one player may control several forces.

In a shared campaign, player order follows the roster's colors, lowest first. The lowest seat chooses the version and matrix and sends them to the other machines before loading. All machines need the chosen file with identical contents. Cancel ends setup for the other machines too. A single-player test gives its only player every force.

## Play and edit a mission

Selecting a unit or structure of a declared force you control shows that force's credits, power, production queues, and radar. [`NextForce`](/commands/nextforce/) reaches another force without selecting an object. Switching cancels pending building placement and sell, repair, power, and superweapon targeting modes. Queued production remains with its house; subsequent sidebar orders act for the displayed force. Object orders act for their object's owner.

The human forces share the main house's campaign visibility. The mission's AI treats all declared forces as human houses. Configure alliances explicitly so their units do not attack one another.

A win action naming any declared force wins the campaign mission; a loss action naming any declared force loses it. Edit the mission's triggers if success must require both bases or if losing one base should be survivable. Selecting a variant does not rewrite its triggers. Unspent credits from all forces are summed for carry-over and the next mission applies its percentage and cap to that sum, paying the main house. The sum is limited to the largest signed 32-bit integer before the percentage and cap apply.

Keep `NextScenario=` and `AltNextScenario=` pointing to original mission filenames. The next mission presents its version chooser again, so a player can change versions at each step. A restart also offers the chooser. A mission copy carried in a save remains selectable even if its loose file has been removed; restarting that version uses the carried copy. Single-player saves restore the human houses as forces without asking for a matrix; the single player controls them all. Shared campaigns retain their [save, waypoint, dropship, and roster limits](/formats/spawn-ini/#a-shared-house-campaign).

Use the same fork build and target platform for all peers. Earlier shared-house snapshots do not understand the setup packet and force-switch event; finish or abandon an existing session before updating every machine together.
