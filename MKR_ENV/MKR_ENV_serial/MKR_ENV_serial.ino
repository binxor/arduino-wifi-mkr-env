/*

  MKR ENV Shield - Read Sensors

  This example reads the sensors on-board the MKR ENV shield

  and prints them to the Serial Monitor once a second.

  The circuit:

  - Arduino MKR board

  - Arduino MKR ENV Shield attached

  This example code is in the public domain.

  All the values are returned as floats and have the following ranges, expressed in the SI units:

  - Absolute pressure range: 260 to 1260 hPa
  - Humidity range: 0 - 100%; accuracy: ± 3.5% rH, 20 to +80% rH
  - Temperature range -40 +120; accuracy: ± 0.5 °C, 15 to +40 °C
  - Lux range of the sensor with analog reading from 10 to 100.000 lux
  - UVA/UVB resolution: 16bit; unit μW/cm2
  - UVIndex: from 1 to 11+ as explained here.

*/

#include <Arduino_MKRENV.h>

void setup() {

  Serial.begin(9600);

  while (!Serial);

  if (!ENV.begin()) {

    Serial.println("Failed to initialize MKR ENV shield!");

    while (1);

  }
}

void loop() {

  // read all the sensor values

  float temperature = ENV.readTemperature();      // C

  float temperatureF = temperature * 9 / 5 + 32;  // F

  float humidity    = ENV.readHumidity();

  float pressure    = ENV.readPressure();         // kPa

  float pressurePsi = pressure / 6.895;           // PSI

  float pressureAtm = pressure / 101;             // Std. atmospheres

  float illuminance = ENV.readIlluminance();

  float uva         = ENV.readUVA();

  float uvb         = ENV.readUVB();

  float uvIndex     = ENV.readUVIndex();

  // print each of the sensor values

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

  // print an empty line

  Serial.println();

  // wait 1 second to print again

  delay(10000);
}
