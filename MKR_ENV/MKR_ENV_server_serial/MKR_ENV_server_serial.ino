#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>

/*
  All the MKR ENV shield sensor values are returned as floats and have the following ranges, expressed in the SI units:

  - Absolute pressure range: 260 to 1260 hPa
  - Humidity range: 0 - 100%; accuracy: ± 3.5% rH, 20 to +80% rH
  - Temperature range -40 +120; accuracy: ± 0.5 °C, 15 to +40 °C
  - Lux range of the sensor with analog reading from 10 to 100.000 lux
  - UVA/UVB resolution: 16bit; unit μW/cm2
  - UVIndex: from 1 to 11+

*/

char ssid[] = "{wifi-ssid-here}";
char pass[] = "{wifi-password-here}";
int status = WL_IDLE_STATUS;

WiFiServer server(80);
WiFiClient client;

void setup() {
  setup_serial();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  Serial.println();
  Serial.println("Initial Readings:");
  read_sensors();
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    Serial.print("Latest Version:\t\t");
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
    Serial.print("Installed Version:\t");
    Serial.println(fv);
  }

  while (status != WL_CONNECTED) {// attempt to connect to Wifi network:
    Serial.print("Attempting to connect to SSID:\t");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000); // wait 1 seconds for connection:
  }
  server.begin();
  printWifiStatus();
}


void loop() {
  WiFiClient client = server.available();// listen for incoming clients
  if (client) {
    boolean currentLineIsBlank = true;  
    while (client.connected()) {
      if (client.available() ) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) { 
          Serial.print("HTTP request from IP ");
          Serial.println(client.remoteIP());
          float tempF = get_tempF();
          float humid = get_humidity();
          float presAtm = get_pressureAtm();
          float illum = get_illum();
          float uva = get_uva();
          float uvb = get_uvb();
          float uvIndex = get_uvi();

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Connection: close");
          client.println("Refresh: 30");
          client.println();
          client.print("[{"); 
          client.print("\"Temperature F\": "); 
          client.print(tempF);
          client.print(", \"Humidity %RH\": ");
          client.print(humid);
          client.print(", \"Pressure ATM\": ");
          client.print(presAtm);
          client.print(", \"Illumination Lux\": ");
          client.print(illum);
          client.print(", \"UVA μW/cm2\": ");
          client.print(uva);
          client.print(", \"UVB μW/cm2\": ");
          client.print(uvb);
          client.print(", \"UV Index\": ");
          client.print(uvIndex);
          client.print("}]");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1); // give the web browser time to receive the data
    client.stop();
  }
}


void printWifiStatus() {
  Serial.println("---");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("---");
}


void setup_serial() {
  Serial.begin(9600);
  while (!Serial);
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
}

float get_tempF() {
    return ENV.readTemperature() * 9 / 5 + 32; // F
}

float get_humidity() {
    return ENV.readHumidity();
}

float get_pressureAtm() {
  return ENV.readPressure() / 101; // Std. atmospheres
}

float get_illum() { 
  return ENV.readIlluminance(); // Lx
}

float get_uva() {
  return ENV.readUVA();
}

float get_uvb() {
  return ENV.readUVB();
}

float get_uvi() {
  return ENV.readUVIndex();
}


float read_sensors() {
    float temperature = ENV.readTemperature();      // C
    float temperatureF = temperature * 9 / 5 + 32;  // F
    float humidity    = ENV.readHumidity();
    float pressure    = ENV.readPressure();         // kPa
    float pressurePsi = pressure / 6.895;           // PSI
    float pressureAtm = pressure / 101;             // Std. atmospheres
    float illuminance = ENV.readIlluminance();      // Lux
    float uva         = ENV.readUVA();              // unit μW/cm2
    float uvb         = ENV.readUVB();              // unit μW/cm2
    float uvIndex     = ENV.readUVIndex();          // UV Index
    Serial.print("Temperature = ");
    Serial.print(temperatureF);
    Serial.println(" °F");
    Serial.print("Humidity    = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Pressure    = ");
    Serial.print(pressureAtm);
    Serial.println(" std atmosphere");
    Serial.print("Illuminance = ");
    Serial.print(illuminance);
    Serial.println(" lx");
    Serial.print("UVA         = ");
    Serial.println(uva);
    Serial.print("UVB         = ");
    Serial.println(uvb);
    Serial.print("UV Index    = ");
    Serial.println(uvIndex);
    Serial.println();
    delay(100);
    return temperatureF;
}
