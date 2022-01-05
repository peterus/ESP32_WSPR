
#include <Arduino.h>
#include <ESP-Config.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

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
ConfigBool     band40_active(band_section, "band40_active", "40m active", "Is this band active?", false);
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
  html.registerWebServer(server, "/");
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });
  server.begin();
}

void setup(void) {
  Serial.begin(115200);
  delay(500);
  SPIFFS.begin(true);
  Serial.println("Si5351 Clockgen Test");
  Serial.println("");

  html.addPage(&wspr_page);
  fact.addPage(&wspr_page);

  initWiFi();
  timeClient.begin();
  initWebserver();
}

int oldSecond = 0;

void loop(void) {
  timeClient.update();

  if (html.wasSaved()) {
    fact.saveConfig("/test.json");
  }

  if (oldSecond == 0 && timeClient.getSeconds() == 1 && timeClient.getMinutes() % 2 == 0) {
    // start sending WSPR
  }
  oldSecond = timeClient.getSeconds();
}
