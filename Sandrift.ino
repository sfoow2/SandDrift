#include <Wire.h>
#include <U8g2lib.h>

/* ================= PINS ================= */
#define MPU_ADDR 0x68

#define SDA_PIN_MAG 8 
#define SCL_PIN_MAG 9

#define MPU_PWR_MGMT_1 0x6B
#define MPU_ACCEL_XOUT_H 0x3B

#define SDA_PIN_SCREEN 6
#define SCL_PIN_SCREEN 5

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(
  U8G2_R0, SCL_PIN_SCREEN, SDA_PIN_SCREEN, U8X8_PIN_NONE
);

int16_t GlobalLastX = 0;
int16_t GlobalLastY = 0;
int16_t GlobalLastZ = 0;


void setup()
{
  Serial.begin(115200);
  delay(1000);

  SetupScreen();
  setupAccelerometer();
}

uint8_t FrameCounter = 0;

void loop(){
  FrameCounter++;
  if (FrameCounter >= 5){
    AccelerometerTick();
    FrameCounter = 0;
  }
  ScreenTick();
  delay(50);
}

