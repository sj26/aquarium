#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Time.h>

#define LED 0
#define LED_STRIP 4

const long timeZone = 10;

const char* ssid = "...";
const char* password = "...";

#define SECONDS_PER_DAY (60 * 60 * 24)

// Turn on to 100, fading up over an hour between 7am and 8am
#define ON_FADE_FROM_SECONDS (7 * 60 * 60)
#define ON_FADE_TO_SECONDS (8 * 60 * 60)
#define ON_FADE_DURATION (ON_FADE_TO_SECONDS - ON_FADE_FROM_SECONDS)
#define ON_LEVEL 100

// Dim to 50, fading from 6pm to 7pm
#define DIM_FADE_FROM_SECONDS (18 * 60 * 60)
#define DIM_FADE_TO_SECONDS (19 * 60 * 60)
#define DIM_FADE_DURATION (DIM_FADE_TO_SECONDS - DIM_FADE_FROM_SECONDS)
#define DIM_LEVEL 50

// Turn off, fading from 10pm to 11pm
#define OFF_FADE_FROM_SECONDS (22 * 60 * 60)
#define OFF_FADE_TO_SECONDS (23 * 60 * 60)
#define OFF_FADE_DURATION (OFF_FADE_TO_SECONDS - OFF_FADE_FROM_SECONDS)
#define OFF_LEVEL 0

WiFiUDP timeClientUDP;
NTPClient timeClient(timeClientUDP, "pool.ntp.org", timeZone * 3600);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  analogWrite(LED_STRIP, 0);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  timeClient.begin();

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
  }
}

void loop() {
  Serial.println("Updating time...");
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

  int level = 0;
  int epoch = timeClient.getEpochTime();
  int seconds = epoch % SECONDS_PER_DAY;
  
  if (seconds >= ON_FADE_FROM_SECONDS && seconds < ON_FADE_TO_SECONDS) {
    Serial.println("Fading to ON");
    level = (int)((float)(seconds - ON_FADE_FROM_SECONDS) / (float)(ON_FADE_DURATION) * (float)(ON_LEVEL));
    
  } else if (seconds >= ON_FADE_TO_SECONDS && seconds < DIM_FADE_FROM_SECONDS) {
    Serial.println("ON");
    level = ON_LEVEL;
    
  } else if (seconds >= DIM_FADE_FROM_SECONDS && seconds < DIM_FADE_TO_SECONDS) {
    Serial.println("Fading to DIM");
    level = ON_LEVEL - (int)((float)(seconds - DIM_FADE_FROM_SECONDS) / (float)(DIM_FADE_DURATION) * (float)(ON_LEVEL - DIM_LEVEL));
    
  } else if (seconds >= DIM_FADE_TO_SECONDS && seconds < OFF_FADE_FROM_SECONDS) {
    Serial.println("DIM");
    level = DIM_LEVEL;
    
  } else if (seconds >= OFF_FADE_FROM_SECONDS && seconds < OFF_FADE_TO_SECONDS) { 
    Serial.println("Fading to OFF");
    level = DIM_LEVEL - (int)((float)(seconds - OFF_FADE_FROM_SECONDS) / (float)(OFF_FADE_DURATION) * (float)(DIM_LEVEL));
    
  } else {
    Serial.println("OFF");
    level = OFF_LEVEL;
  }

  Serial.printf("Setting leds to: %d\n", level);
  analogWrite(LED_STRIP, level);
  
  // Refresh as close to every second as we can
  delay(1000 - (millis() % 1000));
}
