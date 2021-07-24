#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncTelegram2.h>
#include <time.h>
#include "config.h"

#define MYTZ "BRST+3BRDT+2,M10.3.0,M2.3.0"

BearSSL::WiFiClientSecure client;
BearSSL::Session session;
BearSSL::X509List certificate(telegram_cert);
AsyncTelegram2 myBot(client);

// Send a message to specific user who has started your bot
// Target user can find it's own userid with the bot @JsonDumpBot
// https://t.me/JsonDumpBot
int64_t userid = TELEGRAM_USERID;

const char *token = TELEGRAM_TOKEN; // Telegram token
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

bool isMicTriggered = false;

void setupWifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupCertificate()
{
  // Sync time with NTP, to check properly Telegram certificate
  configTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  //Set certficate, session and some other base client properies
  client.setSession(&session);
  client.setTrustAnchors(&certificate);
  client.setBufferSizes(1024, 1024);
}

void setupTelegramBot()
{
  // Set the Telegram bot properies
  myBot.setUpdateTime(2000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nConnecting to Telegram... ");
  myBot.begin() ? Serial.println("Connected!") : Serial.println("FAIL!");

  myBot.sendTo(userid, TELEGRAM_CONNECTED_MESSAGE);
}

void notifyTelegramMicTriggered()
{
  myBot.sendTo(userid, TELEGRAM_TRIGGERED_MESSAGE);
}

void setup()
{
  pinMode(D1, INPUT_PULLUP);
  Serial.begin(115200);

  setupWifi();
  setupCertificate();
  setupTelegramBot();
}

void loop()
{
  isMicTriggered = !digitalRead(D1);

  if (isMicTriggered)
  {
    Serial.println("Mic triggered!");
    notifyTelegramMicTriggered();
    delay(5000);
  }

  delay(100);
}
