﻿var config = require('./config.js')
var EndReason = { opponentLeft: 1, youWin: 2, youLose: 3, timeOut: 4 };
var Chessman = { unknow: 0, common: 1, key: 2, addCol: 3, delCol: 4, flip: 5 };
const left = 0, right = 1;
function Room(onClose) {
    this.chessmen = new Array();
    for (var i = 0; i < config.maxLCol; i++) {
        this.chessmen[i] = new Array();
        for (var j = 0; j < config.maxRCol; j++) {
            this.chessmen[i][j] = Chessman.common;
        }
    }
    this.lCol = config.defaultLCol;
    this.rCol = config.defaultRCol;
    this.turn = left;
    this.leftPlayer = null;
    this.rightPlayer = null;
    this.newChessman = null;
    this.movingCol = null;
    this.over = false;
    this.totalMovementTimes = 0;
    this.timeOutHandle = null;
    this.close = onClose;
}

Room.prototype.setPlayer = function (side, socket) {
    if (!(side == left ? this.leftPlayer : this.rightPlayer)) {
        if (side == left) {
            this.leftPlayer = socket;
        } else {
            this.rightPlayer = socket;
        }
        var room = this;
        socket.on('move', this.onMove.bind(this, side));
        socket.on('endTurn', this.onEndTurn.bind(this, side));
        socket.on('disconnect', this.onDisconnect.bind(this, side));
    }
}

Room.prototype.start = function (turn) {
    this.leftPlayer.emit('start', { side: left });
    this.rightPlayer.emit('start', { side: right });
    this.setTurn(turn);
}

Room.prototype.currentPlayer = function () {
    return this.turn == left ? this.leftPlayer : this.rightPlayer
}

Room.prototype.waitingPlayer = function () {
    return this.turn == left ? this.rightPlayer : this.leftPlayer
}

//只有每回合的第一次移动才传递col
Room.prototype.onMove = function (side, data) {
    if (side != this.turn) { return; }
    data = parseJSON(data);
    if ('col' in data && !this.movingCol) {
        this.movingCol = data.col;
    }
    if (this.movingCol != null && this.totalMovementTimes < config.maxMovementTimes) {
        this.totalMovementTimes++;
        room.waitingPlayer().emit('move', { col: data.col, chessman: room.newChessman });
        if (this.move(this.movingCol, this.newChessman)) {
            this.currentPlayer().emit('endGame', { reason: EndReason.youWin });
            this.waitingPlayer().emit('endGame', { reason: EndReason.youLose });
            clearTimeout(this.timeOutHandle);
            this.over = true;
            room.leftPlayer.disconnect();
            room.rightPlayer.disconnect();
            this.close();
        } else {
            clearTimeout(this.timeOutHandle);
            //此举是为了防止两次移动间间隔时间过长
            this.timeOutHandle = setTimeout(this.timeOut.bind(this), config.maxInterval);
            this.newChessman = this.getRandomChessman();
            this.currentPlayer().emit('newChessman', { chessman: this.newChessman });
        }
    }
}

Room.prototype.onEndTurn = function (side) {
    console.log('onEndTurn');
    if (side != this.turn) { return; }
    if (this.movingCol != null) {
        this.movingCol = null;
        this.setTurn(this.turn == left ? right : left);
        this.currentPlayer().emit('changeTurn');
        console.log('sent');
    }
}

Room.prototype.onDisconnect = function (side) {
    console.log('disconnected ' + (side == right ? this.rightPlayer : this.leftPlayer).id);
    if (!this.over) {
        (side == left ? this.rightPlayer : this.leftPlayer).emit('endGame', { reason: EndReason.opponentLeft });
        clearTimeout(this.timeOutHandle);
        this.close();
    }
}

Room.prototype.setTurn = function (turn) {
    this.turn = turn;
    this.movingCol = null;
    this.totalMovementTimes = 0;
    clearTimeout(this.timeOutHandle);
    this.timeOutHandle = setTimeout(this.timeOut.bind(this), config.timeLimit);
    this.newChessman = this.getRandomChessman();
    this.currentPlayer().emit('newChessman', { chessman: this.newChessman });
}

Room.prototype.timeOut = function () {
    this.leftPlayer.emit('endGame', { reason: EndReason.timeOut });
    this.rightPlayer.emit('endGame', { reason: EndReason.timeOut });
    this.close();
}

Room.prototype.getRandomChessman = function () {
    switch (Math.floor(Math.random() * 11)) {
        case 0:
            return Chessman.key;
        case 1:
            return Chessman.addCol;
        case 2:
            return Chessman.delCol;
        case 3:
            return Chessman.flip;
        default:
            return Chessman.common;
    }
}

Room.prototype.setBoardSize = function (lCol, rCol) {
    if (lCol > config.maxLCol || lCol < config.minLCol || rCol > config.maxRCol || rCol < config.minRCol)
        return false;
    if (this.lCol < lCol) {
        for (var i = this.lCol; i < lCol; i++) {
            for (var j = 0; j < rCol; j++) {
                this.chessmen[i][j] = Chessman.common;
            }
        }
    }
    if (this.rCol < rCol) {
        for (var i = 0; i < lCol; i++) {
            for (var j = this.rCol; j < rCol; j++) {
                this.chessmen[i][j] = Chessman.common;
            }
        }
    }
    this.lCol = lCol;
    this.rCol = rCol;
    return true;
}

//返回值为是否已决胜负
Room.prototype.move = function (col, chessman) {
    var lastChessman;//暂存最底下的棋子
    if (this.turn == left) {
        lastChessman = this.chessmen[col][this.rCol - 1];
        for (var i = this.rCol - 1; i > 0; i--) {
            this.chessmen[col][i] = this.chessmen[col][i - 1];
        }
        this.chessmen[col][0] = this.newChessman;
        switch (lastChessman) {
            case Chessman.key:
                return true;
                break;
            case Chessman.addCol:
                this.setBoardSize(this.lCol, this.rCol + 1);
                break;
            case Chessman.delCol:
                this.setBoardSize(this.lCol, this.rCol - 1);
                break;
            case Chessman.flip:
                this.flip();
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
            case Chessman.key:
                return true;
                break;
            case Chessman.addCol:
                this.setBoardSize(this.lCol + 1, this.rCol);
                break;
            case Chessman.delCol:
                this.setBoardSize(this.lCol - 1, this.rCol);
                break;
            case Chessman.flip:
                this.flip();
                break;
        }
    }
    return false;
}

Room.prototype.flip = function () {
    for (var i = 0; i < this.lCol; i++) {
        for (var j = i + 1; j < this.rCol; j++) {
            this.chessmen[i][j] ^= this.chessmen[j][i];
            this.chessmen[j][i] ^= this.chessmen[i][j];
            this.chessmen[i][j] ^= this.chessmen[j][i];
        }
    }
    this.lCol ^= this.rCol;
    this.rCol ^= this.lCol;
    this.lCol ^= this.rCol;
}

function parseJSON(text) {
    if (text != '') {
        try {
            var obj = JSON.parse(text);
            return obj;
        } 
        catch (e) {
            return {};
        }
    } else {
        return {};
    }
}

module.exports = Room;
