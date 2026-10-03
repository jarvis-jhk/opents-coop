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
#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


struct UIForcesVariant
{
	std::string Label;
	std::vector<std::string> Forces;
};


struct UIForcesColumn
{
	std::string Name;
};


struct UIForcesRow
{
	std::string Name;
	std::vector<int> Cells;
};


struct UIForcesState
{
	std::string Mission;
	std::vector<UIForcesVariant> Variants;
	std::vector<std::string> Players;
	int Selected = 0;
	std::vector<UIForcesColumn> Columns;
	std::vector<UIForcesRow> Rows;
	bool Valid = true;
	ForceControl::Matrix Control{};
};


// Chooses the version of a mission to play and which player controls which of its forces.
class UIForcesPresenterClass : public UIPresenterClass
{
	public:
		explicit UIForcesPresenterClass(UIForcesState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIForcesState State;

	private:
		void Select(int variant);
		void Show_Control(void);
};


std::unique_ptr<UIViewClass> UI_Forces_View(UIForcesPresenterClass & presenter);

bool UI_Forces_Dialog(UIForcesState & state);
