// Brosta - May/25/2021

#include <WiFiNINA.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_MKRIoTCarrier.h>

#define SECRET_SSID               "Spiti"
#define SECRET_PASS               "kodikos888"
#define BRO_WITH_WIFI             "1"
#define BRO_WITH_CARRIER          "1"
#define BRO_WITH_SERIAL_MONITOR   "1"
#define BRO_WITH_CARRIER_MONITOR  "1"
#define BRO_WITH_HTTP             "1"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int _http_port                    = 80;
int loaded                        = 0;
int serial_monitor_opened         = 0;
int carrier_monitor_opened        = 0;
int wifi_connected                = 0;
float humidity                    = 0;
float pressure                    = 0;
float temperature                 = 0;
char _server_name[]               = "www.brosta.org";
String _action_name               = "www.brosta.org";

String _http_scheme               = "http";
String _http_host                 = "www.brosta.org";
String _http_path                 = "/school/weather";
String _http_url                  = _http_scheme + _http_host + _http_path;

String username                   = "Brosta";
String location_country           = "GR";
String location_state             = "Fokida";
String location_city              = "Efpalio";
String location_region            = "Loggos";

WiFiClient _http;
MKRIoTCarrier Brosta;

void setup() {
  Serial.begin(9600);
  CARRIER_CASE = true;
  Brosta.begin();
  // End Arduino
  // Start Brosta
  if(BRO_WITH_CARRIER_MONITOR) {
    openTheCarierMonitor();
  }
  if(BRO_WITH_CARRIER_MONITOR) {
    openTheCarierMonitor();
  }
  if(BRO_WITH_WIFI) {
    loadWiFi(1);
  }
}

void loop() {
  Brosta.Buttons.update();
  if(Brosta.Button4.onTouchDown()) {
    if(wifi_connected) {
      wifi_connected = 0;
    } else {
      wifi_connected = 1;
    }
    loadWiFi(wifi_connected);
  }
  if(loaded) {
    pressure = Brosta.Pressure.readPressure(PSI);

    pressure = pressure * 68.94757293;
    
    humidity = Brosta.Env.readHumidity();
    temperature = Brosta.Env.readTemperature();
    if (BRO_WITH_HTTP) {
      String query = "username=" + username
        + "&location="      + location_country + "%2C " +location_state + "%2C " + location_city + "%2C "  + location_region
        + "&sconnected="    + 1
        + "&temperature="   + String(temperature)
        + "&humidity="      + String(humidity)
        + "&pressure="      + String(pressure);
      // All fields need to set here
      httpPost("/school/weather", query);
    }
  }
  delay(10000);
}

void httpPost(String url, String data) {
  if (_http.connect(_server_name, _http_port)) {
    _http.println("POST " + _http_path + " HTTP/1.1");
    _http.println("Host: " + _http_host);
    _http.println("Content-Type: application/x-www-form-urlencoded");
    _http.print("Content-Length: ");
    _http.println(data.length());
    _http.println();
    _http.print(data);
  }
  if(_http.connected()) {
    _http.stop();
  }
}

void openTheCarierMonitor() {
  carrier_monitor_opened = 1;
  Brosta.display.setRotation(0);
  Brosta.display.fillScreen(ST77XX_BLACK);
  Brosta.display.setTextColor(ST77XX_WHITE);
  Brosta.display.setTextSize(2);
}

void present(String text, int way = 0) {
  if (BRO_WITH_CARRIER_MONITOR) {
    if(way == 1) {
      Brosta.display.println(text);
    } else {
      Brosta.display.print(text);
    }
  }
  if(BRO_WITH_SERIAL_MONITOR) {
    if(way == 1) {
      Serial.println(text);
    } else {
      Serial.print(text);
    }
  }
}

void loadWiFi(int positive) {
  if (!positive) {
    WiFi.disconnect();
    Brosta.display.print("WiFi Disconnected");
  } else {
    WiFi.begin(ssid, pass);
    delay(10000);
    loaded = 1;
    IPAddress ip = WiFi.localIP();
    Brosta.display.setTextSize(2);
    Brosta.display.setCursor(35, 60);
    Brosta.display.println("WiFi Connected");
    Brosta.display.setCursor(100, 90);
    Brosta.display.println("SSID");
    Brosta.display.setCursor(102, 120);
    Brosta.display.println(WiFi.SSID());
    Brosta.display.setCursor(112, 150);
    Brosta.display.println("IP");
    Brosta.display.setCursor(55, 180);
    Brosta.display.println(ip);
  }
}
