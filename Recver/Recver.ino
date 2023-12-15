/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

#include "sbus.h"
#include "WiFi.h"
#include <esp_now.h>

struct chan_adc_val_stru{
  uint16_t ch0;
  uint16_t ch1;
  uint16_t ch2;
  uint16_t ch3;
  uint16_t ch4;
  uint16_t ch5;
  uint16_t ch6;
  uint16_t ch7;
  uint16_t ch8;
};

chan_adc_val_stru chan_adc_val;


bfs::SbusTx sbus_tx(&Serial2, 16, 17, false, false);
bfs::SbusData data;

#define SIGNAL_TIMEOUT 3000  // This is signal timeout in milli seconds. We will reset the data if no signal

unsigned long lastRecvTime = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  if (len == 0)
  {
    return;
  }

  uint16_t receiverData = 0;

  memcpy(&chan_adc_val, incomingData, sizeof(chan_adc_val));

  int16_t ch0_val = map(chan_adc_val.ch0, 0, 4095, 172, 1811);

  Serial.println(chan_adc_val.ch0); 
  Serial.println(ch0_val); 

  data.ch[0] = map(chan_adc_val.ch0, 0, 4095, 172, 1811);
  data.ch[1] = map(chan_adc_val.ch1, 0, 4095, 172, 1811);
  data.ch[2] = map(chan_adc_val.ch2, 0, 4095, 172, 1811);
  data.ch[3] = map(chan_adc_val.ch3, 0, 4095, 172, 1811);
  data.ch[4] = map(chan_adc_val.ch4, 0, 4095, 172, 1811);
  data.ch[5] = map(chan_adc_val.ch5, 0, 4095, 172, 1811);
  data.ch[6] = map(chan_adc_val.ch6, 0, 4095, 172, 1811);
  data.ch[7] = map(chan_adc_val.ch7, 0, 4095, 172, 1811);
  data.ch[8] = map(chan_adc_val.ch8, 0, 4095, 172, 1811);

  sbus_tx.data(data);
  /* Write the data to the servos */
  sbus_tx.Write();
      
  lastRecvTime = millis(); 
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);
  // board1: 54:43:B2:B7:5C:20
  // board2: D4:D4:DA:72:37:60
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  sbus_tx.Begin();


  data.failsafe = false;
  data.ch17 = true;
  data.ch18 = true;
  data.lost_frame = false;


  for (int i = 500; i > 172; i--) {
    for (int j = 0; j < 16; j++) {
      data.ch[j] = i;
    }
    sbus_tx.data(data);
    sbus_tx.Write();
  }

  esp_now_register_recv_cb(OnDataRecv);
  
}

const int potPin = 35; // adc1 ch7

// the loop function runs over and over again forever
void loop() {
  // digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // delay(1000);                      // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  //delay(1000);                      // wait for a second
  
  // uint16_t potValue = analogRead(potPin);
  // Serial.println(potValue);
 
    //Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    Serial.println("No Signal");  
  }

  delay(200);

  // Serial.println("hello world");
}
