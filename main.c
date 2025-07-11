#include <raylib.h>
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

#define SNAKE_SPEED 600.0f
#define SNAKE_SEGMENT_SIZE 20 
#define screenWidth 500
#define screenHeight 500

typedef struct Step {
        Vector2 pos;
        Vector2 dir;
        struct Step* next;
    } Step;

typedef struct {
    Step* head; 
    Step* tail;
    int size; 
    int max_size;
} Path; 

typedef struct Fruit {
    Vector2 pos;
    Color color; 
    bool active; 
} Fruit;

Step* createStep(Vector2 pos, Vector2 dir);
void initPath(Path* path, int initial_size, Vector2 initial_dir);
void addStepBeginning(Path* path, Vector2 pos);
void removeLastStep(Path* path);
void freePath(Path* path);
void generateFruits(Fruit* fruits, int size); 
void updateDirection(Vector2* dir, Vector2 current_dir);
void drawSnake(Path* path, Vector2 dir, float deltaTime);
void checkFruitCollision(Path* path, Fruit* fruits, int fruit_count, int* points);
int checkSnakeCollision(Path* path);



int main(){

    Path path; 

    InitWindow(screenWidth, screenHeight, "Snake Game");
    Vector2 head_position = {screenWidth/2, screenHeight/2};
    Vector2 dir = {1, 0};
    initPath(&path, 3, dir);
    Vector2 prev_dir = dir;
    SetTargetFPS(30);
    int points = 0; 
    int end = 0;

    //addStepBeginning(&path, position);

    int number_fruits = 6; 
    Fruit fruits[number_fruits];

    generateFruits(fruits, number_fruits); 

    while(!WindowShouldClose()){

        float deltaTime = GetFrameTime();
        // Update direction (prevent 180 degree turns)
        updateDirection(&dir, prev_dir);
        prev_dir = dir; 

        // Update position
        //head_position.x += SNAKE_SPEED*deltaTime*dir.x;
        //head_position.y += SNAKE_SPEED*deltaTime*dir.y;

        // Check collisions with border 
        
//      checkPoints();
//      Add new head position
//
//        addStepBeginning(&path, position);

//        if (path.size > path.max_size){
//            removeLastStep(&path);
//        }
        
        checkFruitCollision(&path, fruits, number_fruits, &points);
//        end = checkSnakeCollision(&path);
        
        

        BeginDrawing();
//            printf("END: %d",end);
//            if (end != 0){
//                ClearBackground(RED);
//
//                DrawText(TextFormat("GAME OVER"), screenWidth/2, screenHeight/2, 20, DARKGRAY);
//                DrawText(TextFormat("You obtained %d points"), screenWidth/2, screenHeight/2+20, 20, DARKGRAY);
//                WaitTime(3.0f);
//                break;
//            }
            ClearBackground(LIGHTGRAY);

            for (int i = 0; i < number_fruits; i++){
                if(fruits[i].active) {
                    DrawCircleV(fruits[i].pos,
                                5.0, 
                                fruits[i].color);
                }
            }
            //Draw snake
            
            drawSnake(&path, dir, deltaTime);

            Rectangle button = {screenWidth - 20, 20, 20, 20};

            //DrawRectangleRec(button, RED);

//          DrawSnake(&path, points);
//          Draw score 
            DrawText(TextFormat("Score: %d", points), 10, 10, 20, DARKGRAY);

        EndDrawing();
    }


    CloseWindow();
    freePath(&path);
    return 0;
}

Vector2 checkBorderCollision(Vector2 position){
    if (position.x < 0) position.x = screenWidth;
    if (position.x > screenWidth) position.x = 0;
    if (position.y < 0) position.y = screenHeight;
    if (position.y > screenHeight) position.y = 0;

    return position;
}

Step* createStep(Vector2 position, Vector2 dir){
    Step* newStep = (Step*)malloc(sizeof(Step));
    if (newStep == NULL){
        printf("Memory allocation failed");
        exit(1);
    }
    newStep->pos = position;
    newStep->next = NULL;
    newStep->dir = dir;
    return newStep;
}

void addStepTail(Path* path){
    Step* tail = path->tail;
    Vector2 tail_dir = tail->dir; 
    // the position of the new tail is going to be the current tail position - SNAKE_SEGMENT*direction
    Vector2 tail_position = Vector2Add(tail->pos , Vector2Scale(tail_dir, -SNAKE_SEGMENT_SIZE));

    Step* newStep = createStep(tail_position, tail_dir);
    tail->next = newStep;
    path->tail = newStep;
    path->size++;
}


