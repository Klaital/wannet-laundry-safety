//
// Created by Kit on 1/30/2025.
//

#ifndef CONFIG_H
#define CONFIG_H

constexpr char WIFI_SSID[] = "WANNET";
constexpr char WIFI_PASS[] = "eatmithkabobs";

constexpr char INFLUX_HOST[] = "http://influx.klaital.com:8086";
constexpr char INFLUX_ORG[] = "";
constexpr char INFLUX_BUCKET[] = "";
constexpr char INFLUX_TOKEN[] = "";

constexpr char MQTT_BROKER_HOST[] = "klaital.com";
constexpr int MQTT_BROKER_PORT = 1883;
constexpr char MQTT_TOPIC_DIMMER[] = "bedroom/lights/dimmer";
constexpr char MQTT_TOPIC_WAKEUP[] = "bedroom/lights/wakeup";


#endif //CONFIG_H
