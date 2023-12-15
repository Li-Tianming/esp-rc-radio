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
#include <WiFi.h>
#include <esp_now.h>

// bfs::SbusTx sbus_tx(&Serial2, 33, 32, true, false);
// bfs::SbusData data;

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

// uint8_t receiverMacAddress[] = {0x54,0x43,0xB2,0xB7,0x5C,0x20};  // 54:43:B2:B7:5C:20
uint8_t receiverMacAddress[] = {0xD4,0xD4,0xDA,0x72,0x37,0x60};  // D4:D4:DA:72:37:60


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);
  // 54:43:B2:B7:5C:60
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  
  memset(&peerInfo, 0, sizeof(peerInfo));

  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  } 

}


const int potPin = 35; // adc1 ch7

// the loop function runs over and over again forever
void loop() {
  // digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // delay(1000);                      // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  //delay(1000);                      // wait for a second
  
  // uint16_t potValue = analogRead(potPin);
  chan_adc_val.ch0 = analogRead(35); // // adc1 ch7
  chan_adc_val.ch1 = analogRead(34); // // adc1 ch7
  chan_adc_val.ch2 = analogRead(39); // // adc1 ch7
  chan_adc_val.ch3 = analogRead(36); // // adc1 ch7
  
  Serial.print(chan_adc_val.ch0);
  Serial.print(",");  
  Serial.print(chan_adc_val.ch1);
  Serial.print(",");
  Serial.print(chan_adc_val.ch2);
  Serial.print(",");
  Serial.print(chan_adc_val.ch3);
  Serial.print(",");
  Serial.println();
  
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &chan_adc_val, sizeof(chan_adc_val));
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }  

  delay(20);

  // Serial.println("hello world");
}
