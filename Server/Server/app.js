﻿var players = new Object();
var rooms = new Array();

const defaultLCol = 5, defaultRCol = 5;
const maxLCol = 10, maxRCol = 10;
const minLCol = 3, minRCol = 3;
const maxMovementTimes = 5;
const timeLimit = 20000;//超时时间(ms)
const MaxInterval = 30000;//两次移动之间的最大间隔(ms)
const waiting = 0, playing = 1, over = 2;//房间状态
const left = 0, right = 1;//左右侧
const unknow = 0, common = 1, key = 2, addCol = 3, delCol = 4;//棋子种类
const nothing = 0, leftWins = 1, rightWins = 2;//移动结果
const opponentLeft = 1, youWin = 2, youLose = 3, timeOut = 4;//结束理由

var io = require('socket.io').listen(8000);
console.log('listening on port 8000');

io.on('connection', function (socket) {
    console.log('connected ' + socket.id);
    //寻找可加入的房间
    var found = false;//是否找到了可加入的房间
    for (var i = 0; i < rooms.length; i++) {
        //如果房间为空，则建立房间并将玩家设为左边的玩家
        if (!rooms[i]) {
            found = true;
            rooms[i] = new Room();
            rooms[i].id = i;
            rooms[i].leftPlayer = socket;
            players[socket.id] = i;
            break;
        }
        //如果房间为等待中，则将玩家设为右边的玩家并开始游戏
        if (rooms[i].state == waiting) {
            found = true;
            console.log("start room:" + i);
            players[socket.id] = i;
            rooms[i].rightPlayer = socket;
            rooms[i].leftPlayer.emit("start", { side: left, room: i });
            rooms[i].newChessman = getRandomChessman();
            rooms[i].leftPlayer.emit('tellNewChessman', { chessman: rooms[i].newChessman });
            rooms[i].rightPlayer.emit("start", { side: right, room: i });
            rooms[i].state = playing;
            rooms[i].start();
            break;
        }
    }
    //如果没有找到可加入的房间，则新建一个房间
    if (!found) {
        var room = new Room();
        room.leftPlayer = socket;
        var index = rooms.push(room) - 1;
        players[socket.id] = index;
        room.id = index;
    }

    socket.on('beginMoving', function (data) {
        data = JSON.parse(data);
        if (data.col == undefined) { return; }
        var room = rooms[players[socket.id]];
        if (room.currentPlayer().id == socket.id) {
            console.log('beginMoving ' + JSON.stringify(data) + "   " + room.newChessman + "   " + room.turn + "   " + socket.id);
            if (!room.movingCol) {
                room.movingCol = data.col;
                clearTimeout(room.timeOutHandle);
                room.timeOutHandle = setTimeout(function () {
                    room.leftPlayer().emit('changeTurn');
                    room.rightPlayer().emit('changeTurn');
                    room.newChessman = getRandomChessman();
                    room.waitingPlayer().emit('tellNewChessman', { chessman: room.newChessman });
                    room.changeTurn();
                    console.log('changeTurn ' + room.turn);
                }, MaxInterval);
            }
            if (room.movingCol == data.col) {
                if (room.totalMovementTimes < maxMovementTimes) {
                    room.totalMovementTimes++;
                    var result = room.move(data.col, room.newChessman);
                    room.waitingPlayer().emit('beginMoving', { col: data.col, chessman: room.newChessman });
                    room.newChessman = getRandomChessman();
                    room.currentPlayer().emit('tellNewChessman', { chessman: room.newChessman });
                    if (result != nothing) {
                        if (result == leftWins) {
                            room.leftPlayer.emit('endGame', { reason: youWin });
                            room.rightPlayer.emit('endGame', { reason: youLose });
                        }
                        if (result == rightWins) {
                            room.rightPlayer.emit('endGame', { reason: youWin });
                            room.leftPlayer.emit('endGame', { reason: youLose });
                        }
                        closeRoom(players[socket.id]);
                    }
                } else {
                    room.currentPlayer().emit('changeTurn');
                    room.waitingPlayer().emit('changeTurn');
                    room.newChessman = getRandomChessman();
                    room.waitingPlayer().emit('tellNewChessman', { chessman: room.newChessman });
                    room.changeTurn();
                    room.movingCol = null;
                    room.totalMovementTimes = 0;
                    console.log('changeTurn ' + room.turn);
                }
            }
        }
    });

    socket.on('changeTurn', function () {
        var room = rooms[players[socket.id]];
        if (room.currentPlayer().id == socket.id && room.totalMovementTimes > 0) {
            room.waitingPlayer().emit('changeTurn');
            room.newChessman = getRandomChessman();
            room.waitingPlayer().emit('tellNewChessman', { chessman: room.newChessman });
            room.changeTurn();
            console.log('changeTurn ' + room.turn);
        }
    });

    socket.on('disconnect', function () {
        console.log('disconnected ' + socket.id);
        var roomid = players[socket.id];
        var room = rooms[roomid];
        if (room.state == playing) {
            if (room.leftPlayer.id == socket.id) {
                room.rightPlayer.emit('endGame', { reason: opponentLeft });
            }
            if (room.rightPlayer.id == socket.id) {
                room.leftPlayer.emit('endGame', { reason: opponentLeft });
            }
        }
        closeRoom(roomid);
    });
});

