/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/screens/forces/uiforces.h"
#include "ui/uienginehost.h"
#include "ui/uiview.h"

#include <utility>


/// <summary>
/// Shows the version and control chooser. On acceptance the state holds the chosen version
/// and a valid control matrix for it; returns false when the player cancelled.
/// </summary>
bool UI_Forces_Dialog(UIForcesState & state)
{
	UIForcesPresenterClass presenter(std::move(state));
	std::unique_ptr<UIViewClass> view = UI_Forces_View(presenter);

	UIResult const result = UI_Run_Modal(*view);
	state = std::move(presenter.State);
	return(result == UI_RESULT_ACCEPTED);
}
