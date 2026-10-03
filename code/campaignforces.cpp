/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "always.h"

#include "campaignforces.h"

#include "_map.h"
#include "building.h"
#include "ccfile.h"
#include "ccini.h"
#include "dbgprint.h"
#include "factory.h"
#include "event.h"
#include "sharedcampaign.h"
#include "ui/screens/forces/uiforces.h"
#include "xstraw.h"
#include "globals.h"
#include "msglist.h"
#include "scenario.h"
#include "super.h"
#include "techtype.h"
#include "gamedirs.h"
#include "house.h"
#include "houstype.h"
#include "session.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <limits>


namespace
{
	HouseClass * CampaignPlayerPtr = NULL;
	char const * const BASIC = "Basic";
	char const * const PLAYER_FORCES = "PlayerForces";
}


/// <summary>
/// Returns the house names a mission's players command, in force order. Force 1 is the house
/// [Basic] Player= names; [PlayerForces] lists further forces in file order. A name listed
/// twice counts once.
/// </summary>
std::vector<std::string> Mission_Forces(CCINIClass const & ini)
{
	std::vector<std::string> forces;

	char buffer[128];
	ini.Get_String(BASIC, "Player", "GDI", buffer, sizeof(buffer));
	forces.emplace_back(buffer);

	int const count = ini.Entry_Count(PLAYER_FORCES);
	for (int index = 0; index < count && (int)forces.size() < ForceControl::MAX_FORCES; index++) {
		char const * entry = ini.Get_Entry(PLAYER_FORCES, index);
		if (entry == NULL) {
			continue;
		}
		ini.Get_String(PLAYER_FORCES, entry, "", buffer, sizeof(buffer));
		if (buffer[0] == '\0') {
			continue;
		}

		bool listed = false;
		for (std::string const & force : forces) {
			if (_stricmp(force.c_str(), buffer) == 0) {
				listed = true;
				break;
			}
		}
		if (!listed) {
			forces.emplace_back(buffer);
		}
	}

	return(forces);
}


/// <summary>
/// Reads what the variant chooser shows of one mission file. Returns false when the file is
/// not available.
/// </summary>
static bool Read_Mission_Variant(std::string const & file, MissionVariant & variant)
{
	CCINIClass ini;
	if (Scen->SourceFile.Matches(file.c_str())) {
		BufferStraw straw(Scen->SourceFile.Data(), Scen->SourceFile.Size());
		if (ini.Load(straw, false, false, file.c_str()) == 0) {
			return(false);
		}
	} else {
		CCFileClass source(file.c_str());
		if (!source.Is_Available() || ini.Load(source, false) == 0) {
			return(false);
		}
	}

	char buffer[128];
	variant.File = file;
	ini.Get_String(BASIC, "Name", "", buffer, sizeof(buffer));
	variant.Name = buffer;

	std::string const tag = ForceControl::Variant_Tag(file);
	ini.Get_String(BASIC, "VariantName", tag.empty() ? "Original" : tag.c_str(), buffer, sizeof(buffer));
	variant.Label = buffer;

	variant.Forces = Mission_Forces(ini);
	return(true);
}


/// <summary>
/// Lists the versions of a campaign mission: the stock mission first, then every variant
/// file beside it, sorted by name. Variants are loose files, found in the directories the
/// game reads from; the stock mission may also come from an archive. A variant named in
/// place of its stock mission finds the same list.
/// </summary>
std::vector<MissionVariant> Find_Mission_Variants(char const * scenario)
{
	std::vector<MissionVariant> variants;
	if (scenario == NULL || scenario[0] == '\0') {
		return(variants);
	}

	std::string const base = ForceControl::Base_Name(scenario);

	MissionVariant variant;
	if (Read_Mission_Variant(base, variant)) {
		variants.push_back(variant);
	}

	std::string const pattern = ForceControl::Variant_Pattern(base);
	std::size_t const slash = base.find_last_of("/\\");
	std::string const directory = slash == std::string::npos ? std::string() : base.substr(0, slash + 1);

	for (std::string const & name : Search_Files(pattern.c_str())) {
		if (!ForceControl::Variant_Tag(name).empty()
			&& _stricmp(ForceControl::Base_Name(directory + name).c_str(), base.c_str()) == 0
			&& Read_Mission_Variant(directory + name, variant)) {
			variants.push_back(variant);
		}
	}

	if (!ForceControl::Variant_Tag(scenario).empty() && Scen->SourceFile.Matches(scenario)) {
		bool const listed = std::any_of(variants.begin(), variants.end(), [scenario](MissionVariant const & entry) {
			return(_stricmp(entry.File.c_str(), scenario) == 0);
		});
		if (!listed && Read_Mission_Variant(scenario, variant)) {
			variants.push_back(variant);
			std::sort(variants.begin(), variants.end(), [&base](MissionVariant const & left, MissionVariant const & right) {
				bool const left_stock = _stricmp(left.File.c_str(), base.c_str()) == 0;
				bool const right_stock = _stricmp(right.File.c_str(), base.c_str()) == 0;
				return(left_stock != right_stock ? left_stock : _stricmp(left.File.c_str(), right.File.c_str()) < 0);
			});
		}
	}

	DebugString("Mission %s has %d version(s)\n", base.c_str(), (int)variants.size());
	return(variants);
}


