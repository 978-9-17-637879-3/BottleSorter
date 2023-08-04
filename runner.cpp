#include <vector>
#include "greedy.hpp"

int main() {
    /*
tubeCount = document.querySelectorAll('rect').length
    tubeHeight = Number(document.querySelector('#tubeHeight').value);
    circles = Array.from(document.querySelectorAll('circle')).sort((a,b) => {
		aNumber = Number(a.id.split('_')[1]*10) + Number(a.id.split('_')[2])
		bNumber = Number(b.id.split('_')[1]*10) + Number(b.id.split('_')[2])
		return aNumber - bNumber;
	})
	console.log(circles)
    colors = {
        ball1: "yellow",
        ball2: "green",
        ball3: "blue",
        ball4: "red",
        ball5: "grey",
        ball6: "orange",
        ball7: "violet",
        ball8: "lime",
        ball9: "maroon",
        ball10: "navy",
        ball11: "cyan",
        ball12: "black",
        ball13: "olive"
    }
    lines = []
    for (let i = 0; i < tubeCount; i++) {
        const tubeCircles = circles.filter(circle => circle.id.startsWith(`ball_${i}_`))
        const tubeCircleColors = tubeCircles.map(circle => colors[circle.classList[0]])
        while (tubeCircleColors.length < tubeHeight) {
            tubeCircleColors.push("blank");
        }
        let line = 'bottles.push_back(Bottle{{'
        for (let j = 0; j < tubeHeight - 1; j++) {
            line += tubeCircleColors[j];
            line += ', ';
        }
        line += `${tubeCircleColors[tubeHeight - 1]}}});`;
        lines.push(line);
    }
    console.log(lines.join('\n'));
     */


    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{green, green, black, lime}});
    bottles.push_back(Bottle{{red, red, grey, maroon}});
    bottles.push_back(Bottle{{lime, lime, olive, navy}});
    bottles.push_back(Bottle{{green, orange, navy, maroon}});
    bottles.push_back(Bottle{{navy, yellow, black, yellow}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{maroon, maroon, orange, olive}});
    bottles.push_back(Bottle{{yellow, cyan, violet, grey}});
    bottles.push_back(Bottle{{blue, blue, red, green}});
    bottles.push_back(Bottle{{orange, yellow, navy, violet}});
    bottles.push_back(Bottle{{violet, olive, orange, violet}});
    bottles.push_back(Bottle{{blue, red, lime, blue}});
    bottles.push_back(Bottle{{grey, black, cyan, black}});
    bottles.push_back(Bottle{{olive, cyan, grey, cyan}});

    if (bottles.size() > 255) {
        std::cout << "Bottle count must be <=255" << std::endl;
        return 0;
    }

    // Calculate all permutations bottles size 2 (used for finding possible moves)
    char bottleCount = bottles.size();
    // the size of indicesPermutation is nPr(bottlesCount, 2) which is equivalent to bottleCount * (bottleCount-1)
    int indicesPermutationsCount = bottleCount * (bottleCount - 1);

    Move indicesPermutations[indicesPermutationsCount];
    int insertionPoint = 0;
    for (char i = 0; i < bottleCount; i++) {
        for (char j = 0; j < bottleCount; j++) {
            if (i == j) continue;
            indicesPermutations[insertionPoint] = (Move{i, j});
            insertionPoint++;
        }
    }

    // Solve
    auto start = std::chrono::high_resolution_clock::now();
    const std::vector<Move> &solution = runGreedyBacktracker(bottles, indicesPermutations, indicesPermutationsCount);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if (LOG) std::cout << duration.count() << std::endl;
    std::cout << "SOLUTION FOUND " << std::endl;
    printSequence(solution);
    if (LOG) std::cout << solution.size() << " moves" << std::endl;


    return 0;
}
