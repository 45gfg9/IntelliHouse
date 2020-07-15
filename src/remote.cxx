#include "remote.hxx"

IPAddress remote::AP_ip;

static void gen(int pin, int delay1, int delay2)
{
    digitalWrite(pin, HIGH);
    delay(delay1);
    digitalWrite(pin, LOW);
    delay(delay2);
}

static void connectBlocking(const char *ssid, const char *pass)
{
    static const int LED = LED_BUILTIN;
    Serial.printf_P(PSTR("Connecting to %s"), ssid);
    WiFi.begin(ssid, pass);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    for (;;)
    {
        wl_status_t status = WiFi.status();
        if (status == WL_CONNECTED)
        {
            Serial.println(F(" Connected!"));
            digitalWrite(LED, HIGH);
            break;
        }
        else if (status == WL_CONNECT_FAILED)
        {
            Serial.printf_P(PSTR(" Failed to connect to %s"), ssid);
            for (int i = 0; i < 10; i++)
                gen(LED, 100, 100);
            ESP.reset();
        }
        else if (status == WL_NO_SSID_AVAIL)
        {
            Serial.printf_P(PSTR(" %s not found"), ssid);
            for (int i = 500; i > 0; i -= 50)
                gen(LED, i, i);
            ESP.reset();
        }
        Serial.print('.');
        delay(200);
    }
}

static bool connect(WiFiClient &client, const IPAddress &host, int port)
{
    if (!client.connect(host, port))
    {
        Serial.print(F("Failed to connect to "));
        Serial.println(host);
        return false;
    }
    Serial.print(F("Establishing connection to "));
    Serial.print(host);
    while (!client.connected())
    {
        Serial.print('.');
        delay(200);
    }
    Serial.println(F(". Established!"));
    return true;
}

static bool connect(WiFiClient &client, const char *host, int port)
{
    IPAddress addr;
    if (!WiFi.hostByName(host, addr))
        return false;
    return connect(client, addr, port);
}

static bool connect(WiFiClient &client, String host, int port)
{
    return connect(client, host.c_str(), port);
}

static bool connectAP(WiFiClient &client)
{
    return connect(client, remote::AP_ip, SERVER_PORT);
}

static String readResponse(WiFiClient &client, unsigned long loadTime = 500, unsigned long timeout = 10000)
{
    unsigned long start = millis();
    while (client.available() == 0)
    {
        if (millis() - start > timeout)
        {
            Serial.println("Connection timeout");
            return emptyString;
        }
    }
    Serial.println(F("Receiving"));
    delay(loadTime);

    return client.readString();
}

static String parseContent(String content)
{
    if (content.length() == 0)
        return content;
    int idx = content.indexOf("\r\n\r\n");

    return content.substring(idx + 4);
}

void remote::begin()
{
    if (EX_SSID != nullptr)
    {
        WiFi.mode(WIFI_AP_STA);
        connectBlocking(EX_SSID, EX_PASS);
    }
    else
    {
        WiFi.mode(WIFI_AP);
    }
    WiFi.softAP(AP_SSID, AP_PASS);

    Serial.print(F("Gateway IP: "));
    Serial.println(WiFi.gatewayIP());
    Serial.print(F("Local IP: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("AP IP: "));
    Serial.println(WiFi.softAPIP());
}

void remote::connect()
{
}

bool remote::setTime()
{
}

weather_data remote::getWeatherData()
{
}
