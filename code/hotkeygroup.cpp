/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "always.h"

#include "hotkeygroup.h"

#include "session.h"
#include "techno.h"

#include <unordered_map>


// The groups this machine's player made in a networked game, by object id.
static std::unordered_map<int, int> LocalGroups;


/// <summary>
/// Returns the hotkey group the player at this machine put an object in, or -1 for none. A
/// game played alone keeps it in the object's Group, which the mission's triggers and teams
/// also read. A networked game keeps it at this machine only, because the other machines
/// never learn of it and Group is part of the game they compare.
/// </summary>
int Hotkey_Group(TechnoClass const * object)
{
	if (object == NULL) {
		return(-1);
	}
	if (!Session.Is_Networked()) {
		return(object->Group);
	}
	auto found = LocalGroups.find(object->Fetch_ID());
	return(found != LocalGroups.end() ? found->second : -1);
}


/// <summary>
/// Puts an object in a hotkey group, or in none for -1, where Hotkey_Group reads it.
/// </summary>
void Set_Hotkey_Group(TechnoClass * object, int group)
{
	if (object == NULL) {
		return;
	}
	if (!Session.Is_Networked()) {
		object->Group = group;
		return;
	}
	if (group < 0) {
		LocalGroups.erase(object->Fetch_ID());
	} else {
		LocalGroups[object->Fetch_ID()] = group;
	}
}


/// <summary>
/// Forgets every group a networked game kept at this machine, as a new scenario begins.
/// </summary>
void Clear_Hotkey_Groups(void)
{
	LocalGroups.clear();
}
