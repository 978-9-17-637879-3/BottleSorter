const robot = require('robotjs');
const fs = require('fs');

const bottleToPx = {
    0: {x: 360, y: 455},
    1: {x: 460, y: 455},
    2: {x: 560, y: 455},
    3: {x: 660, y: 455},
    4: {x: 760, y: 455},
    5: {x: 408, y: 677},
    6: {x: 508, y: 677},
    7: {x: 608, y: 677},
    8: {x: 708, y: 677},
}

const wait = (timeMS) => new Promise(resolve => setTimeout(resolve, timeMS));

robot.setMouseDelay(2);

let sol = fs
    .readFileSync('./sol.txt')
    .toString()
    .trim()
    .split('SHORTEST SOLUTION FOUND')[1]
    .split('\n')
    .map(s => s.trim());
sol = sol.slice(1,sol.length-2);
console.log(sol);
(async function () {
    for (const move of sol) {
        console.log(move)
        const from = Number(move.split(' ')[0]);
        const to = Number(move.split(' ')[1]);
        const pxFrom = bottleToPx[from];
        const pxTo = bottleToPx[to];
        console.log(pxFrom);
        console.log(pxTo);
        robot.moveMouse(pxFrom.x, pxFrom.y);
        robot.mouseClick('left', false);
        await wait(500);
        robot.moveMouse(pxTo.x, pxTo.y);
        robot.mouseClick('left', false);
        await wait(500);
    }
})()
