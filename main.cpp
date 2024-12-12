#include <iostream>
#include "include/raylib.h"
#include <deque>
#include "include/raymath.h"
// #include <raymath.h>
using namespace std;

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double LastUpdateTime = 0;

Color ColorArray[] = {RED,ORANGE,YELLOW,GREEN,BLUE,DARKBLUE,PURPLE};

bool ElementInDeque(Vector2 element, deque<Vector2> dq){
    for(unsigned int i=0;i<dq.size();i++){
        if(Vector2Equals(element,dq[i]))
            return true;
    }
    return false;
}

bool EventTriggered(double interval){
    double CurrentTime = GetTime();
    if(CurrentTime - LastUpdateTime >= interval){
        LastUpdateTime = CurrentTime;
        return true;
    }
    return false;
}

class Food {
public:
    Vector2 position;
    Texture2D texture;
    Food(deque<Vector2> snakeBody){
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);  
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }
    ~Food(){
        UnloadTexture(texture);
    }
    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0,cellCount-1);
        float y = GetRandomValue(0,cellCount-1);
        return Vector2{x,y};
    }
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
        Vector2 position = GenerateRandomCell();
        while(ElementInDeque(position,snakeBody)){
            position = GenerateRandomCell();
        }

        return position;
    }
    void Draw(){
        DrawTexture(texture,offset+position.x * cellSize ,offset + position.y * cellSize , WHITE);
    }
};

class Snake{
public:
    
    deque<Vector2>  body = {Vector2{15,15},Vector2{16,15},Vector2{17,15},Vector2{18,15}};
    Vector2 direction = {-1,0};
    bool addSegment = false;
    Texture2D headTexture;
    float rotationAngle = 270.0f;
    Snake(){
        Image head = LoadImage("Graphics/head.png");
        headTexture = LoadTextureFromImage(head);
        UnloadImage(head);
    }
    ~Snake(){
        UnloadTexture(headTexture);
    }
    void Draw(){

        // DrawTexture(headTexture,offset + body[0].x*cellSize, offset + body[0].y*cellSize,WHITE);
        DrawTexturePro(headTexture,
            {0,0,(float)headTexture.width,(float)headTexture.height},
            {(float)(offset + body[0].x*cellSize + (cellSize>>1)),
             (float)(offset + body[0].y*cellSize + (cellSize>>1)),
             (float)cellSize,(float)cellSize  },
             {(float)(cellSize>>1),(float)(cellSize>>1)},
             rotationAngle,WHITE);

        for(unsigned int i=1;i<body.size();i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset+x*cellSize,offset+y*cellSize,(float) cellSize,(float) cellSize};
            DrawRectangleRounded(segment,0.5,0.6,ColorArray[i%8]);
        }
    }
    void update(){

        body.push_front(Vector2Add(body[0],direction));
        
        if(addSegment == true){
            addSegment = false;
        }
        else{
            body.pop_back();
        }
    }
    void Reset(){
        body = {Vector2{15,15},Vector2{16,15},Vector2{17,15},Vector2{18,15}};
        direction = {-1,0};
        addSegment = false;
    }
};

class Game{
public:
    int score = 0;
    bool Running = false;
    Snake snake = Snake();
    Food food = Food(snake.body);

    void Draw(){
        snake.Draw();
        food.Draw();
    }

    void Update(){
        if(Running){
            snake.update();
            CheckCollisonWithFood();
            CheckCollisonWithEdges();
            CheckCollisonWithBody();
        }
    }
    void CheckCollisonWithFood(){
        if(Vector2Equals(snake.body[0],food.position)){
            // generating new food at a random location excluding snake body
            food.position = food.GenerateRandomPos(snake.body);
            // making snake grow
            snake.addSegment = true;
            score++;
        }
    }

    void CheckCollisonWithEdges(){
        if(snake.body[0].x == -1 or snake.body[0].x == cellCount){
            GameOver();
        }
        if(snake.body[0].y == -1 or snake.body[0].y == cellCount){
            GameOver();
        }
    }

    void CheckCollisonWithBody(){
        deque<Vector2> SNAKE_BODY(snake.body.begin() + 1,snake.body.end());
        if(ElementInDeque(snake.body[0],SNAKE_BODY) ){
            GameOver();
        }
    }

    void GameOver(){
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        Running = false;
        score = 0;
        snake.rotationAngle = 270.0f;
    }
};

int main(void)
{
    InitWindow(2*offset + cellSize*cellCount,2*offset + cellSize*cellCount, "Rainbow Snake");
    SetTargetFPS(60);
    int highScore = 0;
    Game game = Game();

    while(WindowShouldClose() == false){

        BeginDrawing();
        
        // update game every 0.1 sec
        if(EventTriggered(0.13)){
            game.Update();
        }

        // change snake direction and head rotation angle when key pressed
        if(IsKeyPressed(KEY_UP) and game.snake.direction.y != 1){
            game.snake.direction = {0,-1};
            game.Running = true;
            game.snake.rotationAngle = 0.0f;
        }

        if(IsKeyPressed(KEY_DOWN) and game.snake.direction.y != -1){
            game.snake.direction = {0,1};
            game.Running = true;
            game.snake.rotationAngle = 180.0f;
        }

        if(IsKeyPressed(KEY_LEFT) and game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.Running = true;
            game.snake.rotationAngle = 270.0f;
        }
        
        if(IsKeyPressed(KEY_RIGHT) and game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.Running = true;
            game.snake.rotationAngle = 90.0f;
        }
        // drawing
        ClearBackground(DARKGRAY);
        // border
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellSize*cellCount+10,(float)cellSize*cellCount+10},5,YELLOW);
        game.Draw();
        DrawText("Rainbow Snake",offset-5,10,40,YELLOW);

        highScore = (highScore > game.score) ? highScore : game.score;
        
        DrawText(TextFormat("Score : %i",game.score),offset-5,cellSize*cellCount+offset+10,40,YELLOW);
        DrawText(TextFormat("High Score : %i",highScore),cellSize*cellCount - 3*offset,cellSize*cellCount+offset+10,40,YELLOW);
        // DrawText(text,x,y,size,color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
