#include <SPI.h>
#include <Adafruit_MAX31855.h>
#include <Arduino.h>

// #define DEBUG 1

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
}

// TODO: fix the default once we're done debugging the alarm circuitry
bool alarm_triggered = false;

void loop() {

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

    delay(5000);
}
