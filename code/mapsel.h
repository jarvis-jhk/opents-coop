/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>
#include <vector>

class ScenarioClass;


struct MapSelectMission
{
	std::string Scenario;
	int Stage = 0;
	int Number = 1;
};

const char * Map_Selection(ScenarioClass * scenario);
const char * Map_Select_Advance(ScenarioClass * scenario, const char * map_name);
int Map_Select_Stage_Of(ScenarioClass const * scenario, char const * map_name);
std::vector<MapSelectMission> Map_Select_Missions(char const * house_name, char const * first_map);