void initPath(Path* path, int initial_size, Vector2 initial_dir){

    Vector2 start_pos = {screenWidth/2, screenHeight/2};
    //Create the head
    Step* snake_head = (Step*)malloc(sizeof(Step));
    snake_head->pos = start_pos; 
    snake_head->dir = initial_dir;
    snake_head->next = NULL;

    path->head = snake_head;
    path->tail = snake_head;
    path->size = 1;
    path->max_size = initial_size;

    for (int i =1; i < initial_size; i++){
        Vector2 seg_pos = (Vector2){start_pos.x -i*SNAKE_SEGMENT_SIZE, start_pos.y};
        addStepTail(path);
    }
}

void updateDirection(Vector2* dir, Vector2 current_dir){
    if (IsKeyPressed(KEY_D) && current_dir.x != -1){
                dir->x = 1;
                dir->y = 0;
            }
    if(IsKeyDown(KEY_A) && current_dir.x != 1 ) {dir->x = -1; dir->y = 0;};
    if(IsKeyDown(KEY_W) && current_dir.y != 1) {dir->x = 0; dir->y= -1;};
    if(IsKeyDown(KEY_S) && current_dir.y != -1) {dir->x = 0;  dir->y= 1;}; 
}            

void addStepBeginning(Path* path, Vector2 pos){
    if (path == NULL){
        fprintf(stderr, "Error: Path is Null\n");
        return;
    }
    Step* newStep = createStep(pos, path->head->dir);
    newStep->next= path->head;
    path->head = newStep;
    if (path->tail == NULL){
        path->tail = newStep;
    }
    path->size++;

}

void removeLastStep(Path* path) {
    if (path->head == NULL || path->size <=1 )return; 

    Step* tail = path->tail;
    free(tail);
    tail->next = NULL;
    path->size--;
}

void drawSnake(Path* path, Vector2 new_dir, float deltaTime) {
    Step* current = path->head;
    Vector2 temp_dir = { 0 };
    Vector2 new = new_dir;
    while(current != NULL){
        //UPDATE DIRECTION 
        temp_dir = current->dir;
        current->dir = new; 
        //MOVE SEGEMENT 
        current->pos = Vector2Add(current->pos , Vector2Scale(new, SNAKE_SPEED*deltaTime)); 
        current->pos = checkBorderCollision(current->pos);         
            //BEGIN DRAWING 
        DrawRectangleV(current->pos, (Vector2){SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE},
                DARKGREEN);
        //END DRAWING 
        current = current->next;
        new = temp_dir;
    }
}

int random_in_range(int min, int max){
    return min + rand() % (max - min + 1);  // Inclusive of max
}

void checkFruitCollision(Path* path, Fruit* fruits, int fruits_count, int* points) {
    if (path->head == NULL) return;

    Vector2 head_pos = path->head->pos;
    for (int i = 0; i < fruits_count; i++){
        if (fruits[i].active && CheckCollisionCircleRec(fruits[i].pos, 5.0, (Rectangle){head_pos.x, head_pos.y, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE})){
            fruits[i].active = false;
            (*points)++;
           addStepTail(path);

            fruits[i].pos = (Vector2){random_in_range(0,screenWidth), random_in_range(0, screenHeight)};
            fruits[i].color = RED;
            fruits[i].active = true;
            break;
        }
    }
}

int checkSnakeCollision(Path* path){
    Vector2 head_pos = path->head->pos;
    Rectangle head_rec = {head_pos.x, head_pos.y, 1, SNAKE_SEGMENT_SIZE};
    Step* current = path->head->next;
    
    while(current != NULL){
        if (CheckCollisionRecs((Rectangle){current->pos.x, current->pos.y, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE}, head_rec)){
                return -1;
        }         
        current = current->next;
    }
    return 0; 
}



void freePath(Path* path){
    if(path == NULL){
        printf(" The path is null on the freepath function");
    }
    Step* current = path->head;
    while (current != NULL){
        Step* temp = current; 
        current = current->next;
        free(temp);
    }
    path->head= NULL;
    path->tail= NULL;
    path->max_size = 0;
    path->size = 0;
}

void generateFruits(Fruit* fruits, int size){
    for (int i = 0; i < size; i++){
        fruits[i].pos = (Vector2){random_in_range(0,screenWidth), random_in_range(0, screenHeight)};
        fruits[i].color = RED;
        fruits[i].active = true;
    }
}
