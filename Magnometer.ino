#include <Wire.h>

/* ================= PINS ================= */
#define MPU_ADDR 0x68

#define SDA_PIN_MAG 8 
#define SCL_PIN_MAG 9

#define MPU_PWR_MGMT_1 0x6B
#define MPU_ACCEL_XOUT_H 0x3B

// ================= Global Variables =================
int16_t GlobalLastX = 0;
int16_t GlobalLastY = 0;
int16_t GlobalLastZ = 0;

// velocity placeholders (optional)


// ================= TwoWire instance for MPU6050 =================
TwoWire I2C_MPU = TwoWire(1); // use hardware I2C port 1

// ================= MPU6050 Initialization =================
void setupAccelerometer() {
  // initialize hardware I2C
  I2C_MPU.begin(SDA_PIN_MAG, SCL_PIN_MAG);
  I2C_MPU.setClock(400000);  // 400 kHz fast I2C (use 100000 if unstable)

  // wake up MPU6050 (it starts in sleep mode)
  I2C_MPU.beginTransmission(MPU_ADDR);
  I2C_MPU.write(MPU_PWR_MGMT_1);
  I2C_MPU.write(0);
  I2C_MPU.endTransmission();
  delay(10); // small delay for MPU6050 to wake up

  Serial.println("MPU6050 Ready");
}

// ================= Read Accelerometer =================
bool readAccel(int16_t &ax, int16_t &ay, int16_t &az) {
  I2C_MPU.beginTransmission(MPU_ADDR);
  I2C_MPU.write(MPU_ACCEL_XOUT_H);
  if (I2C_MPU.endTransmission(false) != 0) return false;

  if (I2C_MPU.requestFrom(MPU_ADDR, 6) != 6) return false;

  ax = I2C_MPU.read() << 8 | I2C_MPU.read();
  ay = I2C_MPU.read() << 8 | I2C_MPU.read();
  az = I2C_MPU.read() << 8 | I2C_MPU.read();

  return true;
}

// ================= Tick =================
void AccelerometerTick() {
  if (readAccel(GlobalLastX, GlobalLastY, GlobalLastZ)) {
    float ax = GlobalLastX / 16384.0;
    float ay = GlobalLastY / 16384.0;
    float az = GlobalLastZ / 16384.0;

    Serial.print("AX: "); Serial.print(ax, 3);
    Serial.print(" AY: "); Serial.print(ay, 3);
    Serial.print(" AZ: "); Serial.println(az, 3);

    // optional: compute simple velocity mapping
    xVelocity = 0;
    yVelocity = 0;

    if (ax > 0.25) yVelocity = 1;
    if (ax < -0.25) yVelocity = -1;
    if (ay > 0.25) xVelocity = 1;
    if (ay < -0.25) xVelocity = -1;

  } else {
    Serial.println("Accel read failed");
  }
}