/// <summary>
/// Returns the section name MISSION.INI keeps a mission's name and briefing under. A variant
/// without its own section uses its stock mission's.
/// </summary>
std::string Mission_INI_Key(char const * scenario)
{
	return(ForceControl::Base_Name(scenario != NULL ? scenario : ""));
}


bool Choose_Mission_Forces(std::string & scenario)
{
	if (Session.IsSharedHouse) {
		std::vector<int> const order = Seat_Player_Order();
		for (int seat = 0; seat < (int)order.size(); seat++) {
			Session.Players[order[seat]]->Player.ID = seat;
		}
	}
	if (!Session.IsSharedHouse || Session.Local_Seat_ID() == Session.Master_Player_ID()) {
		std::vector<MissionVariant> const variants = Find_Mission_Variants(scenario.c_str());
		if (variants.empty()) {
			return(false);
		}
		UIForcesState state;
		state.Mission = variants[0].Name;
		if (Session.IsSharedHouse) {
			for (int index : Seat_Player_Order()) {
				state.Players.emplace_back(Session.Players[index]->Name);
			}
		} else {
			state.Players.emplace_back("Player 1");
		}
		std::vector<int> counts;
		for (MissionVariant const & variant : variants) {
			state.Variants.push_back(UIForcesVariant{variant.Label, variant.Forces});
			counts.push_back((int)variant.Forces.size());
		}
		state.Selected = ForceControl::Best_Variant(counts, (int)state.Players.size());
		if (!UI_Forces_Dialog(state)) {
			if (Session.IsSharedHouse) {
				scenario.clear();
				Shared_Campaign_Setup(scenario, Session.ForceSeats);
			}
			return(false);
		}
		scenario = variants[state.Selected].File;
		Session.ForceSeats = state.Control;
	}
	return(!Session.IsSharedHouse || Shared_Campaign_Setup(scenario, Session.ForceSeats));
}


/// <summary>
/// Returns the indices into Session.Players in seat order: the lowest color first, so every
/// machine numbers the seats alike.
/// </summary>
std::vector<int> Seat_Player_Order(void)
{
	std::vector<int> order;
	std::vector<bool> taken(Session.Players.Count(), false);
	for (int i = 0; i < Session.Players.Count(); i++) {
		int index = -1;
		for (int j = 0; j < Session.Players.Count(); j++) {
			if (!taken[j] && (index == -1 || Session.Players[j]->Player.Color < Session.Players[index]->Player.Color)) {
				index = j;
			}
		}
		taken[index] = true;
		order.push_back(index);
	}
	return(order);
}


/// <summary>
/// Settles the forces of the mission being read: their houses, which seat controls which, and
/// the force each seat starts acting for. Each force is played by people, never by the
/// computer. Returns false for a missing house or an invalid shared control matrix. Must run
/// once the mission's houses exist.
/// </summary>
bool Assign_Mission_Forces(CCINIClass const & ini)
{
	std::vector<std::string> const names = Mission_Forces(ini);

	CampaignPlayerPtr = PlayerPtr;
	Session.ForceCount = 0;
	for (int force = 0; force < ForceControl::MAX_FORCES; force++) {
		Session.ForceHouse[force] = -1;
	}

	for (std::string const & name : names) {
		HouseClass * house = NULL;
		if (Session.ForceCount == 0) {
			house = PlayerPtr;
		} else {
			HousesType const type = HouseTypeClass::From_Name(name.c_str());
			house = type != HOUSE_NONE ? House_From_HousesType(type) : NULL;
		}
		if (house == NULL) {
			DebugString("Force %d, %s, names no house of this mission\n", Session.ForceCount + 1, name.c_str());
			return(false);
		}

		house->IsHuman = true;
		house->IsPlayerControl = true;
		Session.ForceHouse[Session.ForceCount++] = house->HeapID;
		DebugString("Force %d is %s, house %d\n", Session.ForceCount, house->Class->IniName.c_str(), house->HeapID);
	}

	int const seats = Session.IsSharedHouse ? Session.Players.Count() : 1;
	if (!ForceControl::Is_Valid(Session.ForceSeats, Session.ForceCount, seats)) {
		if (Session.IsSharedHouse) {
			return(false);
		}
		Session.ForceSeats = ForceControl::Default(Session.ForceCount, seats);
	}

	for (int seat = 0; seat < MAX_PLAYERS; seat++) {
		int const force = seat < seats ? ForceControl::First_Force(Session.ForceSeats, seat, Session.ForceCount) : -1;
		Session.SeatForce[seat] = force >= 0 ? Session.ForceHouse[force] : -1;
	}
	return(true);
}


