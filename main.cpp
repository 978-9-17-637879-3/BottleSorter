#include <bits/stdc++.h>
#include <vector>

const int COLORS_PER_BOTTLE = 4;
const bool IS_BALL = true; // if they are balls instead of liquid, the pouring mechanism is different
const bool FIND_SHORTEST = true;
const int MAXIMUM_DEPTH = 40;

enum Color {
    blank, yellow, green, blue, red, grey, orange, violet, lime, maroon, navy, cyan, black, olive
};

struct Bottle {
    Color colors[COLORS_PER_BOTTLE];
};

struct Move {
    int fromID;
    int toID;
};

struct FindResult {
    std::optional<std::vector<Move>> sequence;
    std::optional<Move> lastMove;
};

void printMoves(const std::vector<Move> &moves) {
    for (const Move &move: moves) {
        std::cout << move.fromID << " " << move.toID;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printSequence(const std::vector<Move> &moves) {
    if (moves.empty()) return;
    for (long i = 0; i < moves.size()-1; i++) {
        std::cout << moves[i].fromID << " " << moves[i].toID;
        std::cout << "->";
    }
    std::cout << moves.back().fromID << " " << moves.back().toID;
    std::cout << std::endl;
}

void printBottles(const std::vector<Bottle> &bottles) {
    std::cout << "Bottles: " << std::endl;
    for (int i = 0; i < bottles.size(); i++) {
        Bottle bottle = bottles[i];
        printf("%u: %u, %u, %u, %u\n", i, bottle.colors[0], bottle.colors[1], bottle.colors[2], bottle.colors[3]);
    }
}

bool vectorContains(std::vector<Move> vector, Move moveSearchTarget) {
    for (const Move &move: vector) {
        if (move.fromID == moveSearchTarget.fromID && move.toID == moveSearchTarget.toID)
            return true;
    }
    return false;
}

int getTopColorOfBottleBottomIndex(Bottle bottle) {
    int firstColorIndex = -1;
    std::optional<Color> firstColor;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            if (firstColor.has_value() && firstColor.value() != bottle.colors[i]) {
                break;
            }
            firstColorIndex = i;
            firstColor = bottle.colors[i];
        }
    }
    return firstColorIndex;
}

int getTopColorOfBottleTopIndex(Bottle bottle) {
    int firstColorIndex = -1;
    std::optional<Color> firstColor;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            firstColorIndex = i;
            break;
        }
    }
    return firstColorIndex;
}

std::optional<Color> getFirstColorOfBottle(Bottle bottle) {
    std::optional<Color> firstColor;

    int firstColorOfBottleIndex = getTopColorOfBottleBottomIndex(bottle);
    if (firstColorOfBottleIndex != -1)
        firstColor = bottle.colors[firstColorOfBottleIndex];

    return firstColor;
}

std::vector<Move> getPossibleMoves(const std::vector<Bottle> &bottles, const std::vector<Move> &indicesPermutations) {
    std::vector<Move> movePermutations = indicesPermutations;

    for (int i = movePermutations.size() - 1; i >= 0; i--) {
        Move movePerm = movePermutations[i];
        Bottle from = bottles[movePerm.fromID];
        Bottle to = bottles[movePerm.toID];

        std::optional<Color> firstColorOfFromBottle = getFirstColorOfBottle(from);
        std::optional<Color> firstColorOfToBottle = getFirstColorOfBottle(to);

        // from bottle is complete; this may need to be removed...
        if (firstColorOfFromBottle.has_value() &&
            std::all_of(std::begin(from.colors), std::end(from.colors), [&](Color c) -> bool {
                return firstColorOfFromBottle.value() == c;
            })) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // from bottle is empty;
        if (from.colors[0] == blank) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // to bottle is full
        if (to.colors[COLORS_PER_BOTTLE - 1] != blank) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // from bottle top is not the same as to bottle top (to bottle top has to have liquid for this to trigger
        if (firstColorOfToBottle.has_value() && firstColorOfFromBottle.value() != firstColorOfToBottle.value()) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }
    }

    return movePermutations;
}

