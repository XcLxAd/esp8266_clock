#include <wifi.h>

byte tries = 20;

void connectWiFi()
{
    pinMode(INT_LED, OUTPUT); // инициализация пина с подключенным светодиодом
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting");
    while (--tries && WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(INT_LED, !digitalRead(INT_LED));
        Serial.print(".");
        delay(500);
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(INT_LED, HIGH);
        Serial.println();
        Serial.println("Non Connecting to WiFi.");
    }
    else
    {
        digitalWrite(INT_LED, LOW);
        Serial.println();
        Serial.println("WiFi connected.");
        printWiFiStatus();
    }
}

void printWiFiStatus() // print Wifi status
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI): ");
    Serial.print(rssi);
    Serial.println(" dBm");
}