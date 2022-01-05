
#include <Arduino.h>
#include <ESP-Config.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "band_list.h"

// WiFi settings
constexpr char WIFI_SSID[] = "";
constexpr char WIFI_PASS[] = "";

// NTP stuff
WiFiUDP   ntpUDP;
NTPClient timeClient(ntpUDP);

// Config stuff
AsyncWebServer server(80);
ConfigPage     wspr_page("wspr", "WSPR", "This is the WSPR config page.");
ConfigSection  generel_section(wspr_page, "generel", "Generel", "");
ConfigString   callsign(generel_section, "callsign", "Callsign", "Your Callsign.", "NOCALL");
ConfigString   mainhad(generel_section, "mainhad", "Mainhad", "Your Mainhad.", "ABCD");
ConfigInt      power(generel_section, "power", "Power", "Power in dBm", 10);
ConfigSection  band_section(wspr_page, "bands", "Bands", "");
ConfigInt      band80(band_section, "band80", "80m", "Frequency for the 80m band.", 3592600);
ConfigBool     band80_active(band_section, "band80_active", "80m active", "Is this band active?", false);
ConfigInt      band60(band_section, "band60", "60m", "Frequency for the 60m band.", 5364700);
ConfigBool     band60_active(band_section, "band60_active", "60m active", "Is this band active?", false);
ConfigInt      band40(band_section, "band40", "40m", "Frequency for the 40m band.", 7038600);
ConfigBool     band40_active(band_section, "band40_active", "40m active", "Is this band active?", true);
ConfigInt      band30(band_section, "band30", "30m", "Frequency for the 30m band.", 10138700);
ConfigBool     band30_active(band_section, "band30_active", "30m active", "Is this band active?", false);
ConfigInt      band20(band_section, "band20", "20m", "Frequency for the 20m band.", 14095600);
ConfigBool     band20_active(band_section, "band20_active", "20m active", "Is this band active?", false);
ConfigInt      band17(band_section, "band17", "17m", "Frequency for the 17m band.", 18104600);
ConfigBool     band17_active(band_section, "band17_active", "17m active", "Is this band active?", false);
ConfigInt      band15(band_section, "band15", "15m", "Frequency for the 15m band.", 21094600);
ConfigBool     band15_active(band_section, "band15_active", "15m active", "Is this band active?", false);
ConfigInt      band12(band_section, "band12", "12m", "Frequency for the 12m band.", 24924600);
ConfigBool     band12_active(band_section, "band12_active", "12m active", "Is this band active?", false);
ConfigInt      band10(band_section, "band10", "10m", "Frequency for the 10m band.", 28124600);
ConfigBool     band10_active(band_section, "band10_active", "10m active", "Is this band active?", false);
ConfigInt      band6(band_section, "band6", "6m", "Frequency for the 6m band.", 50293000);
ConfigBool     band6_active(band_section, "band6_active", "6m active", "Is this band active?", false);
ConfigInt      freq_offset(band_section, "freq_offset", "Frequency offset", "The Frequency offset in the band.", 1500);

ConfigHTML    html;
ConfigFactory fact;

BandList bandList;

volatile bool proceed = false;
hw_timer_t *  timer   = NULL;

void initWiFi() {
  WiFi.setHostname("OE5BPA-WSPR");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initWebserver() {
  html.addPage(&wspr_page);
  fact.addPage(&wspr_page);

  html.registerWebServer(server, "/");
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
  server.begin();
}

void initBandList() {
  bandList.add(BandConfig("80", &band80_active, &band80));
  bandList.add(BandConfig("60", &band60_active, &band60));
  bandList.add(BandConfig("40", &band40_active, &band40));
  bandList.add(BandConfig("30", &band30_active, &band30));
  bandList.add(BandConfig("20", &band20_active, &band20));
  bandList.add(BandConfig("17", &band17_active, &band17));
  bandList.add(BandConfig("15", &band15_active, &band15));
  bandList.add(BandConfig("12", &band12_active, &band12));
  bandList.add(BandConfig("10", &band10_active, &band10));
  bandList.add(BandConfig("6", &band6_active, &band6));
}

void wspr_spacing() {
  proceed = true;
}

void setupTimer() {
  timer = timerBegin(3, 80, 1);
  timerAttachInterrupt(timer, &wspr_spacing, 1);
  timerAlarmWrite(timer, 682687, true);
  timerAlarmEnable(timer);
}

void setup(void) {
  Serial.begin(115200);
  delay(500);
  SPIFFS.begin(true);
  Serial.println("Si5351 Clockgen Test");
  Serial.println("");

  initWiFi();
  timeClient.begin();
  while (!timeClient.update()) { // wait for ntp server sync
  }
  Serial.println(timeClient.getFormattedTime());
  initWebserver();
  initBandList();
}

int oldSecond = 0;

bool wsprStart(int currentSecond) {
  return oldSecond == 0 && currentSecond == 1 && (timeClient.getMinutes() == 0 || timeClient.getMinutes() % 2 == 0); // 4 == because we want to take a break every 2 min.
}

void secondChange(int currentSecond) {
  if (currentSecond % 5 == 0) {
    Serial.println(timeClient.getFormattedTime());
  }
  if (wsprStart(currentSecond)) {
    BandConfig currentBand = bandList.getNext();
    Serial.printf("current band: %s, is enabled: %d, frequency: %ld\n", currentBand.getName().c_str(), currentBand.isEnabled(), currentBand.getFrequency());
    Serial.println("will transmit for the next 2 min ....");
  }
}

void loop(void) {
  int currentSecond = timeClient.getSeconds();
  if (currentSecond >= 11 || currentSecond < 30) {
    timeClient.update();
  }

  if (html.wasSaved()) {
    fact.saveConfig("/test.json");
  }

  if (oldSecond != currentSecond) {
    secondChange(currentSecond);
  }
  oldSecond = currentSecond;
}
