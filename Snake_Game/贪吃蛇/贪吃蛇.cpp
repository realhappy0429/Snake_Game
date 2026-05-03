#include <iostream>
#include<easyx.h>
#include<vector>
#include<random>
using namespace std;
/*精灵类*/
class Sprite {
public:
    Sprite() :Sprite(0, 0) {

    }
    Sprite(int x, int y) :x(x),y(y),color(RED) {

    }
    //绘制精灵
    virtual void draw() {
        //设置颜色
        setfillcolor(color);
        //绘制
        fillrectangle(x, y, x + 10, y + 10);
    }
    void moveBy(int dx,int dy) {
        x += dx;
        y += dy;
    }

    //碰撞检测
    bool collision(const Sprite& other){
        return x == other.x && y == other.y;
    }

    bool outOfRange() {
        if (x < 0 || x>640 || y < 0 || y>480) {
            return true;
        }
        return false;
    }
protected:
    int x, y;
    COLORREF color; //颜色
};

//蛇类

class Snake : public Sprite {

public:
    Snake() :Snake(0,0) {

    }
    Snake(int x, int y) :Sprite(x,y),dir(VK_RIGHT) {
        nodes.push_back(Sprite(20, 0));
        nodes.push_back(Sprite(10, 0));
        nodes.push_back(Sprite(0, 0));
    }
    void draw() override{
        for (int i = 0; i < nodes.size(); i++) {
            nodes[i].draw();
        }
    }
    //蛇的移动
    void move() {
        for (int i = nodes.size()-1; i >=1; i--) {
            nodes[i] = nodes[i-1];
        }
        switch (dir) {
        case VK_UP:
            nodes[0].moveBy(0, -10);
            break;
        case VK_DOWN:
            nodes[0].moveBy(0, 10);
            break;
        case VK_LEFT:
            nodes[0].moveBy(-10, 0);
            break;
        case VK_RIGHT:
            nodes[0].moveBy(10,0);
            break;

        }
    
    }
    bool collision(const Sprite& other) {
        return nodes[0].collision(other);
    }
    //蛇增加
    void increase() {
        nodes.push_back(Sprite());
    }
    bool collisionSelf() {
        for (int i = 1; i < nodes.size(); i++) {
            if (nodes[0].collision(nodes[i])) {
                return true;
            }
        }
        return false;
    }
    bool outOfRangeNode0(){
        return nodes[0].outOfRange();
    }
private:
    //蛇的所有结点
    vector<Sprite> nodes;
public:
    int dir;

};


class Food :public Sprite {
public:
    Food() :Sprite(0, 0) {
        changePos();
    }
    void draw()override {
        setfillcolor(color);
        solidellipse(x, y, x + 10, y + 10);
    }
    void changePos()
    {
        x = (rand() % 64) * 10;
        y = (rand() % 48) * 10;
    }
};

//游戏场景
class GameScene {
public:
    GameScene() {
        //设置随机种子
    }
    void run() {
        //双缓冲
        BeginBatchDraw();

        //cleardevice();
        setbkcolor(RGB(30, 30, 50));
        cleardevice();

        // 放在 cleardevice() 后面
        setlinecolor(RGB(100, 100, 200));
        setlinestyle(PS_SOLID, 2);
        rectangle(5, 5, 635, 475);

        // 画分数
        settextcolor(RGB(255, 220, 0)); // 金黄色
        settextstyle(26, 0, L"微软雅黑");
        TCHAR buf[60];
        wsprintf(buf, L"分数：%d", score);
        outtextxy(480, 15, buf);

        if (!isGameOver) {

            snake.draw();
            food.draw();

            EndBatchDraw();
            //蛇移动坐标
            snake.move();
            //蛇是否吃到食物
            snakeEatFood();
            gameOver();
        }
        else {
            settextcolor(RED);
            settextstyle(40, 0, L"黑体");
            outtextxy(230, 180, L"游戏结束");

            settextcolor(WHITE);
            settextstyle(28, 0, L"微软雅黑");
            TCHAR scoreBuf[60];
            wsprintf(scoreBuf, L"最终分数：%d", score);
            outtextxy(220, 240, scoreBuf);

            outtextxy(190, 300, L"按 空格键 重新开始");
            EndBatchDraw();

            //检测空格重启
            ExMessage msg = { 0 };
            while (peekmessage(&msg, EX_KEY)) {
                if (msg.message == WM_KEYDOWN && msg.vkcode == VK_SPACE) {
                    resetGame();
                }
            }
        }
        //获取消息
        ExMessage msg = { 0 };
        while (peekmessage(&msg,EX_KEY)) {
            onMessage(msg);
        }
    }
    //获取消息
    void onMessage(const ExMessage& msg) {
        //如果有键盘消息（用户键盘按下）
        if (msg.message==WM_KEYDOWN) {
            //判断哪个按键
            if (msg.vkcode == VK_UP || msg.vkcode == VK_DOWN || msg.vkcode == VK_LEFT || msg.vkcode == VK_RIGHT) {
                snake.dir = msg.vkcode;
            }
        }
    }

    //判断蛇是否吃到食物
    void snakeEatFood() {
        if (snake.collision(food)) {
            //蛇+1
            score += 100;
            snake.increase();
            food.changePos();
        }
    }
    void gameOver() {
        if (snake.collisionSelf() || snake.outOfRangeNode0()) {
            isGameOver = true;
        }
    }
    void resetGame() {
        score = INIT_SCORE;
        isGameOver = false;
        snake = Snake(0, 0);
        food = Food();

    }
private:
    int score = 0;
    Snake snake;
    Food food;
    bool isGameOver = false;
    const int INIT_SCORE = 0;
};



int main()
{
    initgraph(640, 480);
    srand(time(nullptr));
    GameScene scene;

    while (true) {
        scene.run();
        Sleep(80);
    }
    closegraph();
    return 0;
}

