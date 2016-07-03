var config = require('./config.js')
var Room = require('./Room.js')
var URL = require('url');
var rooms = new Array();
const left = 0, right = 1;
var http = require('http').createServer(
    function (request, response) {
        response.writeHead(200);
        switch (URL.parse(request.url).path) {
            case "/is_server":
                response.write('test server');
                break;
            default:
                response.write('<h1>It is a Myomyw server.<h1>');
                break;
        }
        response.end();
    }).listen(8000);
var io = require('socket.io')(http);

console.log('listening on port 8000');

io.on('connection', function (socket) {
    console.log('connected ' + socket.id);
    //寻找可加入的房间
    var found = false;//是否找到了可加入的房间
    for (var i = 0; i < rooms.length; i++) {
        //如果房间为空，则建立房间并将玩家设为左边的玩家
        if (!rooms[i]) {
            found = true;
            console.log("open room:" + i);
            rooms[i] = new Room(closeRoom.bind(this,i));
            rooms[i].setPlayer(left, socket);
            break;
        }
        //如果房间为等待中，则将玩家设为右边的玩家并开始游戏
        if (!rooms[i].rightPlayer) {
            found = true;
            rooms[i].setPlayer(right, socket);
            break;
        }
    }
    //如果没有找到可加入的房间，则新建一个房间
    if (!found) {
        room = new Room(closeRoom.bind(this, i));
        room.setPlayer(left, socket);
        rooms.push(room);
    }
});

function closeRoom(id) {
    if (rooms[id]) {
        rooms[id] = null;
        console.log("close room:" + id);
        //如果房间位于结尾，则从数组里删除此房间，并向后删除可删除的房间
        if (id == rooms.length - 1) {
            var i = id;
            while (i >= 0 && !rooms[i--]) {
                rooms.pop();
            }
        }
    }
}
