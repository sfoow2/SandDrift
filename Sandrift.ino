#include <Wire.h>
#include <U8g2lib.h>

/* ================= PINS ================= */
#define SDA_PIN_MAG 8
#define SCL_PIN_MAG 9
#define RST_PIN_MAG 7

#define SDA_PIN_SCREEN 6
#define SCL_PIN_SCREEN 5

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(
  U8G2_R0, SCL_PIN_SCREEN, SDA_PIN_SCREEN, U8X8_PIN_NONE
);

/* =============== AK09911 ================= */
#define AK09911_ADDR 0x0C

#define AK09911_WIA2  0x01
#define AK09911_ST1   0x10
#define AK09911_HXL   0x11
#define AK09911_ST2   0x18
#define AK09911_CNTL2 0x31

int16_t GlobalLastX = 0;
int16_t GlobalLastY = 0;
int16_t GlobalLastZ = 0;


void setup()
{
  Serial.begin(115200);
  delay(1000);

  SetupScreen();

  setupMagnometer();
}


void loop(){
  MangometerTick();
  ScreenTick();
  delay(100);
}