void transferLiquid(Bottle *from, Bottle *to) {
    std::optional<Color> toTransfer = getFirstColorOfBottle(*from);

    int topColorOfToBottleTopIndex = getTopColorOfBottleTopIndex(*to);

    int amountToTransfer = 0;
    for (int i = getTopColorOfBottleTopIndex(*from); i >= 0; i--) {
        if (from->colors[i] == toTransfer.value() &&
            (IS_BALL ?
             amountToTransfer < 1 :
             (amountToTransfer + topColorOfToBottleTopIndex < COLORS_PER_BOTTLE - 1)
            )) {
            amountToTransfer++;
            (*from).colors[i] = blank;
        } else {
            break;
        }
    }
    for (int i = topColorOfToBottleTopIndex + 1;
         amountToTransfer > 0 && i < COLORS_PER_BOTTLE; i++, amountToTransfer--) {
        (*to).colors[i] = toTransfer.value();
    }
}

// https://stackoverflow.com/a/26958878
template<typename MAP>
const typename MAP::mapped_type &get_with_default(const MAP &m,
                                                  const typename MAP::key_type &key,
                                                  const typename MAP::mapped_type &defval) {
    typename MAP::const_iterator it = m.find(key);
    if (it == m.end())
        return defval;

    return it->second;
}

long scoreGame(const std::vector<Bottle> &bottles) {
    long score = 0;
    bool gameFinished = true;
    for (const Bottle &bottle: bottles) {
        std::map<Color, int> colorCountMap;
        bool allColorSame = true;
        for (const Color &color: bottle.colors) {
            colorCountMap[color] = get_with_default(colorCountMap, color, 0) + 1;
            if (color != bottle.colors[0]) {
                allColorSame = false;
            }
        }

        if (!allColorSame) gameFinished = false;

        // Bonus points if vial is solved (empty does not count as solved)
        if (allColorSame && bottle.colors[0] != blank) score += 10;
    }
    if (gameFinished) score = 10000000;
    return score;
}


bool
sequenceVectorContainsSequence(const std::vector<std::vector<Move>> &sequenceList, const std::vector<Move> &sequence) {
    for (const std::vector<Move> &s : sequenceList) {
        if (s.size() != sequence.size())
            continue;
        for (long i = 0; i < s.size(); i++) {
            if (s[i].fromID == sequence[i].fromID && s[i].toID == sequence[i].toID) {
                return true;
            }
        }
    }
    return false;
}

FindResult
findSolution(const std::vector<Move> &path,
             const std::vector<Bottle> &bottles,
             std::vector<std::vector<Move>> *sequencesSeenPtr,
             std::vector<std::vector<Move>> *solutionsPtr,
             const std::vector<Move> &indicesPermutations,
             int* shortestSolutionLengthPtr,
             int depth = 0) {
    if (depth == MAXIMUM_DEPTH) {
        throw std::invalid_argument("fuck");
    }

    std::vector<Move> possibleMoves = getPossibleMoves(bottles, indicesPermutations);
//    printMoves(possibleMoves);
//    return FindResult{};

    std::cout << "Setup:" <<std::endl;
    printSequence(path);
//    printBottles(bottles);
//    printMoves(possibleMoves);
    for (const Move &newMove: possibleMoves) {
        if (depth == 0) {
            std::cout << "";
        }
        if (!path.empty()) {
            // duplicate move
            if (!IS_BALL) {
                if (path.back().fromID == newMove.fromID && path.back().toID == newMove.toID) {
                    continue;  //kill this branch, we're backtracking
                }
            }
            // backtracked newMove
            if (path.back().fromID == newMove.toID && path.back().toID == newMove.fromID) {
                continue; //kill this branch, we're backtracking
            }
        }
        std::vector<Move> newPath = path;
        if (newPath.size() >= *shortestSolutionLengthPtr) {
            continue;
        }
        // if a ball is being transferred twice in a row, optimize the transfer by eliminating the middleman
//        if (IS_BALL) {
//            if (newPath.size() > 1) {
//                Move lastMove = newPath.back();
//                if (lastMove.toID == newMove.fromID) {
//                    newPath.pop_back();
//                    newPath.push_back(Move{lastMove.fromID, newMove.toID});
//                } else {
//                    newPath.push_back(newMove);
//                }
//            } else {
//                newPath.push_back(newMove);
//            }
//        } else {
            newPath.push_back(newMove);
//        }
//        if (sequenceVectorContainsSequence(*sequencesSeenPtr, newPath)) {
//            continue;
//        }
        sequencesSeenPtr->push_back(newPath);
//        std::cout << "\x1B[2J\x1B[H";
//        std::cout << "Unique Sequences Calculated: " << sequencesSeenPtr->size() << std::endl;
        std::vector<Bottle> newBottles = bottles;
        transferLiquid(&newBottles[newPath.back().fromID], &newBottles[newPath.back().toID]);

        FindResult result;
        result.lastMove = newMove;

        if (scoreGame(newBottles) == 10000000) {
            result.sequence = newPath;
            if (result.sequence.value().size() < *shortestSolutionLengthPtr) {
                *shortestSolutionLengthPtr = newPath.size();
            }
        }

        while (!result.sequence.has_value() && result.lastMove && depth < MAXIMUM_DEPTH-1) {
            result = findSolution(newPath, newBottles, sequencesSeenPtr, solutionsPtr, indicesPermutations, shortestSolutionLengthPtr, depth + 1);
        }

        if (result.sequence.has_value()) {
            if (depth > 0)
                solutionsPtr->push_back(result.sequence.value());
            else {
                continue;
            }
        }
    }

    return FindResult{};
}


