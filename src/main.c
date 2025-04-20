#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define SIZE 39       // 表盘大小（必须为奇数）
#define CENTER_X (SIZE / 2)
#define CENTER_Y (SIZE / 2)
#define RADIUS (SIZE / 2 - 1)

volatile sig_atomic_t running = 1;

void handle_interrupt(int sig) {
    running = 0;
}

// 绘制一个点到二维数组中
void plot(char screen[SIZE][SIZE], int x, int y, char c) {
    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
        screen[y][x] = c;
    }
}

// Bresenham 圆形算法：只画外圈，不填充内部
void draw_circle(char screen[SIZE][SIZE], int cx, int cy, int r, char c) {
    int x = 0, y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        plot(screen, cx + x, cy + y, c);
        plot(screen, cx + y, cy + x, c);
        plot(screen, cx - x, cy + y, c);
        plot(screen, cx - y, cy + x, c);
        plot(screen, cx + x, cy - y, c);
        plot(screen, cx + y, cy - x, c);
        plot(screen, cx - x, cy - y, c);
        plot(screen, cx - y, cy - x, c);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Bresenham 直线算法：绘制从中心出发的直线（用于指针）
void draw_line(char screen[SIZE][SIZE], int x0, int y0, int x1, int y1, char c) {
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        if (x0 != CENTER_X || y0 != CENTER_Y) {
            plot(screen, x0, y0, c);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// 画指针函数（从中心出发）
void draw_hand(char screen[SIZE][SIZE], double angle, int length, char c) {
    int x = (int)(length * cos(angle));
    int y = (int)(length * sin(angle));
    draw_line(screen, CENTER_X, CENTER_Y, CENTER_X + x, CENTER_Y + y, c);
}

// 在指定角度画数字刻度（支持多位数）
void draw_tick(char screen[SIZE][SIZE], const char* tick_label, double angle, int radius) {
    int label_length = strlen(tick_label);
    double radian_offset = 0.0;

    // 计算每个字符的偏移量，实现居中对齐
    for (int i = 0; i < label_length; ++i) {
        // 每个字符沿圆弧分布
        double offset_angle = angle + (label_length / 2 - i) * 0.05;  // 小角度偏移
        int x = (int)((radius) * cos(offset_angle)) + CENTER_X;
        int y = (int)((radius) * sin(offset_angle)) + CENTER_Y;

        if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
            screen[y][x] = tick_label[i];
        }
    }
}

// 清空屏幕
void clear_screen() {
    printf("\033[H\033[J");
}

// 打印整个屏幕
void print_screen(char screen[SIZE][SIZE]) {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            printf("%c", screen[y][x]);
        }
        printf("\n");
    }
}

// 获取当前时间并绘制时钟
void draw_clock() {
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    char screen[SIZE][SIZE];
    memset(screen, ' ', sizeof(screen));

    // 画表盘
    draw_circle(screen, CENTER_X, CENTER_Y, RADIUS, '@');

    // 画刻度数字 1~12
    for (int i = 1; i <= 12; ++i) {
        char buffer[3]; // 支持两位数
        snprintf(buffer, sizeof(buffer), "%d", i);
        double angle = (i * 30.0 - 90.0) * M_PI / 180.0;  // 调整起点为顶部
        draw_tick(screen, buffer, angle, RADIUS - 2);     // 留出边距
    }

    // 计算角度（弧度制），顺时针偏移 M_PI/2
    double hour_angle = ((time_info->tm_hour % 12) * 30.0 + time_info->tm_min * 0.5) * M_PI / 180.0;
    double min_angle = (time_info->tm_min * 6.0) * M_PI / 180.0;
    double sec_angle = (time_info->tm_sec * 6.0) * M_PI / 180.0;

    // 调整方向为顺时针起始于顶部
    hour_angle -= M_PI / 2;
    min_angle -= M_PI / 2;
    sec_angle -= M_PI / 2;

    // 画指针（注意圆心不要被覆盖）
    draw_hand(screen, hour_angle, RADIUS * 0.4, 'O'); // 时针
    draw_hand(screen, min_angle, RADIUS * 0.6, 'o');  // 分针
    draw_hand(screen, sec_angle, RADIUS * 0.8, '.');  // 秒针

    // 打印表盘
    clear_screen();
    print_screen(screen);

    // 显示日期和时间信息
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", time_info);
    printf("%s\n", buffer);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("Usage: showtime\n");
            printf("A simple ASCII analog clock in terminal.\n");
            return 0;
        } else if (strcmp(argv[1], "--version") == 0) {
            printf("showtime version 1.0\n");
            return 0;
        }
    }

    signal(SIGINT, handle_interrupt); // Ctrl+C 退出

    while (running) {
        draw_clock();
        usleep(1000000); // 每秒更新一次
    }

    return 0;
}
