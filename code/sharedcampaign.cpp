/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "always.h"

#include "sharedcampaign.h"

#include "_keyboar.h"
#include "_surface.h"
#include "_xmouse.h"
#include "conquer.h"
#include "dbgprint.h"
#include "dialog.h"
#include "enviro.h"
#include "gscreen.h"
#include "house.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language/language.h"
#include "mapsel.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netglobal.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "spawner.h"
#include "stimer.h"
#include "surface.h"

#include <cstdio>
#include <cstring>


namespace {
	// The master's choice. It can arrive while this machine still shows the score screen.
	bool Received = false;
	int ReceivedStage = -1;
	char ReceivedScenario[64] = "";

	// The master's mission version and force control. It can arrive while this machine loads.
	bool ReceivedSetup = false;
	char SetupScenario[64] = "";
	unsigned char SetupControl[8] = {};
}


/// <summary>
/// Records the mission the master named for the shared campaign to play next.
/// </summary>
void Shared_Campaign_Receive(GlobalPacketType const & packet)
{
	if (!Session.IsSharedHouse) {
		return;
	}
	if (packet.Command == NET_SHARED_SETUP) {
		ReceivedSetup = true;
		std::snprintf(SetupScenario, sizeof(SetupScenario), "%s", packet.SharedSetup.Scenario);
		std::memcpy(SetupControl, packet.SharedSetup.Control, sizeof(SetupControl));
		DebugString("Shared campaign: the master names version %s\n", SetupScenario);
		return;
	}
	Received = true;
	ReceivedStage = packet.SharedMission.Stage;
	std::snprintf(ReceivedScenario, sizeof(ReceivedScenario), "%s", packet.SharedMission.Scenario);
	DebugString("Shared campaign: the master names stage %d, %s\n", ReceivedStage, ReceivedScenario);
}


/// <summary>
/// Sends a decision of the master to every other machine and waits, at most the connection
/// timeout, until each has acknowledged it.
/// </summary>
static void Shared_Campaign_Broadcast(GlobalPacketType & packet)
{
	std::snprintf(packet.Name, sizeof(packet.Name), "%s", Session.Players[0]->Name);

	for (int index = 1; index < Session.Players.Count(); index++) {
		Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &Session.Players[index]->Address);
	}

	CDTimerClass<SystemTimerClass> timer = Session.ConnTimeout;
	while (Ipx.Global_Num_Send() > 0 && timer > 0) {
		Ipx.Service();
		Call_Back();
		Windows_Message_Handler();
		Sleep(10);
	}
}


/// <summary>
/// Names the next mission to every other machine and waits, at most the connection timeout,
/// until each has acknowledged it.
/// </summary>
static void Shared_Campaign_Send(int stage, char const * scenario)
{
	GlobalPacketType packet{};
	NetGlobal::Initialize_Packet(packet, NET_SHARED_MISSION);
	packet.SharedMission.Stage = stage;
	std::snprintf(packet.SharedMission.Scenario, sizeof(packet.SharedMission.Scenario), "%s", scenario);
	Shared_Campaign_Broadcast(packet);
	DebugString("Shared campaign: named stage %d, %s, to the others\n", stage, scenario);
}


/// <summary>
/// Shows one line of text on an otherwise empty screen.
/// </summary>
static void Shared_Campaign_Show(char const * text)
{
	HiddenSurface->Fill(0);
	Rect const rect = HiddenSurface->Get_Rect();
	Fancy_Text_Print(text, *HiddenSurface, rect, Point2D(rect.Width / 2, rect.Height / 2),
		Fetch_Scheme_By_Name("Green"), TBLACK, TextPrintType(TPF_CENTER|TPF_6PT_GRAD|TPF_USE_GRAD_PAL|TPF_FULLSHADOW));
	Update_Visible_Surface(HiddenSurface);
}