int Force_Count(void)
{
	return(Session.ForceCount);
}


HouseClass * Force_House(int force)
{
	if (force < 0 || force >= Session.ForceCount) {
		return(NULL);
	}
	int const house = Session.ForceHouse[force];
	return(house >= 0 && house < Houses.Count() ? Houses[house] : NULL);
}


/// <summary>
/// Returns the force number, from 0, a house plays in this mission, or -1 for a house that is
/// no force.
/// </summary>
int Force_Of(HouseClass const * house)
{
	if (house == NULL) {
		return(-1);
	}
	for (int force = 0; force < Session.ForceCount; force++) {
		if (Session.ForceHouse[force] == house->HeapID) {
			return(force);
		}
	}
	return(-1);
}


/// <summary>
/// May the player at this machine give orders to this house's objects and show its sidebar?
/// Alone, to every house the mission lets the player command; in a shared campaign, to the
/// forces the control matrix gives this machine's seat.
/// </summary>
bool Is_Local_Force(HouseClass const * house)
{
	if (house == NULL) {
		return(false);
	}
	if (Session.IsSharedHouse) {
		return(Session.Seat_Controls(Session.Local_Seat_ID(), house));
	}
	if (Session.Type == GAME_NORMAL) {
		return(house->Is_Player_Control());
	}
	return(house == PlayerPtr);
}


/// <summary>
/// Returns the house every machine treats as the campaign's player: in a campaign mission
/// the house [Basic] Player= names, whose view all forces share and whose win or loss ends
/// the mission; in any other game this machine's own house. Unlike PlayerPtr, it does not
/// change when the player shows another force.
/// </summary>
HouseClass * Campaign_Player(void)
{
	if (Session.Type == GAME_NORMAL && CampaignPlayerPtr != NULL) {
		return(CampaignPlayerPtr);
	}
	return(PlayerPtr);
}


/// <summary>
/// Is this house played by a person rather than by the mission? In a campaign mission that
/// is each of its forces; in any other game only this machine's own house.
/// </summary>
bool Is_Players_House(HouseClass const * house)
{
	if (house == NULL) {
		return(false);
	}
	if (Session.Type == GAME_NORMAL && CampaignPlayerPtr != NULL) {
		return(Force_Of(house) >= 0);
	}
	return(house == PlayerPtr);
}


/// <summary>
/// Does a trigger's house selector name the player? In a campaign mission it does when it
/// matches any force.
/// </summary>
bool Players_Match(HousesType selector)
{
	if (Session.Type == GAME_NORMAL && CampaignPlayerPtr != NULL) {
		for (int force = 0; force < Session.ForceCount; force++) {
			if (House_Matches(Force_House(force), selector)) {
				return(true);
			}
		}
		return(false);
	}
	return(House_Matches(PlayerPtr, selector));
}


/// <summary>
/// Returns the house whose waypoint paths an object of this house follows: a force follows
/// its own, and any other house the campaign player's.
/// </summary>
HouseClass * Waypoint_House(HouseClass * house)
{
	if (Session.Type == GAME_NORMAL && CampaignPlayerPtr != NULL) {
		return(Force_Of(house) >= 0 ? house : CampaignPlayerPtr);
	}
	return(PlayerPtr);
}


/// <summary>
/// Returns the money every force has left, which a won mission carries over.
/// </summary>
int Forces_Money(void)
{
	if (Session.ForceCount == 0 || CampaignPlayerPtr == NULL) {
		return(Campaign_Player()->Available_Money());
	}
	std::int64_t money = 0;
	for (int force = 0; force < Session.ForceCount; force++) {
		HouseClass * house = Force_House(force);
		if (house != NULL) {
			money += house->Available_Money();
		}
	}
	return((int)std::clamp<std::int64_t>(money, 0, std::numeric_limits<int>::max()));
}


