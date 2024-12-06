#include <raylib.h>
#include <iostream>
#include <cmath>
using namespace std;

int winning_score;
int player_score = 0;
int cpu_score = 0;

bool game_over = false;
bool play_with_cpu = true;

const int default_ball_speed_x = 9;
const int default_ball_speed_y = 9;

bool CheckCollisionCircleCircle(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    float dx = center2.x - center1.x;
    float dy = center2.y - center1.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= (radius1 + radius2);
}

class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;
    bool speed_increased = false;
    bool speed_decreased = false;

    void Draw()
    {
        DrawCircle( x, y, radius, ORANGE);
    }

    void Update()
    {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
        }

        // Cpu wins
        if (x + radius >= GetScreenWidth())
        {
            cpu_score++;
            ResetBall();
        }

        //Player wins
        if (x - radius <= 0)
        {
            player_score++;
            ResetBall();
        }

        //game over 
        if ((player_score >= winning_score && player_score - cpu_score >= 2) || (cpu_score >= winning_score && cpu_score - player_score >= 2))
        {
            game_over = true;
        }

        CheckSpeedChange();
    }

    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = { -1, 1 };
        speed_x = default_ball_speed_x * speed_choices[GetRandomValue(0, 1)];
        speed_y = default_ball_speed_y * speed_choices[GetRandomValue(0, 1)];

        speed_increased = false;
        speed_decreased = false;
    }

    void CheckSpeedChange()
    {
        float positive_circle_x = GetScreenWidth() / 2;
        float positive_circle_y = 200;
        float negative_circle_x = GetScreenWidth() / 2;
        float negative_circle_y = 600;
        float circle_radius = 50;

        if (CheckCollisionCircleCircle({ x, y }, radius, { positive_circle_x, positive_circle_y }, circle_radius))
        {
            if (!speed_increased && !speed_decreased)
            {
                speed_x += (speed_x > 0) ? 2 : -2;
                speed_y += (speed_y > 0) ? 2 : -2;
                speed_increased = true;
            }
        }

        if (CheckCollisionCircleCircle({ x, y }, radius, { negative_circle_x, negative_circle_y }, circle_radius))
        {
            if (!speed_decreased && !speed_increased)
            {
                speed_x -= (speed_x > 0) ? 2 : -2;
                speed_y -= (speed_y > 0) ? 2 : -2;
                speed_decreased = true;
            }
        }
    }

    void ResetSpeed()
    {
        speed_x = (speed_x > 0) ? default_ball_speed_x : -default_ball_speed_x;
        speed_y = (speed_y > 0) ? default_ball_speed_y : -default_ball_speed_y;
        speed_increased = false;
        speed_decreased = false;
    }
};

class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.8, 0, WHITE);
    }

    virtual void Update() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
public:
    void Update(int ball_y) {
        if (y + height / 2 > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class Player2Paddle : public Paddle {
public:
    void Update() {
        if (IsKeyDown(KEY_T)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_F)) {
            y = y + speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddle Riyad;
Player2Paddle Sabbir;
CpuPaddle cpu;



int main() {
    cout << "Enter the winning score: ";
    cin >> winning_score;
    cout << "Play (Man Vs Computer) or (Man Vs Man) ? (1 for Computer, 2 for Man): ";
    int choice;
    cin >> choice;
    play_with_cpu = (choice == 1);

    cout << "Starting the game" << endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, " My Pong Game!   ( Group -- 22 )");
    SetTargetFPS(80);

    // Load sound and check
    InitAudioDevice(); 
    Sound paddleHitSound = LoadSound("music/magic.mp3");


    //About Ball
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = default_ball_speed_x;
    ball.speed_y = default_ball_speed_y;


    //Player1 paddle
    Riyad.width = 25;
    Riyad.height = 120;
    Riyad.x = screen_width - Riyad.width - 10;
    Riyad.y = screen_height / 2 - Riyad.height / 2;
    Riyad.speed = 8;

    //player2 paddle
    Sabbir.width = 25;
    Sabbir.height = 120;
    Sabbir.x = 10;
    Sabbir.y = screen_height / 2 - Sabbir.height / 2;
    Sabbir.speed = 8;

    //cpu paddle
    cpu.width = Sabbir.width;
    cpu.height = Sabbir.height;
    cpu.x = Sabbir.x;
    cpu.y = Sabbir.y;
    cpu.speed = Sabbir.speed;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        // Updating
        if (!game_over) {
            ball.Update();
            Riyad.Update();
            if (play_with_cpu) {
                cpu.Update(ball.y);
            }
            else {
                Sabbir.Update();
            }
        }

        // Checking for collisions
        if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { Riyad.x, Riyad.y, Riyad.width, Riyad.height })) {
            ball.speed_x *= -1;
            ball.ResetSpeed();
            PlaySound(paddleHitSound);  
        }

        if (play_with_cpu) {
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { cpu.x, cpu.y, cpu.width, cpu.height })) {
                ball.speed_x *= -1;
                ball.ResetSpeed();
                PlaySound(paddleHitSound);  
            }
        }
        else {
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { Sabbir.x, Sabbir.y, Sabbir.width, Sabbir.height })) {
                ball.speed_x *= -1;
                ball.ResetSpeed();
                PlaySound(paddleHitSound);  

            }
        }

        // Drawing
        ClearBackground( RED );
        DrawRectangle(5, 5, screen_width-10, screen_height-10, DARKBLUE);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        DrawCircle(screen_width / 2, 200, 50, GREEN );
        DrawCircle(screen_width / 2, 200, 45 , DARKBLUE);
        DrawText("+", screen_width / 2 - MeasureText("+", 40) / 2, 185, 40, BLACK);
        DrawCircle(screen_width / 2, 600, 50, RED );
        DrawCircle(screen_width / 2, 600, 45, DARKBLUE);
        DrawText("-", screen_width / 2 - MeasureText("-", 40) / 2, 585, 40, BLACK);

        ball.Draw();
        Riyad.Draw();
        if (play_with_cpu) {
            cpu.Draw();
        }
        else {
            Sabbir.Draw();
        }

        // Draw scores and names
        const int fontSize = 40;
        const int fontSizescore = 60;

        DrawText("Riyad", 3 * screen_width / 4 - MeasureText("Riyad", fontSize) / 2, 10, fontSize, WHITE);
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 60, fontSizescore, RED);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 60, fontSizescore, RED);

        if (!play_with_cpu) {
            DrawText("Sabbir", screen_width / 4 - MeasureText("Sabbir", fontSize) / 2, 10, fontSize, WHITE);
        }
        else
        {
            DrawText("CPU", screen_width / 4 - MeasureText("CPU", fontSize) / 2, 10, fontSize, WHITE);
        }
    
        // Display the winner if the game is over
        if (game_over) {
            const char* winner = (player_score >= winning_score && player_score - cpu_score >= 2) ? "Riyad Wins!" : "CPU Wins!";
            if (!play_with_cpu) {
                winner = (player_score >= winning_score && player_score - cpu_score >= 2) ? "Riyad Wins!" : "Sabbir Wins!";
            }
            DrawText(winner, screen_width / 2 - MeasureText(winner, 80) / 2, screen_height / 2 - 40, 80, RED);
        }
        EndDrawing();
    }
    UnloadSound(paddleHitSound);  // Unload sound from memory
    CloseAudioDevice();           // Close the audio device
    CloseWindow();
    return 0;
}
