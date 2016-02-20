#pragma once
const int defaultLCol = 5, defaultRCol = 5;//默认棋盘大小
const int maxLCol = 10, maxRCol = 10;//最大棋盘大小
const int minLCol = 3, minRCol = 3;//最小棋盘大小
const float movingCooling = 0.3f;//移动冷却时间
const float movingTime = 0.3f;//移动用时
const int maxMovementTimes = 5;//最大移动数量

enum Side { left, right };
enum Chessman { c_unknown, common, key, addCol, delCol };
enum GameMode { pvp, pva, pvo };
enum EndGameReason { unknown, opponentLeft, youWin, youLose };