/// <summary>
/// Waits for the master's decision, until the flag its arrival sets is up.
/// </summary>
/// <returns>bool; Did the master name one? False when this player leaves with Esc or every
/// other machine has gone during a connected wait, or the connection timeout expires.</returns>
static bool Shared_Campaign_Wait(bool const & received, bool connected = true)
{
	char text[160];
	char const * master = Session.MasterPlayerName;
	for (int index = 0; index < Session.Players.Count(); index++) {
		if (Session.Players[index]->Player.ID == Session.Master_Player_ID()) {
			master = Session.Players[index]->Name;
			break;
		}
	}
	std::snprintf(text, sizeof(text), "Waiting for %s to choose. Press Esc to leave.", master);

	CDTimerClass<SystemTimerClass> timer = Session.ConnTimeout;
	while (!received) {
		Shared_Campaign_Show(text);
		Call_Back();
		Windows_Message_Handler();

		if (Keyboard->Check() && Keyboard->Get() == KN_ESC) {
			DebugString("Shared campaign: left while waiting for the master\n");
			return(false);
		}
		if (timer == 0) {
			DebugString("Shared campaign: the master did not choose before the connection timeout\n");
			return(false);
		}
		if (connected && Ipx.Num_Connections() == 0) {
			DebugString("Shared campaign: every other machine has gone\n");
			return(false);
		}
		Sleep(10);
	}
	return(true);
}


/// <summary>
/// Settles what a shared house campaign plays after the mission that just ended. The master
/// chooses: on a win from the map selection screen, on a loss by answering whether to replay.
/// Every other machine waits for that choice. The game then starts again on the chosen
/// mission, carrying the flags and money a won mission passes on.
/// </summary>
/// <param name="won">Was the mission won?</param>
/// <returns>bool; Is another mission to be played? False ends the campaign at this machine.</returns>
bool Shared_Campaign_Next(bool won)
{
	if (won) {
		Environment.Store();
	}

	int stage = -1;
	char scenario[64] = "";

	if (Session.Am_I_Master()) {
		if (won) {
			if (Scen->IsNoMapSel) {
				if (Map_Select_Advance(Scen, Scen->GlobalFlags[1].Value ? Scen->AltNextScenarioName : Scen->NextScenarioName) == nullptr) {
					Scen->Set_Scenario_Name("");
				}
			} else {
				Show_Mouse();
				if (Map_Selection(Scen) == nullptr) {
					Scen->Set_Scenario_Name("");
				}
				Hide_Mouse();
			}
		} else if (WWMessageBox().Process(TXT_TO_REPLAY, TXT_YES, TXT_NO) != 0) {
			Scen->Set_Scenario_Name("");
		}

		if (Scen->ScenarioName[0] != '\0') {
			stage = Scen->Stage;
			std::snprintf(scenario, sizeof(scenario), "%s", Scen->ScenarioName);
		}
		Shared_Campaign_Send(stage, scenario);
	} else {
		if (!Shared_Campaign_Wait(Received)) {
			return(false);
		}
		stage = ReceivedStage;
		std::snprintf(scenario, sizeof(scenario), "%s", ReceivedScenario);
	}

	if (stage < 0 || scenario[0] == '\0') {
		DebugString("Shared campaign: the campaign ends here\n");
		return(false);
	}

	return(Spawner_Continue_Shared_Campaign(scenario, stage, won));
}


/// <summary>
/// Agrees the mission version and force control of a shared campaign before the mission
/// loads. The master names its own choice to every other machine; each other machine waits
/// for it and takes it over.
/// </summary>
/// <param name="scenario">The master's chosen mission file; replaced by it at the others.</param>
/// <param name="control">The master's control matrix; replaced by it at the others.</param>
/// <returns>bool; Was a version agreed? False when this player left or the master never chose.</returns>
bool Shared_Campaign_Setup(std::string & scenario, ForceControl::Matrix & control)
{
	if (Session.Local_Seat_ID() == Session.Master_Player_ID()) {
		GlobalPacketType packet{};
		NetGlobal::Initialize_Packet(packet, NET_SHARED_SETUP);
		if (scenario.size() >= sizeof(packet.SharedSetup.Scenario)) {
			DebugString("Shared campaign scenario name is too long\n");
			scenario.clear();
		}
		std::snprintf(packet.SharedSetup.Scenario, sizeof(packet.SharedSetup.Scenario), "%s", scenario.c_str());
		std::memcpy(packet.SharedSetup.Control, control.data(), sizeof(packet.SharedSetup.Control));
		Shared_Campaign_Broadcast(packet);
		DebugString("Shared campaign: named version %s to the others\n", scenario.c_str());
		return(!scenario.empty());
	}

	if (!Shared_Campaign_Wait(ReceivedSetup, false)) {
		return(false);
	}
	ReceivedSetup = false;
	scenario = SetupScenario;
	std::memcpy(control.data(), SetupControl, sizeof(SetupControl));
	return(!scenario.empty());
}
