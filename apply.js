const robot = require('robotjs');
const fs = require('fs');

const startX = 151;
const gap = 130;
const bottleCount = 7;

const bottleToPx = {}
for (let i = 0; i < bottleCount; i++) {
    bottleToPx[i] = {x: startX + i*gap, y: 364};
}
console.log(bottleToPx)

const wait = (timeMS) => new Promise(resolve => setTimeout(resolve, timeMS));

robot.setMouseDelay(2);

let sol = fs
    .readFileSync('./sol.txt')
    .toString()
    .trim()
    .split('\n')
    .map(s => s.trim());
console.log(sol);
(async function () {
    for (const move of sol) {
        console.log(move)
        const from = Number(move.split(' ')[0]);
        const to = Number(move.split(' ')[1]);
        const pxFrom = bottleToPx[from];
        const pxTo = bottleToPx[to];

        robot.moveMouse(pxFrom.x, pxFrom.y);
        console.log(pxFrom);
        robot.mouseClick('left', false);
        await wait(10);
        robot.moveMouse(pxTo.x, pxTo.y);
        console.log(pxTo);
        robot.mouseClick('left', false);
        await wait(10);
    }
})()
