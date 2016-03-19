#pragma once
const int defaultLCol = 5, defaultRCol = 5;//Ĭ�����̴�С
const int maxLCol = 10, maxRCol = 10;//������̴�С
const int minLCol = 3, minRCol = 3;//��С���̴�С
const float movingCooling = 0.3f;//�ƶ���ȴʱ��
const float movingTime = 0.3f;//�ƶ���ʱ
const int maxMovementTimes = 5;//����ƶ�����
const int timeLimit = 20;//��ʱʱ��

enum Side { left, right };
enum class Chessman { unknown, common, key, addCol, delCol };
enum class GameMode { pvp, pva, pvo };
enum class EndGameReason { unknown, opponentLeft, youWin, youLose, timeOut };