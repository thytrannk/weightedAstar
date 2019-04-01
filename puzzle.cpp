//
// Created by Katie Tran on 2019-01-13.
//

#include "puzzle.h"
#include "puzzlePDB.h"
#include "util.h"
#include <iostream>

using namespace std;

StatePuzzle::StatePuzzle(int *state) {
    stateID = hash(state);
}

StatePuzzle::StatePuzzle(int id) {
    stateID = id;
}

int StatePuzzle::getValue(int location) {
    int ret;
    ret = (int) (this->stateID >> (4 * (15 - location))) & 15;
    return ret;
}

uint64_t StatePuzzle::hash(int *state) {
    // among 64 bits, every 4 bits represents a number on a tile from 0 to 15
    uint64_t res = 0;
    for (int i = 0; i < 16; i++) {
        res |= (uint64_t) state[i] << (4 * (15 - i));
    }
    return res;
}

uint64_t StatePuzzle::getStateID() const {
    return this->stateID;
}

int StatePuzzle::getLocation(int val) {
    int loc;
    for (loc = 0; loc < 16; loc++) {
        if (getValue(loc) == val) {
            return loc;
        }
    }
    cerr << "Invalid location." << endl;
    exit(EXIT_FAILURE);
}

void StatePuzzle::display() {
    for (int i = 0; i < 16; i++) {
        cout << getValue(i) << "   ";
    }
    cout << endl;
}

void StatePuzzle::assignValue(int val, int location) {
    // clearing that location
    this->stateID &= ~(1ULL << (4 * (15 - location)));
    this->stateID &= ~(1ULL << (4 * (15 - location) + 1));
    this->stateID &= ~(1ULL << (4 * (15 - location) + 2));
    this->stateID &= ~(1ULL << (4 * (15 - location) + 3));
    // setting bits
    this->stateID += (uint64_t) val << (4 * (15 - location));
}

void StatePuzzle::swap(int location1, int location2) {
    int val1 = getValue(location1);
    int val2 = getValue(location2);
    assignValue(val1, location2);
    assignValue(val2, location1);
}

bool operator==(const StatePuzzle &lhs, const StatePuzzle &rhs) {
    return lhs.getStateID() == rhs.getStateID();
}

bool operator<(const StatePuzzle &lhs, const StatePuzzle &rhs) {
    return lhs.getStateID() < rhs.getStateID();
}

bool operator>(const StatePuzzle &lhs, const StatePuzzle &rhs) {
    return lhs.getStateID() > rhs.getStateID();
}

size_t hash<StatePuzzle>::operator()(StatePuzzle const &s) const noexcept {
    hash<uint64_t> a;
    return a(s.getStateID());
}

ActionPuzzle::ActionPuzzle(char dir) {
    slideDir = dir;
}

char ActionPuzzle::getAction() {
    return this->slideDir;
}

HeuristicPuzzle::HeuristicPuzzle(vector<pair<int, vector<uint8_t> *>> *list) {
    hList = list;
}

double HeuristicPuzzle::hCost(StatePuzzle &node1, StatePuzzle &node2) {
    double ret = 0;
    vector<int8_t> stateArr(16);
    for (int i = 0; i < 16; i++) {
        stateArr[i] = (int8_t) node1.getValue(i);
    }
    for (auto &h : *hList) {
        if (h.first == 0) {
            // Manhattan distance
            double md = 0;
            for (int i = 0; i < 16; i++) {
                if (stateArr[i] != 0) {
                    // if this is not a blank
                    md += ManhattanDist(i, node2.getLocation((int) stateArr[i]));
                }
            }
            if (md > ret) {
                ret = md;
            }
        } else {
            // PDB
            vector<int8_t> pattern;
            if (h.first == 1) {
                pattern = {0, 1, 2, 3, 4, 5, 6, 7};
            } else if (h.first == 2) {
                pattern = {0, 8, 9, 12, 13};
            } else if (h.first == 3) {
                pattern = {0, 10, 11, 14, 15};
            } else {
                cerr << "Invalid pattern database." << endl;
                exit(EXIT_FAILURE);
            }
            vector<int8_t> stateArr(16);
            for (int i = 0; i < 16; i++) {
                stateArr[i] = (int8_t) node1.getValue(i);
            }
            StatePuzzleLexi s(stateArr, &pattern);
            double cost = (*(h.second))[s.getStateID()];
            if (cost > ret) {
                ret = cost;
            }
        }
    }
    return ret;
}

bool HeuristicPuzzle::isConsistent() {
    return true;
}
