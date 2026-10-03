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

struct GlobalPacketType;


bool Shared_Campaign_Next(bool won);
void Shared_Campaign_Receive(GlobalPacketType const & packet);
bool Shared_Campaign_Setup(std::string & scenario, ForceControl::Matrix & control);
