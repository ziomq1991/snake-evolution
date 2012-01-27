#include "snakearea.h"
#include "ui_snakearea.h"
#include "snake.h"

#include <QPainter>
#include <QKeyEvent>
#include <QTime>
#include <QTimer>


#include "snakefunctions.h"

SnakeArea::SnakeArea(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnakeArea), colls(40), rows(20), fields(new int[colls*rows]),
    snake(new Snake(colls, rows, 10, Snake::MOVING_RIGHT, 420)), key(snake->getDirection()),
    timer(new QTimer(this))
{
    ui->setupUi(this);    
    initFields();
    qsrand(QTime().currentTime().msecsTo(QTime(0,0)));
    insertFood();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(moveSnake()));
    timer->start(300);
}

SnakeArea::~SnakeArea()
{
    delete timer;
    delete snake;
    delete[] fields;
    delete ui;        
}

void SnakeArea::paintEvent(QPaintEvent *){
    QPainter painter(this);    
    initPainter(&painter);
    drawCells(&painter);
}

void SnakeArea::keyPressEvent(QKeyEvent *e){
    switch(e->key()){
    case Qt::Key_Left:
        key = Snake::MOVING_LEFT;
        break;
    case Qt::Key_Up:
        key = Snake::MOVING_UP;
        break;
    case Qt::Key_Right:
        key = Snake::MOVING_RIGHT;
        break;
    case Qt::Key_Down:
        key = Snake::MOVING_DOWN;
        break;
    case Qt::Key_Space:
        if(timer->isActive()){
            timer->stop();
        }else{
            timer->start();
        }
        break;
    }

    return QDialog::keyPressEvent(e);
}

void SnakeArea::initFields(){
    for(int i=0; i<colls; ++i){
        for(int j=0; j<rows; ++j){
            fields[xy(i, j)] = 0; //background color
        }
    }
}

void SnakeArea::initPainter(QPainter *painter){
    painter->setRenderHint(QPainter::Antialiasing, true);    
}

void SnakeArea::drawCells(QPainter *painter){
    int cellWidth=width()/colls;
    int cellHeight=height()/rows;

    fields[snake->getLast()] = 0; //background color
    fields[snake->getFood()] = 3; //food's color
    fields[snake->getFirst()] = 2; //snake's head color    

    for(int i=1; i<snake->getLenght(); ++i){
        fields[snake->getSnake()[i]] = 1; //snake color
    }

    for(int i=0; i<colls; ++i){
        for(int j=0; j<rows; ++j){            
            painter->fillRect(i*cellWidth, j*cellHeight, cellWidth, cellHeight,
                              getBrush( fields[xy(i,j)] ));
        }
    }
}

QBrush SnakeArea::getBrush(int i){
    switch(i){
    case 0:
        return QBrush(Qt::white);
    case 1:
        return QBrush(Qt::green);
    case 2:
        return QBrush(Qt::red);
    case 3:
        return QBrush(Qt::magenta);
    }
    return QBrush(Qt::black);
}

void SnakeArea::restart(){
    initFields();
    snake->restart(10, Snake::MOVING_RIGHT);
}

void SnakeArea::insertFood(){
    int x = qrand()%(colls*rows);
    int *s = snake->getSnake();
    for(int i=0; i<snake->getLenght(); ++i){
        if(x == s[i]){
            i=-1;
            x = qrand()%(colls*rows);
        }
    }
    snake->setFood(x);
}

void SnakeArea::moveSnake(){
    if(!snake->move2(key)){
        restart();
    }
    update();
    resetKey();
    if(snake->isFoodEaten()){
        insertFood();
    }
    int k = 7;
    int w = -1;
    int r = -1;
    SnakeFunction **sf = new SnakeFunction*[k];
    SnakeFunction::snake = snake;
    sf[++w] = new IsMovingRight();
    sf[++w] = new IsMovingLeft();
    sf[++w] = new IsMovingUp();
    sf[++w] = new IsMovingDown();
    sf[++w] = new IsFoodAhead();
    sf[++w] = new IsFoodRight();
    sf[++w] = new IsFoodLeft();
    qDebug("-----------------");
    qDebug(("isMovingRight " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isMovingLeft " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isMovingUp " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isMovingDown " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isFoodAhead " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isFoodRight " + QString::number(sf[++r]->exec())).toAscii());
    qDebug(("isFoodLeft " + QString::number(sf[++r]->exec())).toAscii());
    for(int i=0; i<k; ++i){
        delete sf[i];
    }
    delete[] sf;
}

void SnakeArea::resetKey(){
    key = snake->getDirection();
}


