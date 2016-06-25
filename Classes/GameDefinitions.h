#pragma once
const int defaultLCol = 6, defaultRCol = 6;//Ĭ�����̴�С
const int maxLCol = 10, maxRCol = 10;//������̴�С
const int minLCol = 3, minRCol = 3;//��С���̴�С
const float movingCooling = 0.4f;//�ƶ���ȴʱ��
const float movingTime = 0.3f;//�ƶ���ʱ
const int maxMovementTimes = 5;//����ƶ�����
const int timeLimit = 20;//��ʱʱ��
const float boardScaleTime = 0.2f;//���Ŷ���ʱ��

enum Side { left, right };
enum class Chessman { unknown, common, key, addCol, delCol, flip };
enum class ActionState { nothing, moving, cooling };
enum class GameMode { pvp, pva, pvo };
enum class EndGameReason { unknown, opponentLeft, youWin, youLose, timeOut };