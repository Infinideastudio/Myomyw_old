# Myomyw
**Myomyw Alpha 0.2 已发布!**
## 游戏规则
游戏界面中两边是发射器，中间是棋盘，轮到你时你可以点击你的一个发射器，这样就可以把球推出去，按住发射器可以连续发射球，但最多只能发射五次。

发射器发射出的球有1/10的几率是决胜球(红色)，1/10是增列球(绿色带加号)，1/10是减列球(黄色带减号)。

* 如果一方把决胜球推出棋盘，那么这一方就算失败。
* 如果一方把增列球推出棋盘，那么推出球的那一边就会增加一列(最多10列)。
* 如果一方把减列球推出棋盘，那么推出球的那一边就会减少一列(最少3列)。

目前有单机双人、人机对战和联机对战三个模式。

## 配置
### 客户端
本游戏使用Cocos2d-x 3.9制作，如果你没有Cocos2d-x 3.9，你可以在[这里](http://www.cocos2d-x.org/filedown/cocos2d-x-3.9.zip)下载，然后把它解压到项目的*cocos2d*文件夹中。

如果你有其他使用Cocos2d-x 3.9的项目，你可以把其他项目的*cocos2d*文件夹复制过来。你也可以创建一个Cocos2d-x 3.9项目然后用本项目替换它，总之把Cocos2d-x 3.9源码放到*cocos2d*文件夹就对了。

### 服务端
服务端需要node.js才能运行，首次运行请在\\Server\\Server目录下运行npm install命令安装依赖模块。

服务端的开发使用了VS插件NTVS来方便编辑和调试，不过要是你不想用的话也可以。

## 更新历史
2016/2/4   建立Repository
2016/2/26  发布Alpha 0.1
2016/2/28  发布Alpha 0.1.1
2016/3/27  发布Alpha 0.2