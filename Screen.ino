#include <Wire.h>
#include <U8g2lib.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define SANDAMOUNT 500

int8_t xVelocity = 0;
int8_t yVelocity = -1;

struct Sand{
    uint8_t x;
    uint8_t y;
};

Sand ListOFSand[SANDAMOUNT];

uint8_t screenBuffer[SCREEN_W * SCREEN_H / 8];

// ================= Bit-packed Pixel Functions =================
inline void setPixel(int x, int y) {
    if ((unsigned)x >= SCREEN_W || (unsigned)y >= SCREEN_H) return;
    uint16_t index = x + ((y >> 3) * SCREEN_W);
    screenBuffer[index] |= (1 << (y & 7));
}

inline void clearPixel(int x, int y) {
    if ((unsigned)x >= SCREEN_W || (unsigned)y >= SCREEN_H) return;
    uint16_t index = x + ((y >> 3) * SCREEN_W);
    screenBuffer[index] &= ~(1 << (y & 7));
}

inline bool getPixel(int x, int y) {
    if ((unsigned)x >= SCREEN_W || (unsigned)y >= SCREEN_H) return false;
    uint16_t index = x + ((y >> 3) * SCREEN_W);
    return screenBuffer[index] & (1 << (y & 7));
}

inline void clearScreenBuffer() {
    memset(screenBuffer, 0, sizeof(screenBuffer));
}

// ================= Push buffer to OLED =================
void pushBufferToDisplay() {
    u8g2.clearBuffer();
    for (int y = 0; y < SCREEN_H; y++) {
        for (int x = 0; x < SCREEN_W; x++) {
            if (getPixel(x, y)) u8g2.drawPixel(x, y);
        }
    }
    u8g2.sendBuffer();
}

// ================= Setup and Loop =================
void SetupScreen() {
    u8g2.begin();
    randomSeed(esp_random());  // True hardware random seed

    // Initialize sand positions randomly at the top
    clearScreenBuffer();
    for (int i = 0; i < SANDAMOUNT; i++) {
        ListOFSand[i].x = random(0, SCREEN_W);
        ListOFSand[i].y = random(0, SCREEN_H);  // top 8 rows
        setPixel(ListOFSand[i].x, ListOFSand[i].y);
    }
}

void ScreenTick() {
    pushBufferToDisplay();
}
