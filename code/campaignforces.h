/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include "forcecontrol.h"

#include <string>
#include <vector>

#include "house.hh"

class CCINIClass;
class HouseClass;


// One playable version of a campaign mission: the stock file or a variant beside it.
struct MissionVariant
{
	std::string File;
	std::string Name;
	std::string Label;
	std::vector<std::string> Forces;
};


std::vector<std::string> Mission_Forces(CCINIClass const & ini);
std::vector<MissionVariant> Find_Mission_Variants(char const * scenario);
std::string Mission_INI_Key(char const * scenario);
bool Choose_Mission_Forces(std::string & scenario);

std::vector<int> Seat_Player_Order(void);
bool Assign_Mission_Forces(CCINIClass const & ini);
int Force_Count(void);
HouseClass * Force_House(int force);
int Force_Of(HouseClass const * house);
bool Is_Local_Force(HouseClass const * house);
void Restore_Mission_Forces(void);
void Clear_Mission_Forces(void);

HouseClass * Campaign_Player(void);
bool Is_Players_House(HouseClass const * house);
bool Players_Match(HousesType selector);
HouseClass * Waypoint_House(HouseClass * house);
int Forces_Money(void);

void Show_Force(HouseClass * house);
void Show_Next_Force(void);
