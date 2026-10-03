/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include <array>
#include <string>
#include <vector>


/*
 * A campaign mission names the houses people play as its forces. Which player controls which
 * force is a matrix with one byte per force and one bit per player seat.
 */
namespace ForceControl
{
	constexpr int MAX_FORCES = 8;
	constexpr int MAX_SEATS = 8;

	using Matrix = std::array<unsigned char, MAX_FORCES>;

	Matrix Default(int forces, int seats);
	bool Is_Valid(Matrix const & matrix, int forces, int seats);
	bool Controls(Matrix const & matrix, int seat, int force);
	int First_Force(Matrix const & matrix, int seat, int forces);

	std::string Base_Name(std::string const & file);
	std::string Variant_Pattern(std::string const & base);
	std::string Variant_Tag(std::string const & file);
	int Best_Variant(std::vector<int> const & force_counts, int seats);
}
