#include <iostream>
#include <vector>
#include <unistd.h>
using namespace std;

constexpr size_t LEN = 4;
constexpr size_t CUBES = LEN * LEN * LEN;
constexpr size_t SPACE_WIDTH = LEN * 2 - 1; // Enough to contain any 4x4x4 with a piece in the center

// This encodes the length of each straight section of cubes.
const vector<int> LENGTHS = {3, 1, 2, 1, 1, 3, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1, 3, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1};

enum Direction {
	plusX = 0,
	negX,
	plusY,
	negY,
	plusZ,
	negZ
};

const char* directionString[] = {
	"right",
	"left",
	"forward",
	"backward",
	"up",
	"down"
};

// Possible next directions from a current direction
Direction turns[6][4] = {
	{plusY, negY, plusZ, negZ}, // plusX
	{plusY, negY, plusZ, negZ}, // negX
	{plusX, negX, plusZ, negZ}, // plusY
	{plusX, negX, plusZ, negZ}, // negY
	{plusX, negX, plusY, negY}, // plusZ
	{plusX, negX, plusY, negY}, // negZ
};

struct Pos {
	int x, y, z;
};

struct Cube {
	bool occupied[SPACE_WIDTH][SPACE_WIDTH][SPACE_WIDTH]; // 3D Array representing whether the space is occupied (to check for collisions). Starts in the center.
	Pos minP, maxP; // Cache the bounding box to make it faster to calculate
	Pos p; // Current head
	Direction d; // Current direction
	int ind; // Current index into LENGTHS
	Direction path[CUBES]; // Only used for reading out solution
};

Pos move(Pos p, Direction d) {
	switch (d) {
	case plusX:
		return {p.x + 1, p.y, p.z};
	case negX:
		return {p.x - 1, p.y, p.z};
	case plusY:
		return {p.x, p.y + 1, p.z};
	case negY:
		return {p.x, p.y - 1, p.z};
	case plusZ:
		return {p.x, p.y, p.z + 1};
	case negZ:
		return {p.x, p.y, p.z - 1};
	}
}

void expandBoundingBox(Pos* minP, Pos* maxP, Pos p) {
	minP->x = min(minP->x, p.x);
	minP->y = min(minP->y, p.y);
	minP->z = min(minP->z, p.z);
	maxP->x = max(maxP->x, p.x);
	maxP->y = max(maxP->y, p.y);
	maxP->z = max(maxP->z, p.z);
}

bool validMove(const Cube& cube, Direction d) {
	Pos newP = move(cube.p, d);
	if (newP.x < 0 || newP.x >= SPACE_WIDTH
	 || newP.y < 0 || newP.y >= SPACE_WIDTH
 	 || newP.z < 0 || newP.z >= SPACE_WIDTH) {
		return false;
	}

	if (cube.occupied[newP.x][newP.y][newP.z]) {
		return false;
	}

	Pos minP = cube.minP;
	Pos maxP = cube.maxP;
	expandBoundingBox(&minP, &maxP, newP);
	if (maxP.x - minP.x >= LEN
	 || maxP.y - minP.y >= LEN
 	 || maxP.z - minP.z >= LEN) {
		return false;
	}

	return true;
}

bool solve(Cube* input) {
	// Make a copy of the current cube so that we can iterate on it
	Cube temp = *input;

	// Move in current direction
	for (int i = 0; i < LENGTHS[temp.ind]; i++) {
		if (validMove(temp, temp.d)) {
			temp.p = move(temp.p, temp.d);
			temp.occupied[temp.p.x][temp.p.y][temp.p.z] = true;
			expandBoundingBox(&temp.minP, &temp.maxP, temp.p);
		} else {
			return false;
		}
	}
	temp.ind++;

	if (temp.ind == LENGTHS.size()) {
		return true;
	}

	// Try out next turns
	for (Direction d : turns[temp.d]) {
		temp.d = d;
		temp.path[temp.ind] = d;
		if (solve(&temp)) {
			*input = temp;
			return true;
		}
	}

	return false;
}

int main(int argc, char* argv[]) {
	bool verbose = false;
	int opt;
	while ((opt = getopt(argc, argv, "v")) != -1) {
		switch (opt) {
			case 'v':
				// Used for passing into render.py
				verbose = true;
				break;
			default:
				cout << "Unknown arg" << endl;
				return 1;
		}
	}

	Cube cube = {};
	cube.p = {LEN - 1, LEN - 1, LEN - 1}; // Center of the cube
	cube.p = move(cube.p, negX); // Go back one cube to account for the first move  (a plusX)
	cube.minP = {SPACE_WIDTH, SPACE_WIDTH, SPACE_WIDTH};

	if (!solve(&cube)) {
		return 1;
	}

	if (verbose) {
		// Emulate the starting position but shift it so that the bottom corner is at (0,0,0)
		Pos p = move(Pos{LEN - 1, LEN - 1, LEN - 1}, negX);
		p.x -= cube.minP.x;
		p.y -= cube.minP.y;
		p.z -= cube.minP.z;

		for (int i = 0; i <= cube.ind; i++) {
			for (int j = 0; j < LENGTHS[i]; j++) {
				p = move(p, cube.path[i]);
				cout << p.x << " " << p.y << " " << p.z << " ";
			}
			cout << endl;
		}
	} else {
		for (int i = 0; i < cube.ind; i++) {
			cout << "Segment " << i + 1 << " with " << LENGTHS[i] << " piece(s) points " << directionString[cube.path[i]] << endl;
		}
	}
}
