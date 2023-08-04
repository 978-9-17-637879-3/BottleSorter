const robot = require('robotjs');
const fs = require('fs');

const startX = 171 - 64;
const gap = 62;
const bottleCount = 15;

const bottleToPx = {}
for (let i = 0; i < bottleCount; i++) {
    bottleToPx[i] = { x: startX + i * gap, y: 353 };
}
console.log(bottleToPx)

const wait = (timeMS) => new Promise(resolve => setTimeout(resolve, timeMS));
const moveParser = (moveString) => { const noParentheses = moveString.replaceAll(/(\(|\))/g, "").split(" "); return [Number(noParentheses[0]), Number(noParentheses[1])] }

let sol = fs
    .readFileSync('./sol.txt')
    .toString()
    .trim()
    .split('->')
console.log(sol);
(async function () {
    for (const move of sol) {
        console.log(move)
        const [from, to] = moveParser(move);
        const pxFrom = bottleToPx[from];
        const pxTo = bottleToPx[to];

        robot.moveMouse(pxFrom.x, pxFrom.y);
        console.log(pxFrom);
        robot.mouseClick('left', false);
        await wait(100);
        robot.moveMouse(pxTo.x, pxTo.y);
        console.log(pxTo);
        robot.mouseClick('left', false);
        await wait(100);
    }
})()
