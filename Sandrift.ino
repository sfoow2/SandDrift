#include <Wire.h>
#include <U8g2lib.h>

// Pin definitions
#define SDA_PIN_SCREEN 6
#define SCL_PIN_SCREEN 5

int8_t xVelocity = 0;
int8_t yVelocity = 0;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0, U8X8_PIN_NONE
);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  SetupScreen();
  setupAccelerometer();
}

void loop(){
  AccelerometerTick();
  ScreenTick();
}