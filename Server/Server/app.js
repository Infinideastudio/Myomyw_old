var leftPlayer, rightPlayer;
const left = 0, right = 1;
var turn = left;
var io = require('socket.io').listen(8000);
console.log('listening on port 8000');

io.on('connection', function (socket) {
    console.log('connected ' + socket.id);
    if (!leftPlayer) {
        leftPlayer = socket;
    } else if (!rightPlayer) {
        rightPlayer = socket;
        leftPlayer.emit("start", left);
        leftPlayer.emit('tellNewChessman', getRandomChessman());
        rightPlayer.emit("start", right);
    }

    socket.on('beginMoving', function (data) {
        console.log('beginMoving ' + data + "   " + turn + "   " + socket.id);
        (turn == left ? rightPlayer : leftPlayer).emit('beginMoving', JSON.parse(data));
        (turn == left ? leftPlayer : rightPlayer).emit('tellNewChessman', getRandomChessman());
    });

    socket.on('changeTurn', function () {
        console.log('changeTurn');
        (turn == left ? rightPlayer : leftPlayer).emit('changeTurn');
        (turn == left ? rightPlayer : leftPlayer).emit('tellNewChessman', getRandomChessman());
        turn = turn == left ? right : left;
    });

    socket.on('disconnect', function () {
        console.log('disconnected ' + socket.id);
    });
});

function getRandomChessman() {
    switch (Math.floor(Math.random() * 11)) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 3;
        default:
            return 4;
    }
}