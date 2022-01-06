#include <deque>
#include <thread>
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

using namespace std;

int sx = 0;
int sy = 0;
int w=19;
int h=11;
char wall = '#';

inline void setcur(int x,int y) { printf("\033[%d;%dH",y+1,x+1);}

void del(int x,int y)
{
    setcur(x,y);
    cout << ' ';
}

inline void clear() { system("clear"); }

struct termios old_tio, new_tio;//GLOBAL SETTINGS VARIABLES

void set(int x,int y,char c)
{
    setcur(x,y);
    cout << c;
}

struct pos
{
    bool operator== (const pos &var)
    {
        return this->x == var.x && this->y == var.y;
    }
    int x;
    int y;
};

struct snake
{
    public: 
    snake()
    {

    }
    snake(int X,int Y)
    {
        head.x=X;
        head.y=Y;
        body.push_front(head);
    }
    deque<pos> body;
    pos head;
    char dir = '\0';
    char c='o';
    void up()
    {
        head.y--;
        pos p=body[0];
        del(p.x,p.y);
        body.pop_back();
        body.push_front(head);//
        p = head;
        set(p.x,p.y,c);
    }
    void down()
    {
        head.y++;
        pos p = body[0];
        del(p.x,p.y);
        body.pop_back();
        body.push_front(head);
        p = head;
        set(p.x,p.y,c);
    }
    void left()
    {
        head.x--;
        pos p=body[0];
        del(p.x,p.y);
        body.pop_back();
        body.push_front(head);
        p = head;
        set(p.x,p.y,c);
    }
    void right()
    {
        head.x++;
        pos p = body[0];
        del(p.x,p.y);
        body.pop_back();
        body.push_front(head);
        p = head;
        set(p.x,p.y,c);
    }
    void eat()
    {
        if(dir == 'w') head.y--;
        if(dir == 's') head.y++;
        if(dir == 'a') head.x--;
        if(dir == 'd') head.x++;
        body.push_front(head);
    }
};

struct apple
{
    public:
    apple()
    {

    }
    pos position;
    char c='@';
    void respawn()
    {
        del(position.x,position.y);
        position.x = rand() % (sx+w) + sx;
        //position.x -= 1;
        position.y = rand() % (sy+h) + sy;
        position.y -= 1;
        set(position.x,position.y,c);
    }
};

snake s = snake(sx+w/2,sy+h/2);
apple ap = apple();

void drawSnake()
{
    for(int i=0;i<s.body.size();i++)
    {
        pos p = s.body[i];
        set(p.x,p.y,s.c);
    }
}

void drawField()
{
    for(int i=sy;i < h+sy;i++)
    {
        for(int j=sx;j < w+sx;j++)
        {
            if((i == sy || i == (h-1)+sy) || (j == sx || j == (w-1)+sx))
            {
                set(j,i,wall);
            }
        }
        cout<<endl;
    }
}

void keyListener()
{
    for(;;)
    {
        this_thread::sleep_for (std::chrono::milliseconds(100));
        s.dir = getchar();
    }
}

thread kl;

class esc
{
    public:
    esc()
    {
        tcgetattr(STDIN_FILENO,&old_tio);//Get settings in variable
        new_tio=old_tio;//Copy old settings to new
        new_tio.c_lflag &= (~ICANON & ~ECHO);//Set new flags
        tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);//Set new settings
    }
    ~esc()
    {
        tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);//Set old settings
    }
};

int main()
{
    //esc ex = esc();

    drawField();

    drawSnake();

    ap.respawn();

    //kl = thread(keyListener);
    //kl.detach();

    for(;;)
    {   
        usleep(150000);
        set(0,0,s.dir);
        if(s.head.x <= sx+1 || s.head.y <= sy+1 || s.head.x >= w-1 || s.head.y >= h-1) break;
        if(s.head == ap.position)
        {
            s.eat();
            ap.respawn();
        }
        if(s.dir == 'w') {s.up(); continue;   }//UP
        if(s.dir == 's') {s.down(); continue; }//DOWN
        if(s.dir == 'a') {s.left(); continue; }//LEFT
        if(s.dir == 'd') {s.right(); continue;} //RIGHT
    }

    setcur(0,0);

    cout << "GAME OVER!" << endl;
    cout << "SCORE: " << s.body.size() << endl;
    
    return 0;
}