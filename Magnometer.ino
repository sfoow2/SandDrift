
void setupAccelerometer()
{
  Wire.begin(SDA_PIN_MAG, SCL_PIN_MAG);
  Wire.setClock(100000);

  // wake up MPU6050 (it starts asleep)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission();

  Serial.println("MPU6050 Ready");
}

void AccelerometerTick()
{
  if (readAccel(GlobalLastX, GlobalLastY, GlobalLastZ))
  {
    // convert to g force (optional but useful)
    float ax = GlobalLastX / 16384.0;
    float ay = GlobalLastY / 16384.0;
    float az = GlobalLastZ / 16384.0;

    Serial.print("AX: "); Serial.print(ax,3);
    Serial.print(" AY: "); Serial.print(ay,3);
    Serial.print(" AZ: "); Serial.println(az,3);
  }
  else
  {
    Serial.println("Accel read failed");
  }
}


bool readAccel(int16_t &ax, int16_t &ay, int16_t &az)
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom(MPU_ADDR, 6) != 6)
    return false;

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();

  return true;
}
