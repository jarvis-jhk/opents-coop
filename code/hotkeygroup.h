/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

class TechnoClass;


int Hotkey_Group(TechnoClass const * object);
void Set_Hotkey_Group(TechnoClass * object, int group);
void Clear_Hotkey_Groups(void);
