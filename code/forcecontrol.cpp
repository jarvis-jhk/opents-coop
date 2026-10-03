/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "forcecontrol.h"

#include <algorithm>
#include <cstdlib>


namespace
{
	std::size_t File_Start(std::string const & file)
	{
		std::size_t const slash = file.find_last_of("/\\");
		return(slash == std::string::npos ? 0 : slash + 1);
	}
}


namespace ForceControl
{

/// <summary>
/// Returns the preset control: player 1 controls force 1, player 2 force 2, and so on. With
/// more players than forces the count wraps, so several players share a force; with more
/// forces than players it wraps the other way, so a player controls several forces.
/// </summary>
Matrix Default(int forces, int seats)
{
	Matrix matrix{};
	forces = std::clamp(forces, 0, MAX_FORCES);
	seats = std::clamp(seats, 0, MAX_SEATS);
	if (forces == 0 || seats == 0) {
		return(matrix);
	}

	for (int index = 0; index < std::max(forces, seats); index++) {
		matrix[index % forces] |= (unsigned char)(1u << (index % seats));
	}
	return(matrix);
}


/// <summary>
/// Can a mission be played under this control? Every force needs a player and every player
/// a force, and no bit may name a seat or force outside the counts.
/// </summary>
bool Is_Valid(Matrix const & matrix, int forces, int seats)
{
	if (forces < 1 || forces > MAX_FORCES || seats < 1 || seats > MAX_SEATS) {
		return(false);
	}

	unsigned const seat_mask = (1u << seats) - 1u;
	unsigned seen = 0;
	for (int force = 0; force < MAX_FORCES; force++) {
		if (force >= forces) {
			if (matrix[force] != 0) {
				return(false);
			}
			continue;
		}
		if (matrix[force] == 0 || (matrix[force] & ~seat_mask) != 0) {
			return(false);
		}
		seen |= matrix[force];
	}
	return(seen == seat_mask);
}


bool Controls(Matrix const & matrix, int seat, int force)
{
	if (seat < 0 || seat >= MAX_SEATS || force < 0 || force >= MAX_FORCES) {
		return(false);
	}
	return((matrix[force] & (1u << seat)) != 0);
}


/// <summary>
/// Returns the lowest numbered force a seat controls, or -1 for none.
/// </summary>
int First_Force(Matrix const & matrix, int seat, int forces)
{
	for (int force = 0; force < std::min(forces, MAX_FORCES); force++) {
		if (Controls(matrix, seat, force)) {
			return(force);
		}
	}
	return(-1);
}


/// <summary>
/// Returns the stock mission a file belongs to. A variant is named after its stock mission with
/// a tag before the extension: GDI1A.coop2.map is a variant of GDI1A.map. Any other name is
/// returned unchanged.
/// </summary>
std::string Base_Name(std::string const & file)
{
	std::size_t const start = File_Start(file);
	std::size_t const first = file.find('.', start);
	std::size_t const last = file.rfind('.');
	if (first == std::string::npos || first == last || first == start) {
		return(file);
	}
	return(file.substr(0, first) + file.substr(last));
}


/// <summary>
/// Returns the wildcard pattern that matches every variant of a stock mission, directory
/// included.
/// </summary>
std::string Variant_Pattern(std::string const & base)
{
	std::size_t const start = File_Start(base);
	std::size_t const dot = base.find('.', start);
	if (dot == std::string::npos || dot == start) {
		return(base + ".*");
	}
	return(base.substr(0, dot) + ".*" + base.substr(dot));
}


/// <summary>
/// Returns the tag that names a variant, empty for a stock mission.
/// </summary>
std::string Variant_Tag(std::string const & file)
{
	std::size_t const start = File_Start(file);
	std::size_t const first = file.find('.', start);
	std::size_t const last = file.rfind('.');
	if (first == std::string::npos || first == last || first == start) {
		return(std::string());
	}
	return(file.substr(first + 1, last - first - 1));
}


/// <summary>
/// Picks the variant whose force count matches the player count best: an exact match, else
/// the nearest count, preferring more forces than players over fewer, so each player keeps a
/// force of their own. Among equals the earliest listed wins. Returns -1 for an empty list.
/// </summary>
int Best_Variant(std::vector<int> const & force_counts, int seats)
{
	int best = -1;
	for (int index = 0; index < (int)force_counts.size(); index++) {
		if (best < 0) {
			best = index;
			continue;
		}
		int const distance = std::abs(force_counts[index] - seats);
		int const best_distance = std::abs(force_counts[best] - seats);
		if (distance < best_distance || (distance == best_distance && force_counts[index] > force_counts[best])) {
			best = index;
		}
	}
	return(best);
}

}
