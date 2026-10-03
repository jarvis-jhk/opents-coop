/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_surface.h"
#include "_ui.h"
#include "addon.h"
#include "campaign.h"
#include "campaignforces.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "mapsel.h"
#include "options.h"
#include "surface.h"
#include "ui/screens/campaign/uicampaign.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"
#include "vector.h"

#include <utility>


/// <summary>
/// Lists the missions a campaign can start with, labeled with their step number and the name in
/// their map file, or their filename when the map has no name. The first entry is the campaign's own first mission,
/// which keeps the stage and mission number a new campaign starts with; a campaign without map
/// selection data offers only that entry, and one without a first mission offers none.
/// </summary>
static std::vector<UICampaignMission> Campaign_Missions(CampaignClass const & campaign)
{
	std::vector<UICampaignMission> missions;
	if (campaign.ScenarioName[0] == '\0') {
		return(missions);
	}
	missions.push_back(UICampaignMission{"", campaign.ScenarioName, -1, 1});

	std::vector<MapSelectMission> stages;
	CCFileClass first(campaign.ScenarioName);
	CCINIClass ini;
	if (first.Is_Available() && ini.Load(first, false) != 0) {
		std::vector<std::string> const forces = Mission_Forces(ini);
		AddonType const addon = Get_Required_Addon();
		Set_Required_Addon((AddonType)campaign.RequiredAddon);
		stages = Map_Select_Missions(forces[0].c_str(), campaign.ScenarioName);
		Set_Required_Addon(addon);
	}
	for (MapSelectMission const & stage : stages) {
		if (_stricmp(stage.Scenario.c_str(), campaign.ScenarioName) != 0) {
			missions.push_back(UICampaignMission{"", stage.Scenario, stage.Stage, stage.Number});
		}
	}

	for (UICampaignMission & mission : missions) {
		char title[128] = "";
		CCFileClass map(mission.File.c_str());
		CCINIClass map_ini;
		if (!mission.File.empty() && map.Is_Available() && map_ini.Load(map, false) != 0) {
			map_ini.Get_String("Basic", "Name", "", title, sizeof(title));
		}
		std::size_t const slash = mission.File.find_last_of("/\\");
		std::string const file = slash == std::string::npos ? mission.File : mission.File.substr(slash + 1);
		mission.Label = std::to_string(mission.Number) + ": " + (title[0] != '\0' ? std::string(title) : file);
	}
	return(missions);
}


void UI_Campaign_State(UICampaignState & state)
{
	state = UICampaignState();

	for (int index = 0; index < Campaigns.Count(); index++) {
		CampaignClass * campaign = Campaigns[index];
		if (campaign == NULL || !Campaign_Available(campaign)) {
			continue;
		}

		UICampaignEntry entry;
		entry.Description = campaign->Description;
		entry.Campaign = index;
		entry.Missions = Campaign_Missions(*campaign);
		state.Entries.push_back(entry);
	}

	for (int index = 0; index < OptionsClass::MAX_DIFFICULTY_SETTING; index++) {
		state.DifficultyNames.push_back(Fetch_String(GameDifficultyNames[index]));
	}

	state.Difficulty = Options.Difficulty;

	if (HiddenSurface != NULL) {
		// The mission row makes the dialog taller, so it starts higher to keep its bottom edge.
		state.Top = (HiddenSurface->Get_Height() - 400) / 2 + 147 - 32;
	}
}


std::optional<UICampaignEntry> UI_Campaign_Dialog(void)
{
	UICampaignState state;
	UI_Campaign_State(state);

	UICampaignPresenterClass presenter(std::move(state));
	std::unique_ptr<UIViewClass> view = UI_Campaign_View(presenter);

	if (UI_Run_Modal(*view) != UI_RESULT_ACCEPTED) {
		return(std::nullopt);
	}

	Options.Difficulty = presenter.State.Difficulty;
	return(presenter.Picked);
}
