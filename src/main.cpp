#include "config.h"

#include <SPI.h>
#include <Adafruit_MAX31855.h>
#include <Arduino.h>

// #include <WiFiNINA.h>
// #include <HttpClient.h>
// #include <influxdb.h>
#include <point.h>


// #define DEBUG 1
//
// int wifi_status;
// WiFiClient wifi_client;
// HttpClient http_client("influx.klaital.com",  8086, &wifi_client);
// Influx::InfluxDbClient influx(INFLUX_HOST, INFLUX_ORG, INFLUX_BUCKET, INFLUX_TOKEN);
// Influx::Point dryer;
// Influx::Point water;

constexpr pin_size_t water_sensor_pin = A7;
constexpr pin_size_t alarm_pin = A6;

constexpr pin_size_t thermo_do_pin = MISO;
constexpr pin_size_t thermo_cs_pin = 8;
constexpr pin_size_t thermo_clk_pin = SCK;

Adafruit_MAX31855 thermocouple(thermo_clk_pin, thermo_cs_pin, thermo_do_pin);
bool thermocouple_initialized = false;

void setup() {
// write your initialization code here
    Serial.begin(9600);
#ifdef DEBUG
    while(!Serial) delay(1); // wait for Serial connection
#endif

    Serial.println("initializing thermocouple...");
    delay(500);
    if(thermocouple.begin()) {
        thermocouple_initialized = true;
    } else {
        Serial.println("ERROR - failed to initialize thermocouple");
    }

    Serial.println("initializing water sensor...");
    pinMode(water_sensor_pin, INPUT);

    Serial.println("initializing alarm...");
    pinMode(alarm_pin, OUTPUT);

#ifdef WIFI
    Serial.println("initializing wifi...");
    wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    dryer.set_field("room", "laundry");
    water.set_field("room", "laundry");
#endif

}

// TODO: fix the default once we're done debugging the alarm circuitry
bool alarm_triggered = false;

void loop() {

#ifdef WIFI
    wifi_status = WiFi.status();
    if (wifi_status != WL_CONNECTED) {
        Serial.println("Reconnecting to wifi...");
        wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
#endif

    if (alarm_triggered) {
        Serial.println("come hell or high water!");
        // Bleep the alarm three times
        digitalWrite(alarm_pin, HIGH);
        delay(1000);
        digitalWrite(alarm_pin, LOW);
        delay(1000);

        digitalWrite(alarm_pin, HIGH);
        delay(1000);
        digitalWrite(alarm_pin, LOW);
        delay(1000);

        digitalWrite(alarm_pin, HIGH);
        delay(1000);
        digitalWrite(alarm_pin, LOW);
    }

    if (thermocouple_initialized) {
        const double internal = thermocouple.readInternal();
        const double t = thermocouple.readFahrenheit();
        if (isnan(t)) {
            Serial.println("ERROR - thermocouple read error");
            const uint8_t e = thermocouple.readError();
            if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
            if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
            if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
        } else {
            Serial.print("Internal = ");
            Serial.print(internal);
            Serial.print(" Thermocouple = ");
            Serial.println(t);
            // TODO: transmit reading over mqtt or influx if network is available

            // Trip the alarm if it looks like the dryer duct is on fire
            if (t > 200) {
                Serial.println("bleep!");
                alarm_triggered = true;
            }
        }
    }

    // Read the water sensor. If there is water, trigger the alarm!
    const int water_reading = analogRead(water_sensor_pin);
    // TODO: transmit water reading over mqtt or influx if the network is available
    Serial.print("water = ");
    Serial.println(water_reading);
    if (water_reading > 100) { // TODO: tune the threshold value
        Serial.println("bleep!");
        alarm_triggered = true;
    }

    delay(2500);
}
