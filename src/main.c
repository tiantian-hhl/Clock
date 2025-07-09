#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>


ssadsdaffewg
#include <time.h>

#define SIZE 39
#define CENTER_X (SIZE / 2)
#define CENTER_Y (SIZE / 2)
#define RADIUS (SIZE / 2 - 1)

#define COLOR_RESET   "\033[0m"
#define COLOR_HOUR    "\033[34m"  // Blue
#define COLOR_MIN     "\033[32m"  // Green
#define COLOR_SEC     "\033[31m"  // Red

volatile sig_atomic_t running = 1;

void handle_interrupt(int sig) {
    running = 0;
}

void move_cursor_top() {
    printf("\033[2J\033[H"); // clear screen and move cursor to top
}

void plot(char screen[SIZE][SIZE][16], int x, int y, const char* color, char c) {
    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
        snprintf(screen[y][x], 16, "%s%c%s", color, c, COLOR_RESET);
    }
}

void draw_circle(char screen[SIZE][SIZE][16], int cx, int cy, int r, char c) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        plot(screen, cx + x, cy + y, COLOR_RESET, c);
        plot(screen, cx + y, cy + x, COLOR_RESET, c);
        plot(screen, cx - x, cy + y, COLOR_RESET, c);
        plot(screen, cx - y, cy + x, COLOR_RESET, c);
        plot(screen, cx + x, cy - y, COLOR_RESET, c);
        plot(screen, cx + y, cy - x, COLOR_RESET, c);
        plot(screen, cx - x, cy - y, COLOR_RESET, c);
        plot(screen, cx - y, cy - x, COLOR_RESET, c);
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void draw_line(char screen[SIZE][SIZE][16], int x0, int y0, int x1, int y1, const char* color, char c) {
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        if (x0 != CENTER_X || y0 != CENTER_Y) {
            plot(screen, x0, y0, color, c);
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_hand(char screen[SIZE][SIZE][16], double angle, int length, const char* color, char c) {
    int x = (int)(length * cos(angle));
    int y = (int)(length * sin(angle));
    draw_line(screen, CENTER_X, CENTER_Y, CENTER_X + x, CENTER_Y + y, color, c);
}

void draw_tick(char screen[SIZE][SIZE][16], const char* tick_label, double angle, int radius) {
    int label_length = strlen(tick_label);
    int x = (int)(radius * cos(angle)) + CENTER_X - label_length / 2;
    int y = (int)(radius * sin(angle)) + CENTER_Y;

    for (int i = 0; i < label_length; ++i) {
        plot(screen, x + i, y, COLOR_RESET, tick_label[i]);
    }
}

void draw_center(char screen[SIZE][SIZE][16]) {
    plot(screen, CENTER_X, CENTER_Y, COLOR_RESET, '+');
}

void print_screen(char screen[SIZE][SIZE][16]) {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            printf("%s", screen[y][x]);
        }
        putchar('\n');
    }
}

void draw_clock() {
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    char screen[SIZE][SIZE][16];
    for (int y = 0; y < SIZE; ++y)
        for (int x = 0; x < SIZE; ++x)
            snprintf(screen[y][x], 16, " ");

    draw_circle(screen, CENTER_X, CENTER_Y, RADIUS, '@');

    for (int i = 1; i <= 12; ++i) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%2d", i);
        double angle = (i * 30.0 - 90.0) * M_PI / 180.0;
        draw_tick(screen, buffer, angle, RADIUS - 2);
    }

    double hour_angle = ((time_info->tm_hour % 12) * 30.0 + time_info->tm_min * 0.5) * M_PI / 180.0;
    double min_angle = (time_info->tm_min * 6.0) * M_PI / 180.0;
    double sec_angle = (time_info->tm_sec * 6.0) * M_PI / 180.0;

    hour_angle -= M_PI / 2;
    min_angle -= M_PI / 2;
    sec_angle -= M_PI / 2;

    draw_hand(screen, hour_angle, (int)(RADIUS * 0.4), COLOR_HOUR, 'O');
    draw_hand(screen, min_angle, (int)(RADIUS * 0.6), COLOR_MIN, 'o');
    draw_hand(screen, sec_angle, (int)(RADIUS * 0.8), COLOR_SEC, '.');

    draw_center(screen);

    move_cursor_top();
    print_screen(screen);

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", time_info);
    printf("%s\n", buffer);
    fflush(stdout);
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

    signal(SIGINT, handle_interrupt);

    printf("\033[?25l"); // hide cursor
    fflush(stdout);

    while (running) {
        draw_clock();
        sleep(1);
    }

    printf("\033[?25h"); // show cursor
    printf("\033[%d;0H", SIZE + 2);
    printf("Exiting showtime. Goodbye!\n");
    fflush(stdout);

    return 0;
}