/// <summary>
/// Settles the forces of a campaign mission restored from a saved game. A save keeps no
/// control matrix: it is played alone, so every house the player commands is a force.
/// </summary>
void Restore_Mission_Forces(void)
{
	CampaignPlayerPtr = NULL;
	Session.ForceCount = 0;
	if (Session.Type != GAME_NORMAL) {
		return;
	}

	CampaignPlayerPtr = House_From_HousesType(Scen->PlayerHouse);
	if (CampaignPlayerPtr == NULL) {
		CampaignPlayerPtr = PlayerPtr;
	}
	Session.ForceHouse[Session.ForceCount++] = CampaignPlayerPtr->HeapID;
	for (int index = 0; index < Houses.Count() && Session.ForceCount < ForceControl::MAX_FORCES; index++) {
		HouseClass * house = Houses[index];
		if (house != CampaignPlayerPtr && house->IsHuman && house->IsPlayerControl) {
			Session.ForceHouse[Session.ForceCount++] = house->HeapID;
		}
	}
	Session.ForceSeats = ForceControl::Default(Session.ForceCount, 1);
}


/// <summary>
/// Forgets the forces of the mission that has ended.
/// </summary>
void Clear_Mission_Forces(void)
{
	CampaignPlayerPtr = NULL;
	Session.ForceCount = 0;
}


/// <summary>
/// Shows another of this player's forces: its sidebar, money, power and radar. Orders already
/// given stay with their forces; the sidebar's next orders act for the force shown. Pending
/// placement and the sell, repair and targeting modes are dropped, since they belonged to the
/// force shown before. Does nothing for a house this player does not control.
/// </summary>
void Show_Force(HouseClass * house)
{
	if (house == NULL || house == PlayerPtr || PlayerPtr == NULL || Force_Of(house) < 0 || !Is_Local_Force(house)) {
		return;
	}

	if (Map.PendingObject != NULL) {
		Map.PendingObject = NULL;
		Map.PendingObjectPtr = NULL;
		Map.PendingHouse = HOUSE_NONE;
		Map.Set_Cursor_Shape(0);
	}
	Map.IsTargettingMode = SUPER_NONE;
	Map.Sell_Mode_Control(0);
	Map.Repair_Mode_Control(0);
	Map.Power_Mode_Control(0);

	if (Session.IsSharedHouse) {
		OutList.push_back(EventClass(Session.Local_Seat_ID(), EventClass::FORCE, house->HeapID));
	}
	PlayerPtr = house;

	Map.Column[0].Init_Clear();
	Map.Column[1].Init_Clear();
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building != NULL && building->IsActive && building->House == house && !building->IsInLimbo) {
			building->Update_Buildables();
		}
	}
	for (int index = 0; index < house->SuperWeapon.Count(); index++) {
		if (house->SuperWeapon[index] != NULL && house->SuperWeapon[index]->Is_Present()) {
			Map.Add(RTTI_SPECIAL, index);
		}
	}
	for (RTTIType rtti : {RTTI_BUILDINGTYPE, RTTI_UNITTYPE, RTTI_INFANTRYTYPE, RTTI_AIRCRAFTTYPE}) {
		FactoryClass * factory = house->Fetch_Factory(rtti);
		TechnoClass * object = factory != NULL ? factory->Get_Object() : NULL;
		if (object != NULL) {
			TechnoTypeClass const * type = object->Techno_Type_Class();
			Map.Factory_Link(factory, (RTTIType)type->What_Am_I(), type->Fetch_Heap_ID());
		}
	}
	Map.Recalc();
	Map.SidebarClass::IsToRedraw = true;
	Map.IsToBlitSidebar = true;
	Map.Column[0].Flag_To_Redraw();
	Map.Column[1].Flag_To_Redraw();

	Map.PowerClass::IsToRedraw = true;
	Map.Credits.Current = house->Available_Money();
	Map.Credits.IsAudible = false;
	Map.Credits.IsToRedraw = true;
	Map.IsToRedrawCredits = true;

	house->Recalc_Radar_Availability();
	Map.Complete_Radar_Refresh();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);

	char message[96];
	std::snprintf(message, sizeof(message), "Showing force %d: %s", Force_Of(house) + 1, house->Class->IniName.c_str());
	Session.Messages.Add_Message(NULL, 0, message, house->Scheme,
		TextPrintType(TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW), TICKS_PER_SECOND * 4);
	DebugString("Showing force %d, house %d\n", Force_Of(house) + 1, house->HeapID);
}


/// <summary>
/// Shows the next force this player controls after the one shown, wrapping around.
/// </summary>
void Show_Next_Force(void)
{
	int const current = Force_Of(PlayerPtr);
	for (int step = 1; step <= Session.ForceCount; step++) {
		HouseClass * house = Force_House((current + step) % Session.ForceCount);
		if (house != NULL && house != PlayerPtr && Is_Local_Force(house)) {
			Show_Force(house);
			return;
		}
	}
}
