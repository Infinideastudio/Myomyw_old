#pragma once
const int defaultLCol = 6, defaultRCol = 6;//默认棋盘大小
const int maxLCol = 10, maxRCol = 10;//最大棋盘大小
const int minLCol = 3, minRCol = 3;//最小棋盘大小
const float movingCooling = 0.4f;//移动冷却时间
const float movingTime = 0.3f;//移动用时
const int maxMovementTimes = 5;//最大移动数量
const int timeLimit = 20;//超时时间
const float aiThinkingTime = 0.8f;
const float boardScaleTime = 0.2f;//缩放动画时间
const float boardFilpTime = 0.4f;//翻转动画时间
const float nextChessmanChangeTime = 0.5f;//切换下一个棋子提示的时间

enum Side { left, right };
enum class Chessman { unknown, common, key, addCol, delCol, flip };
enum class ActionState { nothing, moving, cooling };
enum class GameMode { pvp, pva, pvo };
enum class EndGameReason { unknown, opponentLeft, youWin, youLose, timeOut };