function closeRoom(id) {
    var room = rooms[id];
    if (room && room.state != over) {
        room.state = over;
        if (room.leftPlayer) {
            room.leftPlayer.disconnect();
            delete players[room.leftPlayer.id];
        }
        if (room.rightPlayer) {
            room.rightPlayer.disconnect();
            delete players[room.rightPlayer.id];
        }
        //清除延时
        clearTimeout(room.timeOutHandle);
        //删除房间（没有从数组里删除）
        delete rooms[id];
        //如果房间位于结尾，则从数组里删除此房间，并向后删除可删除的房间
        if (id == rooms.length - 1) {
            var i = id;
            while (i >= 0 && !rooms[i--]) {
                rooms.pop();
            }
        }
    }
}

function getRandomChessman() {
    switch (Math.floor(Math.random() * 11)) {
        case 0:
            return key;
        case 1:
            return addCol;
        case 2:
            return delCol;
        default:
            return common;
    }
}

function Room() {
    this.id = null;
    this.leftPlayer = null;
    this.rightPlayer = null;
    this.state = waiting;
    this.turn = left;
    this.lCol = defaultLCol;
    this.rCol = defaultRCol;
    this.newChessman = null;
    this.chessmen = new Array();
    for (var i = 0; i < maxLCol; i++) {
        this.chessmen[i] = new Array();
        for (var j = 0; j < maxRCol; j++) {
            this.chessmen[i][j] = common;
        }
    }
    this.movingCol = null;
    this.totalMovementTimes = 0;
    this.timeOutHandle = null;

    this.start = function () {
        var _this = this;
        this.timeOutHandle = setTimeout(function () { _this.timeOut() }, timeLimit);
    }

    this.move = function (col, chessman) {
        var lastChessman;//暂存最底下的棋子
        if (this.turn == left) {
            lastChessman = this.chessmen[col][this.rCol - 1];
            for (var i = this.rCol - 1; i > 0; i--) {
                this.chessmen[col][i] = this.chessmen[col][i - 1];
            }
            this.chessmen[col][0] = this.newChessman;
            switch (lastChessman) {
                case key:
                    return rightWins;
                    break;
                case addCol:
                    this.setBoardSize(this.lCol, this.rCol + 1);
                    break;
                case delCol:
                    this.setBoardSize(this.lCol, this.rCol - 1);
                    break;
            }
        }
        else {
            lastChessman = this.chessmen[this.lCol - 1][col];
            for (var i = this.lCol - 1; i > 0; i--) {
                this.chessmen[i][col] = this.chessmen[i - 1][col];
            }
            this.chessmen[0][col] = this.newChessman;
            switch (lastChessman) {
                case key:
                    return leftWins;
                    break;
                case addCol:
                    this.setBoardSize(this.lCol + 1, this.rCol);
                    break;
                case delCol:
                    this.setBoardSize(this.lCol - 1, this.rCol);
                    break;
            }
        }
        return nothing;
    }

    this.setBoardSize = function (lCol, rCol) {
        if (lCol > maxLCol || lCol < minLCol || rCol > maxRCol || rCol < minRCol)
            return false;
        if (this.lCol < lCol) {
            for (var i = this.lCol; i < lCol; i++) {
                for (var j = 0; j < rCol; j++) {
                    this.chessmen[i][j] = common;
                }
            }
        }
        if (this.rCol < rCol) {
            for (var i = 0; i < lCol; i++) {
                for (var j = this.rCol; j < rCol; j++) {
                    this.chessmen[i][j] = common;
                }
            }
        }
        this.lCol = lCol;
        this.rCol = rCol;
        return true;
    }

    this.currentPlayer = function () {
        return this.turn == left ? this.leftPlayer : this.rightPlayer
    }

    this.waitingPlayer = function () {
        return this.turn == left ? this.rightPlayer : this.leftPlayer
    }

    this.changeTurn = function () {
        this.turn = this.turn == left ? right : left;
        this.movingCol = null;
        this.totalMovementTimes = 0;
        clearTimeout(this.timeOutHandle);
        var _this = this;
        this.timeOutHandle = setTimeout(function () { _this.timeOut() }, timeLimit);
    }

    this.timeOut = function () {
        var room = rooms[this.id];
        this.leftPlayer.emit('endGame', { reason: timeOut });
        this.rightPlayer.emit('endGame', { reason: timeOut });
        closeRoom(this.id);
    }
}