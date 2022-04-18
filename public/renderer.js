
const socket = io();
let moves = {};
let engineSide = 'b';
let nonEngineSide = 'w';
var board;

socket.emit('command', 'getFen\n');
socket.emit('command', 'getMoves\n');
socket.emit('command', 'eval\n');

function shift(data, nr) {
    for(let i = 0; i < nr; i++)
        data.shift();
    return data;
}

socket.on('result', (data) => {
    data = data.replace('\r\n', ' ').split(' ');
    console.log(data);

    while(data.length != 0) {
        while(data.length && data[0].length == 0)
            data.shift();
        if(data[0] == 'moves') {
            for(let i = 0; i < data[1].length; i += 4)
                moves[data[1].substr(i, 4)] = 1;
            data = shift(data, 2);
        } else if(data[0] == 'Fen') {
            if(data[2] == 'w') 
                board = Chessboard('myBoard', {
                    draggable: true,
                    position: data[1],
                    onDrop: onDrop,
                });
            else
                board = Chessboard('myBoard', {
                    draggable: true,
                    position: data[1],
                    onDrop: onDrop,
                    orientation: 'black'
                });
                moves = {};
                socket.emit('command', 'getMoves\n');
            side = data[2];
            if(engineSide == side)
                [engineSide, nonEngineSide] = [nonEngineSide, engineSide];
            data.shift();
        } else if(data[0] == 'engine') {
            const source = data[1].substr(0, 2);
            const target = data[1].substr(2, 2);
            board.move(source + '-' + target);
            side = nonEngineSide;
            socket.emit('command', 'getMoves\n');
            socket.emit('command', 'eval\n');
        } else if(data[0] == 'eval') {
            eval(data[1]);
        }
        data = shift(data, 2);
    }
})

let button = document.getElementById('sideButton');
button.onclick = function changeSize() {
    [engineSide, nonEngineSide] = [nonEngineSide, engineSide];
    socket.emit('command', 'make\n');
}


let undoButton = document.getElementById('undoButton');
undoButton.onclick = function undo() {
    console.log('undo');
    socket.emit('command', 'undo\n');
}


function eval(nr) {
    let evalValue = document.getElementById('eval');
    evalValue.textContent = 'AI Evaluation: ' + nr;
}

function onDrop(source, target) {
    if (source == target || moves[source + target] == undefined)
        return 'snapback';
    
    if(side != engineSide) {
        moves = {};
        socket.emit('command', source + target + '\n');
        side = engineSide;
        socket.emit('command', 'make\n');
    }
}