int main() {
    /*
    tubeCount = document.querySelectorAll('rect').length
    tubeHeight = Number(document.querySelector('#tubeHeight > option:nth-child(2)').text);
    circles = Array.from(document.querySelectorAll('circle'))
    colors = {ball1: "yellow", ball2: "green", ball3: "blue", ball4: "red", ball5: "grey", ball6: "orange", ball7: "violet", ball8: "lime", ball9: "maroon", ball10: "navy", ball11: "cyan", ball12: "black", ball13: "olive"}
    lines = []
		for (let i = 0; i < tubeCount; i++) {
        const tubeCircles = circles.filter(circle => circle.id.startsWith(`ball_${i}_`)).reverse()
        const tubeCircleColors = tubeCircles.map(circle => colors[circle.classList[0]])
        while (tubeCircleColors.length < tubeHeight) {
            tubeCircleColors.push("blank");
        }
        console.log(tubeCircleColors)
        lines.push(`bottles.push_back(Bottle{{${tubeCircleColors[0]}, ${tubeCircleColors[1]}, ${tubeCircleColors[2]}, ${tubeCircleColors[3]}}});`)
    }
	console.log(lines.join('\n'))
     */


    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{green, red, blank, blank}});
    bottles.push_back(Bottle{{yellow, red, blue, blank}});
    bottles.push_back(Bottle{{blue, red, yellow, green}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{green, yellow, blue, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{blue, green, red, yellow}});

    // Calculate all permutations bottles size 2 (used for finding possible moves)
    int bottleCount = bottles.size();

    std::vector<Move> indicesPermutations;
    for (int i = 0; i < bottleCount; i++) {
        for (int j = 0; j < bottleCount; j++) {
            if (i==j) continue;
            indicesPermutations.push_back(Move{i,j});
        }
    }

    // Solve
    std::vector<std::vector<Move>> sequencesSeen;
    std::vector<std::vector<Move>> solutions;
    std::optional<std::vector<Move>> bestSolution;

    int shortestSolutionLength = MAXIMUM_DEPTH;
    findSolution(std::vector<Move>{}, bottles, &sequencesSeen, &solutions, indicesPermutations, &shortestSolutionLength).sequence;

    std::sort(solutions.begin(), solutions.end(), [](const std::vector<Move> &a, const std::vector<Move> &b) {
        return a.size() < b.size();
    });

    if (solutions.empty()) {
        std::cout << "NO SOLUTIONS" << std::endl;
        std::cout << "Explored " << sequencesSeen.size() << " sequences" << std::endl;
        return 0;
    } else {
        bestSolution = solutions.front();
        if (bestSolution.has_value()) {
            std::cout << "BEST SOLUTION FOUND " << std::endl;
            printMoves(bestSolution.value());
        }
    }

//    if (FIND_SHORTEST) {
//        while (bestSolution.has_value()) {
//            solutions.push_back(bestSolution.value());
//            bestSolution = findSolution(std::vector<Move>{}, bottles, &sequencesSeen, indicesPermutations, &shortestSolutionLength).sequence;
//        }
//
//        std::sort(solutions.begin(), solutions.end(), [](const std::vector<Move> &a, const std::vector<Move> &b) {
//            return a.size() < b.size();
//        });
//
//        std::cout << "SHORTEST SOLUTION FOUND " << std::endl;
//        printMoves(solutions.front());
//    }

    std::cout << "Explored " << sequencesSeen.size() << " move sequences!" << std::endl;

    return 0;
}
