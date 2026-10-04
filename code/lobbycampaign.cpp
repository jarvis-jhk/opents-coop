/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "lobbycampaign.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>


namespace LobbyCampaign {

namespace {

// The launch file holds at most this many seats.
int const SEAT_MAX = 8;

// A campaign is played at one of three difficulties; the computer's is the mirror of the player's.
int const DIFFICULTY_COUNT = 3;

// The scenario name travels in a 64-byte field of the shared campaign packets.
std::size_t const SCENARIO_MAX = 63;


bool Same_Name(std::string const & left, std::string const & right)
{
	return(left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin(),
		[](char a, char b) { return(std::tolower((unsigned char)a) == std::tolower((unsigned char)b)); }));
}


bool Name_Before(std::string const & left, std::string const & right)
{
	return(std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end(),
		[](char a, char b) { return(std::tolower((unsigned char)a) < std::tolower((unsigned char)b)); }));
}


std::string Dotted(std::uint32_t ip)
{
	unsigned char quad[4];
	std::memcpy(quad, &ip, sizeof(quad));

	char text[16];
	std::snprintf(text, sizeof(text), "%u.%u.%u.%u", quad[0], quad[1], quad[2], quad[3]);
	return(text);
}


void Put(std::string & out, char const * key, std::string const & value)
{
	out += key;
	out += '=';
	out += value;
	out += '\n';
}


void Put(std::string & out, char const * key, int value)
{
	Put(out, key, std::to_string(value));
}

}


/// <summary>
/// Returns the name a seat carries in the launch file: the lobby name with every character the
/// file format cannot hold in a value replaced by an underscore.
/// </summary>
std::string Seat_Name(std::string const & name)
{
	std::string safe = name;
	for (char & c : safe) {
		if (c == ';' || c == '\r' || c == '\n' || c == '[' || c == ']') {
			c = '_';
		}
	}
	return(safe);
}


/// <summary>
/// Returns the seats in the order every machine numbers them: the lobby host first, then the
/// others by name, ignoring case. The host's first seat makes it the shared campaign's master.
/// </summary>
std::vector<Seat> Seat_Order(Launch const & launch)
{
	std::vector<Seat> order = launch.Seats;
	std::stable_sort(order.begin(), order.end(), [&launch](Seat const & left, Seat const & right) {
		bool const left_host = Same_Name(left.Name, launch.Host);
		bool const right_host = Same_Name(right.Name, launch.Host);
		if (left_host != right_host) {
			return(left_host);
		}
		return(Name_Before(left.Name, right.Name));
	});
	return(order);
}


/// <summary>
/// Judges whether the lobby state describes a shared campaign the launch file can start.
/// </summary>
/// <param name="fault">Receives a sentence naming the first problem found.</param>
/// <returns>bool; Can every machine start this campaign?</returns>
bool Is_Launchable(Launch const & launch, std::string & fault)
{
	fault.clear();

	if (launch.Campaign < 0) {
		fault = "No campaign is chosen.";
	} else if (launch.Scenario.empty() || launch.Scenario.size() > SCENARIO_MAX) {
		fault = "The mission file name is empty or longer than 63 characters.";
	} else if (launch.Difficulty < 0 || launch.Difficulty >= DIFFICULTY_COUNT) {
		fault = "The difficulty is not one of the three campaign difficulties.";
	} else if (launch.Stage < -1 || launch.Stage > 32767) {
		fault = "The map selection stage is out of range.";
	} else if (launch.Seats.size() < 2 || launch.Seats.size() > (std::size_t)SEAT_MAX) {
		fault = "A shared campaign needs two to eight players.";
	} else if (launch.ListenPort < 1 || launch.ListenPort > 65535) {
		fault = "The lobby port is not a valid port.";
	}
	if (!fault.empty()) {
		return(false);
	}

	bool host = false;
	bool local = false;
	for (std::size_t index = 0; index < launch.Seats.size(); index++) {
		Seat const & seat = launch.Seats[index];
		std::string const name = Seat_Name(seat.Name);
		if (name.empty()) {
			fault = "A player has no name.";
			return(false);
		}
		for (std::size_t other = 0; other < index; other++) {
			if (Same_Name(Seat_Name(launch.Seats[other].Name), name)) {
				fault = "Two players share the name " + name + ".";
				return(false);
			}
		}

		bool const is_local = Same_Name(seat.Name, launch.Local);
		host = host || Same_Name(seat.Name, launch.Host);
		local = local || is_local;
		if (!is_local && (seat.IP == 0 || seat.Port < 1 || seat.Port > 65535)) {
			fault = "The address of " + name + " is unknown.";
			return(false);
		}
	}

	if (!host) {
		fault = "The host is not among the players.";
		return(false);
	}
	if (!local) {
		fault = "This machine's player is not among the players.";
		return(false);
	}
	return(true);
}


/// <summary>
/// Returns the launch file text that starts the shared campaign at this machine, or an empty
/// string when the lobby state cannot be launched. Every seat gets a color in seat order so
/// the launch file's color sort keeps that order on every machine.
/// </summary>
std::string Spawn_INI(Launch const & launch)
{
	std::string fault;
	if (!Is_Launchable(launch, fault)) {
		return(std::string());
	}

	std::vector<Seat> const order = Seat_Order(launch);
	bool const is_host = Same_Name(launch.Local, launch.Host);

	std::string out = "[Settings]\n";
	for (std::size_t index = 0; index < order.size(); index++) {
		if (!Same_Name(order[index].Name, launch.Local)) {
			continue;
		}
		Put(out, "Name", Seat_Name(order[index].Name));
		Put(out, "Side", 0);
		Put(out, "Color", (int)index);
	}
	Put(out, "IsSinglePlayer", "yes");
	Put(out, "CampaignID", launch.Campaign);
	Put(out, "Scenario", launch.Scenario);
	if (launch.Stage >= 0) {
		Put(out, "CampaignStage", launch.Stage);
	}
	Put(out, "DifficultyModeHuman", launch.Difficulty);
	Put(out, "DifficultyModeComputer", DIFFICULTY_COUNT - 1 - launch.Difficulty);
	Put(out, "GameSpeed", launch.GameSpeed);
	Put(out, "Seed", launch.Seed);
	Put(out, "Firestorm", launch.Firestorm ? "yes" : "no");
	if (launch.ConnTimeout > 0) {
		Put(out, "ConnTimeout", launch.ConnTimeout);
	}
	Put(out, "Port", launch.ListenPort);
	Put(out, "Host", is_host ? "yes" : "no");

	int other = 1;
	for (std::size_t index = 0; index < order.size(); index++) {
		Seat const & seat = order[index];
		if (Same_Name(seat.Name, launch.Local)) {
			continue;
		}
		out += "\n[Other" + std::to_string(other++) + "]\n";
		Put(out, "Name", Seat_Name(seat.Name));
		Put(out, "Side", 0);
		Put(out, "Color", (int)index);
		Put(out, "Ip", Dotted(seat.IP));
		Put(out, "Port", seat.Port);
	}

	return(out);
}

}
