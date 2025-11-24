#include <graphics.h>// uses graphic file
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define LANES 3
#define MAX_OBS 20
#define HIGH_SCORE_FILE "highscores.txt"

typedef struct {
    int lane;
    int y;
    int active;
} Obstacle;

// Global colors for theme
int carColor = YELLOW;
int obsColor = RED;

// Function prototypes
void drawCar(int lane);
void drawObstacle(Obstacle o);
void showHighScores();
void selectTheme();
void saveHighScore(int score);
int startGame();

void drawCar(int lane) {
    int laneWidth = 120;
    int carWidth = 40, carHeight = 50;
    int x = 160 + lane * laneWidth + (laneWidth - carWidth)/2;
    int y = 400;

    // Car main body 
    setcolor(carColor);
    setfillstyle(SOLID_FILL, carColor);
    bar(x, y + 10, x + carWidth, y + carHeight);
    bar(x + 10, y, x + carWidth - 10, y + 10);

    // Windows
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(x + 10, y + 18, x + carWidth - 10, y + 35);

    // Wheels
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    bar(x + 3, y + carHeight - 10, x + 10, y + carHeight);
    bar(x + carWidth - 10, y + carHeight - 10, x + carWidth - 3, y + carHeight);
}

void drawObstacle(Obstacle o) {
    int laneWidth = 120;
    int carWidth = 45, carHeight = 60;
    int x = 160 + o.lane * laneWidth + (laneWidth - carWidth) / 2;
    int y = o.y;

    // Main body
    setcolor(obsColor);
    setfillstyle(SOLID_FILL, obsColor);
    bar(x, y + 10, x + carWidth, y + carHeight);

    // Hood (top part)
    setfillstyle(SOLID_FILL, obsColor);
    bar(x + 8, y, x + carWidth - 8, y + 15);

    // Windows
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(x + 10, y + 18, x + carWidth - 10, y + 35);

    // Wheels
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    bar(x + 5, y + carHeight - 10, x + 15, y + carHeight);         // Left wheel
    bar(x + carWidth - 15, y + carHeight - 10, x + carWidth - 5, y + carHeight); // Right wheel
}



void saveHighScore(int score) {
    int scores[100], count = 0;

    FILE *f = fopen(HIGH_SCORE_FILE, "r");
    if (f) {
        while (fscanf(f, "%d", &scores[count]) == 1 && count < 100) {
            count++;
        }
        fclose(f);
    }

    // Add new score
    scores[count++] = score;

    // Sort scores in descending order
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    // Keep only top 5
    if (count > 5) count = 5;

    // Write back top 5 scores
    f = fopen(HIGH_SCORE_FILE, "w");
    for (int i = 0; i < count; i++) {
        fprintf(f, "%d\n", scores[i]);
    }
    fclose(f);
}

void showHighScores() {
    FILE *f = fopen(HIGH_SCORE_FILE, "r");
    int score;
    int yPos = 80;

    cleardevice();
    setcolor(WHITE);
    outtextxy(200, 20, "=== TOP 5 HIGH SCORES ===");

    if (f) {
        int rank = 1;
        while (fscanf(f, "%d", &score) == 1 && rank <= 5) {
            char buf[50];
            sprintf(buf, "%d. %d", rank, score);
            outtextxy(200, yPos, buf);
            yPos += 30;
            rank++;
        }
        fclose(f);
    } else {
        outtextxy(200, 80, "No high scores yet!");
    }

    outtextxy(150, yPos + 40, "Press any key to return to menu...");
    getch();
}


void selectTheme() {
    cleardevice();
    outtextxy(100, 100, "Select Car Color:");
    outtextxy(120, 150, "1. Yellow");
    outtextxy(120, 180, "2. Green");
    outtextxy(120, 210, "3. Blue");
    char c = getch();
    if (c == '1') carColor = YELLOW;
    else if (c == '2') carColor = GREEN;
    else if (c == '3') carColor = BLUE;
    obsColor = RED; // obstacles remain red
}

int startGame() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    srand(time(0));

    int carLane = 1;
    int score = 0;
    int speed = 5;
    int spawnGap = 40;

    Obstacle obs[MAX_OBS];
    for (int i = 0; i < MAX_OBS; i++) obs[i].active = 0;

    int frameCount = 0;
    int laneY[3] = {0, 0, 0};  // For animating lane lines

    while (1) {
        cleardevice();

        // Animate lane lines
        setcolor(WHITE);
        int laneX[4] = {160, 280, 400, 520};
        for (int i = 0; i < 4; i++) {
            laneY[i] += speed; 
            if (laneY[i] > 40) laneY[i] = 0;
            for (int y = laneY[i]; y < 480; y += 40) {
                line(laneX[i], y, laneX[i], y + 20);  // dashed line
            }
        }

        drawCar(carLane);

        // Spawn obstacles
        frameCount++;
        if (frameCount >= spawnGap) {
            frameCount = 0;
            for (int i = 0; i < MAX_OBS; i++) {
                if (!obs[i].active) {
                    obs[i].active = 1;
                    obs[i].lane = rand() % LANES;
                    obs[i].y = 0;
                    break;
                }
            }
        }

        // Move & draw obstacles
        for (int i = 0; i < MAX_OBS; i++) {
            if (obs[i].active) {
                obs[i].y += speed;
                drawObstacle(obs[i]);

                if (obs[i].lane == carLane && obs[i].y + 40 >= 400 && obs[i].y <= 450) {
                    setcolor(RED);
                    outtextxy(200, 200, "GAME OVER!");
                    getch();
                    closegraph();
                    saveHighScore(score);
                    return score;
                }

                if (obs[i].y > 480) {
                    obs[i].active = 0;
                    score++;
                    if (score < 40) speed = 5 + score / 10;
                }
            }
        }
        

        char buf[50];
        setcolor(WHITE);
        sprintf(buf, "Score: %d", score);
        outtextxy(10, 10, buf);

        // Controls
        if (kbhit()) {
            char key = getch();
            if (key == 'a' && carLane > 0) carLane--;
            if (key == 'd' && carLane < 2) carLane++;
            if (key == 'q') break;
        }

        delay(40);
    }

    closegraph();
    return score;
}


int main() {
    int choice;
    while (1) {
        printf("\n--- 3-LANE CAR GAME ---\n");
        printf("1. Start Game\n");
        printf("2. High Scores / Change Theme\n");
        printf("3. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 1) startGame();
        else if (choice == 2) {
            int sub;
            printf("1. View High Scores\n2. Change Theme\nChoice: ");
            scanf("%d", &sub);
            getchar();
            int gd = DETECT, gm;
            initgraph(&gd, &gm, "");
            if (sub == 1) showHighScores();
            else if (sub == 2) selectTheme();
            closegraph();
        } else if (choice == 3) break;
        else printf("Invalid choice!\n");
    }
    return 0;
}


