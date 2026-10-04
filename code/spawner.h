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


void Spawner_Request(void);
bool Spawner_Is_Requested(void);
bool Spawner_Is_Active(void);
bool Spawner_Prepare(bool & gameloaded);
bool Spawner_Launch_From_Lobby(std::string const & text, bool & gameloaded);
int Spawner_Session_Identity(void);
void Spawner_Announce_Master(void);
void Spawner_Apply_Campaign_State(void);
bool Spawner_Continue_Shared_Campaign(char const * scenario, int stage, bool advance);
