/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/forces/uiforces.h"

#include "ui/rml/rmlview.h"

#include <RmlUi/Core/ElementDocument.h>
#include <utility>


UIForcesPresenterClass::UIForcesPresenterClass(UIForcesState state) :
	State(std::move(state))
{
	Select(State.Selected);
}


/// <summary>
/// Shows one version and presets its control matrix, which a different version replaces.
/// </summary>
void UIForcesPresenterClass::Select(int variant)
{
	if (variant < 0 || variant >= (int)State.Variants.size()) {
		return;
	}
	State.Selected = variant;

	int const forces = (int)State.Variants[variant].Forces.size();
	State.Control = ForceControl::Default(forces, (int)State.Players.size());

	State.Columns.clear();
	for (int force = 0; force < forces; force++) {
		State.Columns.push_back(UIForcesColumn{std::to_string(force + 1) + ": " + State.Variants[variant].Forces[force]});
	}
	Show_Control();
}


void UIForcesPresenterClass::Show_Control(void)
{
	int const forces = (int)State.Columns.size();
	int const seats = (int)State.Players.size();

	State.Rows.clear();
	for (int seat = 0; seat < seats; seat++) {
		UIForcesRow row;
		row.Name = State.Players[seat];
		for (int force = 0; force < forces; force++) {
			row.Cells.push_back(ForceControl::Controls(State.Control, seat, force) ? 1 : 0);
		}
		State.Rows.push_back(row);
	}
	State.Valid = ForceControl::Is_Valid(State.Control, forces, seats);
}


void UIForcesPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "variant") {
		Select(intent.Value);
	} else if (intent.Name == "cell") {
		int const seat = intent.Value / ForceControl::MAX_FORCES;
		int const force = intent.Value % ForceControl::MAX_FORCES;
		if (seat >= 0 && seat < (int)State.Players.size() && force < (int)State.Columns.size()) {
			State.Control[force] ^= (unsigned char)(1u << seat);
			Show_Control();
		}
	} else if (intent.Name == "ok") {
		if (State.Valid) {
			Result = UI_RESULT_ACCEPTED;
		}
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIForcesPresenterClass::Refresh(void)
{
}


namespace
{

class UIForcesViewClass : public UIRmlViewClass
{
	public:
		explicit UIForcesViewClass(UIForcesPresenterClass & presenter) :
			UIRmlViewClass(presenter, "forces.rml", "forces"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("selected");
			Model.DirtyVariable("columns");
			Model.DirtyVariable("rows");
			Model.DirtyVariable("valid");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIForcesVariant> variant = model.RegisterStruct<UIForcesVariant>();
			Rml::StructHandle<UIForcesColumn> column = model.RegisterStruct<UIForcesColumn>();
			if (!variant || !column || !model.RegisterArray<std::vector<int>>() || !model.RegisterArray<std::vector<std::string>>()) {
				return(false);
			}
			Rml::StructHandle<UIForcesRow> row = model.RegisterStruct<UIForcesRow>();
			if (!row) {
				return(false);
			}
			variant.RegisterMember("label", &UIForcesVariant::Label);
			variant.RegisterMember("forces", &UIForcesVariant::Forces);
			column.RegisterMember("name", &UIForcesColumn::Name);
			row.RegisterMember("name", &UIForcesRow::Name);
			row.RegisterMember("cells", &UIForcesRow::Cells);

			UIForcesState & state = Data.State;
			return(model.RegisterArray<std::vector<UIForcesVariant>>()
				&& model.RegisterArray<std::vector<UIForcesColumn>>()
				&& model.RegisterArray<std::vector<UIForcesRow>>()
				&& model.Bind("mission", &state.Mission)
				&& model.Bind("variants", &state.Variants)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("columns", &state.Columns)
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("valid", &state.Valid));
		}

	private:
		UIForcesPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Forces_View(UIForcesPresenterClass & presenter)
{
	return(std::make_unique<UIForcesViewClass>(presenter));
}
