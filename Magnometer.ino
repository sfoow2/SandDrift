
//Ic2 stuff

void writeRegister(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(AK09911_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg)
{
  Wire.beginTransmission(AK09911_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(AK09911_ADDR, 1);
  return Wire.read();
}

void triggerMeasurement()
{
  writeRegister(AK09911_CNTL2, 0x01);
}

// read XYZ and clear latch via ST2
bool readMag(int16_t &x, int16_t &y, int16_t &z)
{
  Wire.beginTransmission(AK09911_ADDR);
  Wire.write(AK09911_HXL);
  Wire.endTransmission(false);

  // 6 data bytes + ST2 unlock byte
  if (Wire.requestFrom(AK09911_ADDR, 7) != 7)
    return false;

  uint8_t xL = Wire.read();
  uint8_t xH = Wire.read();
  uint8_t yL = Wire.read();
  uint8_t yH = Wire.read();
  uint8_t zL = Wire.read();
  uint8_t zH = Wire.read();
  uint8_t ST2 = Wire.read();   // REQUIRED — clears sensor lock

  // overflow check
  if (ST2 & 0x08)
    return false;

  x = (int16_t)(xH << 8 | xL);
  y = (int16_t)(yH << 8 | yL);
  z = (int16_t)(zH << 8 | zL);

  return true;
}


void setupMagnometer(){
// hardware reset
  pinMode(RST_PIN_MAG, OUTPUT);
  digitalWrite(RST_PIN_MAG, LOW);
  delay(20);
  digitalWrite(RST_PIN_MAG, HIGH);
  delay(20);

  Wire.begin(SDA_PIN_MAG, SCL_PIN_MAG);
  Wire.setClock(100000);   // IMPORTANT: 100kHz, NOT 400kHz

  // verify chip
  uint8_t id = readRegister(AK09911_WIA2);

  Serial.print("Device ID: 0x");
  Serial.println(id, HEX);

  if (id != 0x05)
  {
    Serial.println("AK09911 NOT DETECTED");
    while(true);
  }

  Serial.println("AK09911 Ready");


}

void MangometerTick(){
  triggerMeasurement();

  // sensor conversion time (~10ms)
  delay(12);

  if (readMag(GlobalLastX, GlobalLastY, GlobalLastZ))
  {
    // heading calculation (yaw)
    float heading = atan2((float)GlobalLastY, (float)GlobalLastX) * 180.0 / PI;
    if (heading < 0) heading += 360.0;

    Serial.print("X: "); Serial.print(GlobalLastX);
    Serial.print(" Y: "); Serial.print(GlobalLastY);
    Serial.print(" Z: "); Serial.print(GlobalLastZ);
    Serial.print(" Heading: "); Serial.println(heading);
  }
  else
  {
    Serial.println("Read failed");
  }
}
