/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the preset control matrix, its validity rules, the variant file naming and the
// variant preselection, with no engine and no game data.

#include <cstdio>
#include <initializer_list>

#include "forcecontrol.h"

namespace {

int Failures = 0;


void Check(bool condition, char const * what)
{
	std::printf("%-64s %s\n", what, condition ? "ok" : "FAILED");

	if (!condition) {
		Failures++;
	}
}


ForceControl::Matrix Make(std::initializer_list<unsigned char> forces)
{
	ForceControl::Matrix matrix{};
	int index = 0;
	for (unsigned char seats : forces) {
		matrix[index++] = seats;
	}
	return(matrix);
}

}


int main()
{
	using namespace ForceControl;

	Check(Default(1, 1) == Make({0x01}), "one player controls the only force");
	Check(Default(2, 2) == Make({0x01, 0x02}), "two players control a force each");
	Check(Default(1, 3) == Make({0x07}), "three players share one force");
	Check(Default(2, 3) == Make({0x05, 0x02}), "the third player wraps to force 1");
	Check(Default(3, 1) == Make({0x01, 0x01, 0x01}), "one player controls three forces");
	Check(Default(3, 2) == Make({0x01, 0x02, 0x01}), "force 3 wraps to player 1");
	Check(Default(0, 2) == Make({}), "no forces leaves the matrix empty");

	for (int forces = 1; forces <= MAX_FORCES; forces++) {
		for (int seats = 1; seats <= MAX_SEATS; seats++) {
			if (!Is_Valid(Default(forces, seats), forces, seats)) {
				std::printf("preset %d forces x %d seats is invalid\n", forces, seats);
				Failures++;
			}
		}
	}
	Check(true, "every preset is valid");

	Check(!Is_Valid(Make({0x01, 0x00}), 2, 1), "a force without a player is refused");
	Check(!Is_Valid(Make({0x01}), 1, 2), "a player without a force is refused");
	Check(!Is_Valid(Make({0x05}), 1, 2), "a seat beyond the player count is refused");
	Check(!Is_Valid(Make({0x01, 0x01}), 1, 1), "a force beyond the force count is refused");
	Check(Is_Valid(Make({0x03, 0x02}), 2, 2), "a force may have several players");

	Check(Controls(Make({0x01, 0x02}), 1, 1), "seat 2 controls force 2");
	Check(!Controls(Make({0x01, 0x02}), 0, 1), "seat 1 does not control force 2");
	Check(First_Force(Make({0x01, 0x02, 0x02}), 1, 3) == 1, "seat 2 first controls force 2");
	Check(First_Force(Make({0x01}), 1, 1) == -1, "a seat with no force has none");

	Check(Base_Name("GDI1A.MAP") == "GDI1A.MAP", "a stock name is its own base");
	Check(Base_Name("GDI1A.coop2.map") == "GDI1A.map", "a variant names its stock mission");
	Check(Base_Name("Maps/Missions/gdi1a.coop-hard.map") == "Maps/Missions/gdi1a.map", "a variant keeps its directory");
	Check(Base_Name("Maps/my.dir/gdi1a.map") == "Maps/my.dir/gdi1a.map", "a dot in a directory is not a tag");
	Check(Variant_Pattern("Maps/Missions/GDI1A.MAP") == "Maps/Missions/GDI1A.*.MAP", "the variant pattern keeps the directory");
	Check(Variant_Tag("GDI1A.coop2.map") == "coop2", "the tag sits between the dots");
	Check(Variant_Tag("GDI1A.map").empty(), "a stock mission has no tag");

	Check(Best_Variant({1, 2, 4}, 1) == 0, "one player gets the stock mission");
	Check(Best_Variant({1, 2, 4}, 2) == 1, "two players get the two-force variant");
	Check(Best_Variant({1, 2, 4}, 3) == 2, "three players prefer four forces over two");
	Check(Best_Variant({1, 2, 2}, 2) == 1, "the earliest of two equal variants wins");
	Check(Best_Variant({1}, 4) == 0, "a lone stock mission is always chosen");
	Check(Best_Variant({}, 2) == -1, "no variants chooses nothing");

	if (Failures != 0) {
		std::printf("%d check(s) failed\n", Failures);
		return(1);
	}
	return(0);
}
