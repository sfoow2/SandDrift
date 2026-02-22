#include <Wire.h>
#include <U8g2lib.h>

#define SCREEN_W 128
#define SCREEN_H 64
#define SANDAMOUNT 2500

struct Sand {
    uint8_t x;
    uint8_t y;
};

Sand ListOFSand[SANDAMOUNT];

uint8_t screenBuffer[SCREEN_W * SCREEN_H / 8];

// ================= Fast Pixel Macros =================
#define PIXEL_INDEX(x,y) ((x) + (((y) >> 3) * SCREEN_W))
#define PIXEL_MASK(y) (1 << ((y) & 7))

// ================= Fast RNG (XORShift) =================
uint32_t rngState = 1234567;

inline uint32_t fastRand() {
    rngState ^= rngState << 13;
    rngState ^= rngState >> 17;
    rngState ^= rngState << 5;
    return rngState;
}

// ================= Screen Buffer =================
inline void clearScreenBuffer() {
    memset(screenBuffer, 0, sizeof(screenBuffer));
}

// ================= Fast Display Push =================
inline void pushBufferToDisplay() {
    u8g2.clearBuffer();
    memcpy(u8g2.getBufferPtr(), screenBuffer, sizeof(screenBuffer));
    u8g2.sendBuffer();
}

// ================= Setup =================
void SetupScreen() {

    Wire.begin(SDA_PIN_SCREEN, SCL_PIN_SCREEN);
    Wire.setClock(1000000);
    u8g2.begin();

    clearScreenBuffer();

    // Seed fast RNG
    rngState = esp_random();

    for (int i = 0; i < SANDAMOUNT; i++) {

        bool placed = false;

        for (int attempts = 0; attempts < 100 && !placed; attempts++) {

            uint8_t x = fastRand() % SCREEN_W;
            uint8_t y = fastRand() % SCREEN_H;

            uint16_t idx = PIXEL_INDEX(x, y);
            uint8_t mask = PIXEL_MASK(y);

            if (!(screenBuffer[idx] & mask)) {

                ListOFSand[i].x = x;
                ListOFSand[i].y = y;
                screenBuffer[idx] |= mask;
                placed = true;
            }
        }

        if (!placed) {
            break; // screen too full
        }
    }
}

// ================= Main Tick =================
void ScreenTick() {
    for (int x = 0; x < floor(pow(abs(xVelocity) + abs(yVelocity),1.2)) + 1; x++){
        DoSandTick();
    }
    pushBufferToDisplay();
}

// ================= Sand Logic =================
void DoSandTick()
{
    if (xVelocity == 0 && yVelocity == 0){ 
        return;
    }

    for (int s = 0; s < SANDAMOUNT; s++)
    {
        uint8_t x = ListOFSand[s].x;
        uint8_t y = ListOFSand[s].y;

        int nx = x + xVelocity;
        int ny = y + yVelocity;

        // ===== 1. Straight Move =====
        if ((unsigned)nx < SCREEN_W && (unsigned)ny < SCREEN_H)
        {
            uint16_t nIdx = PIXEL_INDEX(nx, ny);
            uint8_t nMask = PIXEL_MASK(ny);

            if (!(screenBuffer[nIdx] & nMask))
            {
                uint16_t cIdx = PIXEL_INDEX(x, y);
                screenBuffer[cIdx] &= ~PIXEL_MASK(y);

                ListOFSand[s].x = nx;
                ListOFSand[s].y = ny;
                screenBuffer[nIdx] |= nMask;
                continue;
            }
        }

        // ===== 2. Proper Diagonals (gravity-aware) =====
        bool flip = fastRand() & 1;

        int dx1, dy1;
        int dx2, dy2;

        if (yVelocity != 0)
        {
            // vertical gravity (falling up/down)
            dx1 = -1; dy1 = yVelocity;
            dx2 =  1; dy2 = yVelocity;
        }
        else
        {
            // horizontal gravity (falling left/right)
            dx1 = xVelocity; dy1 = -1;
            dx2 = xVelocity; dy2 =  1;
        }

        // randomize order to avoid patterns
        if (flip)
        {
            int t;
            t = dx1; dx1 = dx2; dx2 = t;
            t = dy1; dy1 = dy2; dy2 = t;
        }

        // ----- First diagonal -----
        int tx = x + dx1;
        int ty = y + dy1;

        if ((unsigned)tx < SCREEN_W && (unsigned)ty < SCREEN_H)
        {
            uint16_t idx = PIXEL_INDEX(tx, ty);
            uint8_t mask = PIXEL_MASK(ty);

            if (!(screenBuffer[idx] & mask))
            {
                uint16_t cIdx = PIXEL_INDEX(x, y);
                screenBuffer[cIdx] &= ~PIXEL_MASK(y);

                ListOFSand[s].x = tx;
                ListOFSand[s].y = ty;
                screenBuffer[idx] |= mask;
                continue;
            }
        }

        // ----- Second diagonal -----
        tx = x + dx2;
        ty = y + dy2;

        if ((unsigned)tx < SCREEN_W && (unsigned)ty < SCREEN_H)
        {
            uint16_t idx = PIXEL_INDEX(tx, ty);
            uint8_t mask = PIXEL_MASK(ty);

            if (!(screenBuffer[idx] & mask))
            {
                uint16_t cIdx = PIXEL_INDEX(x, y);
                screenBuffer[cIdx] &= ~PIXEL_MASK(y);

                ListOFSand[s].x = tx;
                ListOFSand[s].y = ty;
                screenBuffer[idx] |= mask;
            }
        }
    }
}