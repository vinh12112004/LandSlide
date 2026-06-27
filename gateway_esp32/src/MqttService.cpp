#include "MqttService.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"

static WiFiClient espClient;
static PubSubClient mqtt(espClient);

static String getIsoTimestamp()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return "";
    }

    char buffer[30];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%dT%H:%M:%SZ",
        &timeinfo
    );

    return String(buffer);
}

void Wifi_Connect()
{
    Serial.print("[WIFI] Connecting");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("[WIFI] Connected");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
}

void Time_Sync()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("[NTP] Syncing");

    struct tm timeinfo;

    while (!getLocalTime(&timeinfo))
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("[NTP] Time synced");
}

static void MQTT_Connect()
{
    while (!mqtt.connected())
    {
        Serial.print("[MQTT] Connecting... ");

        if (mqtt.connect(MQTT_CLIENT_ID))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.println(mqtt.state());
            delay(1000);
        }
    }
}

void MQTT_Init()
{
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    MQTT_Connect();
}

void MQTT_Loop()
{
    if (!mqtt.connected())
    {
        MQTT_Connect();
    }

    mqtt.loop();
}

void MQTT_PublishSensor(const PacketData_t &packet)
{
    JsonDocument doc;

    doc["DeviceId"] = packet.deviceId;

    String timestamp = getIsoTimestamp();

    if (timestamp.length() > 0)
    {
        doc["Timestamp"] = timestamp;
    }

    JsonObject data = doc["Data"].to<JsonObject>();

    if (packet.sensorMask & HAS_SOIL)
        data["soil_m"] = packet.soil_m;

    if (packet.sensorMask & HAS_VIB)
        data["vib"] = packet.vib;

    if (packet.sensorMask & HAS_TILT)
        data["tilt"] = packet.tilt;
    if (packet.sensorMask & HAS_GPS)
        {
            // Chú ý: Key là "Lat" và "Lon" (viết hoa chữ cái đầu) 
            // để khớp hoàn toàn với Backend C# (dto.Data.TryGetValue("Lat", ...))
            data["Lat"] = packet.lat / 10000000.0;
            data["Lon"] = packet.lon / 10000000.0;

            // Bổ sung thêm các thông số độ cao và trạng thái vệ tinh (tùy chọn)
            data["hMSL"] = packet.hMSL;
            data["fixType"] = packet.fixType;
            data["numSV"] = packet.numSV;
        }

    char topic[80];

    snprintf(
        topic,
        sizeof(topic),
        "landslide/%s/data",
        packet.deviceId
    );

    char payload[256];

    serializeJson(
        doc,
        payload,
        sizeof(payload)
    );

    bool ok = mqtt.publish(topic, payload);

    Serial.print("[MQTT] Topic: ");
    Serial.println(topic);

    Serial.print("[MQTT] Payload: ");
    Serial.println(payload);

    Serial.print("[MQTT] Publish: ");
    Serial.println(ok ? "OK" : "FAILED");
}