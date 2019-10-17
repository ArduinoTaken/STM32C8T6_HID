#include <USBComposite.h>
#include "commandAndResponse.h"
/*
 * This example types which cardinal direction the mouse will be moved
 * and then moves the mouse in that direction. If the mouse doesn't recenter
 * at the end, it is probably due to mouse acceleration 
 */

USBHID HID;
HIDKeyboard Keyboard(HID);
HIDMouse Mouse(HID); 
USBCompositeSerial usbcSerial;
String comdata="";
bool _is_busy = false;
bool _led_status = false;
int _loop_count = 0;
int cmdCount = 0;
void setup() {
  pinMode(PC13,OUTPUT);
  HID.begin(usbcSerial,HID_KEYBOARD_MOUSE);
  Keyboard.begin();
  Mouse.begin();
  Serial1.begin(9600);
  cmdCount = sizeof(cmdFeedbackArr) /sizeof(cmdFeedbackArr[0]);
  delay(2000);
}

void loop() {
  _loop_count += 1;
  if(_loop_count > 20){
    _loop_count = 0;
    _led_status = !_led_status;
    digitalWrite(PC13,_led_status);
  }
  receiveCom();
  receiveCom1();
  delay(10);
  
}
void receiveCom()
{
  comdata = "";
  while (usbcSerial.available() > 0)
  {
    comdata += char(usbcSerial.read());
    delay(2);
  }
  if(_is_busy) return;
  if (comdata != "")
  {
    respondAction();
  }
}
void receiveCom1()
{
  comdata = "";
  while (Serial1.available() > 0)
  {
    comdata += char(Serial1.read());
    delay(2);
  }
  if(_is_busy) return;
  if (comdata != "")
  {
    respondAction();
  }
}
void respondAction()
{
  _is_busy = true;
  comdata.trim();
  comdata.replace("\r","");
  comdata.replace("\n","");
  
  //check data complete
  if(comdata.length() < 3) {
    usbcSerial.println("ERROR");
    Serial1.println("ERROR");
    _is_busy = false;
    return;
  }
  //mouse move
  if(comdata.startsWith("M:")){
    //M:100,200\r\n ritht:100 down:200
    String cmd = comdata.substring(2);
    int location = cmd.indexOf(',');
    int move_x = cmd.substring(0,location).toInt();
    int move_y = cmd.substring(location+1).toInt();
    Mouse.move(move_x,0);
    delay(10);
    Mouse.move(0,move_y);
    delay(10);
    usbcSerial.println("OK,"+comdata);
    Serial1.println("OK,"+comdata);
  }
  //mouse click
  else if(comdata.startsWith("C:")){
    String cmd = comdata.substring(2);
    if(cmd == "L"){
      Mouse.click(MOUSE_LEFT);
    }else{
      Mouse.click(MOUSE_RIGHT);
    }
    delay(10);
    usbcSerial.println("OK,"+comdata);
    Serial1.println("OK,"+comdata);
  }
  //keyboard input
  else if(comdata.startsWith("K:")){
    //K:Hello,world!
    String cmd = comdata.substring(2);
    Keyboard.print(cmd);
    delay(10);
    usbcSerial.println("OK,"+comdata);
    Serial1.println("OK,"+comdata);
  }
  //press "ESC" ->OK
  else if(comdata == "ESC"){
    delay(10);
    Keyboard.press(KEY_ESC);
    delay(5);
    Keyboard.release(KEY_ESC);
    delay(10);
    usbcSerial.println("OK,"+comdata);
    Serial1.println("OK,"+comdata);
  }
  else if(comdata == "Ready"){
    delay(10);
    usbcSerial.println("OK,"+comdata);
    Serial1.println("OK,"+comdata);
  }
  else if(comdata == "help"){
    delay(50);
    usbcSerial.println("ESC->OK");
    usbcSerial.println("Ready->OK");
    usbcSerial.println("M:100,20->OK");
    usbcSerial.println("C:L->OK");
    usbcSerial.println("K:Hello,world!");
    delay(10);
    Serial1.println("ESC->OK");
    Serial1.println("Ready->OK");
    Serial1.println("M:100,20->OK");
    Serial1.println("C:L->OK");
    Serial1.println("K:Hello,world!");
    printCommandList();
  }
  else{
    for (int i = 0; i < cmdCount; i++)
    {
      int location = cmdFeedbackArr[i].indexOf(';');
      String cmd = cmdFeedbackArr[i].substring(0, location);
      if (comdata == cmd)
      {
        String feedback = cmdFeedbackArr[i].substring(location + 1, cmdFeedbackArr[i].length());
        delay(10);
        if (feedback != "")
          usbcSerial.println(feedback);
          Serial1.println(feedback);
        break;
      }
    }
  }
  
  _is_busy = false;
}

void printCommandList()
{
  for (int i = 0; i < cmdCount; i++)
  {
    usbcSerial.println(cmdFeedbackArr[i]);
    Serial1.println(cmdFeedbackArr[i]);
    delay(5);
  }
}
