#include <weather.h>

String regionID = "8176";
WiFiClient client;
HTTPClient http;

XMLDocument xmlDocument;

String parseXML(String xmlDoc)
{
    Serial.println("Parsing XML...");
    const char *msg = xmlDoc.c_str();
    // Serial.println(msg); // uncomment for debugging
    if (xmlDocument.Parse(msg) != XML_SUCCESS)
    {
        Serial.println("Error parsing");
        //return;
    };
    XMLElement *pRoot = xmlDocument.FirstChildElement("feed");
    XMLElement *pElement_0 = pRoot->FirstChildElement("entry");
    XMLElement *pElement_1 = pElement_0->FirstChildElement("title");
    XMLElement *pElement_2 = pElement_0->FirstChildElement("summary");
    const char *pData_1 = nullptr;
    const char *pData_2 = nullptr;
    pData_1 = pElement_1->GetText();
    Serial.print("Check block_1...");
    XMLCheckResult(pData_1);
    pData_2 = pElement_2->GetText();
    Serial.print("Check block_2...");
    XMLCheckResult(pData_2);
    Serial.print(pData_1);
    Serial.println(pData_2);
    Serial.println("Parsing OK!");
    String output = output + pData_1 + pData_2;
    return output;
}

void getWeatherData()
{
    Serial.println("Connecting to the HTTP server....");
    String serverPath = "http://rp5.ru/rss/" + regionID + "/ru";
    Serial.print("HTTP URL = ");
    Serial.println(serverPath);
    if (http.begin(client, serverPath))
    {
        Serial.println("[HTTP] GET...");
        int code = http.GET();
        if (code > 0)
        {
            if (code == HTTP_CODE_OK || code == HTTP_CODE_MOVED_PERMANENTLY)
            {
                Serial.print("GET OK.");
                Serial.printf(" HTTP Code [%d]", code);
                Serial.println();
                String payload = http.getString();
                // Serial.print("XML: "); // uncomment for debugging
                // Serial.println(payload); // uncomment for debugging
                global::temp = parseXML(payload);
            }
        }
        else
        {
            Serial.printf("[HTTP] GET failed, error: %s", http.errorToString(code).c_str());
            http.end();
        }
    }
}