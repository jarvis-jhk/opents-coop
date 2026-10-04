/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <string>
#include <vector>


// Turns a campaign chosen in the local network lobby into the launch file a shared campaign
// starts from. Every machine writes its own file from the same lobby state, and the files
// describe one match.
namespace LobbyCampaign {

struct Seat
{
	std::string Name;

	// The machine's address in network byte order and its port in host order; both are
	// ignored for this machine's own seat.
	std::uint32_t IP = 0;
	int Port = 0;
};


struct Launch
{
	int Campaign = -1;
	int Difficulty = 1;
	int Stage = -1;
	bool Firestorm = false;
	std::string Scenario;

	int Seed = 0;
	int GameSpeed = 0;
	int ListenPort = 0;
	int ConnTimeout = 0;

	std::string Host;
	std::string Local;
	std::vector<Seat> Seats;
};


std::string Seat_Name(std::string const & name);

std::vector<Seat> Seat_Order(Launch const & launch);

bool Is_Launchable(Launch const & launch, std::string & fault);

std::string Spawn_INI(Launch const & launch);

}
