
void SetupScreen(){
  u8g2.begin();
}

void ScreenTick(){

  String msg = "x: " + String(GlobalLastX) + ",y:" + String(GlobalLastY)  + ",Z:" + String(GlobalLastZ);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 20, msg.c_str());
  u8g2.sendBuffer();

}

