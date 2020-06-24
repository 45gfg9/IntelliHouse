#include "remote.hxx"

char *remote::EX_ssid = nullptr;
char *remote::EX_pass = nullptr;
const char *remote::AP_ssid = "SH_FFF";
const char *remote::AP_pass = "nullptr!";

IPAddress remote::AP_ip;

static void connectBlocking(const char *ssid, const char *pass)
{
    static const int LED = LED_BUILTIN;
    WiFi.begin(ssid, pass);
    pinMode(LED, OUTPUT);

    for (;;)
    {
        wl_status_t status = WiFi.status();
        if (status == WL_CONNECTED)
            break;
        else if (status == WL_CONNECT_FAILED)
            for (;;)
            {
                digitalWrite(LED, HIGH);
                delay(100);
                digitalWrite(LED, LOW);
                delay(100);
            }
        else if (status == WL_NO_SSID_AVAIL)
            for (int i = 1000; i > 0; i -= 100)
            {
                digitalWrite(LED, HIGH);
                delay(i);
                digitalWrite(LED, LOW);
                delay(i);
        }
    }
}

static bool connect(WiFiClient &client, String host, int port)
{
    if (!client.connect(host, port))
        return false;
    while (!client.connected())
        ;
    return true;
}

static bool connect(WiFiClient &client, const IPAddress &host, int port)
{
    return connect(client, host.toString(), port);
}

static bool connectAP(WiFiClient &client)
{
    return connect(client, remote::AP_ip, 80);
}

static String readResponse(WiFiClient &client, int loadTime = 500)
{
    unsigned long timeout = millis();
    while (client.available() == 0)
        if (millis() - timeout > 5000)
            return String();
    delay(loadTime);

    return client.readString();
}

static String parseContent(String content)
{
    int idx = content.indexOf("\r\n\r\n");

    return content.substring(idx + 4);
}

void remote::begin()
{
    if (EX_ssid != nullptr)
    {
        WiFi.mode(WIFI_AP_STA);
        connectBlocking(EX_ssid, EX_pass);
    }
    else
    {
        WiFi.mode(WIFI_AP);
    }
    WiFi.softAP(AP_ssid, AP_pass);
    remote::AP_ip = WiFi.softAPIP();
}

void remote::connect()
{
    WiFi.mode(WIFI_STA);

    connectBlocking(AP_ssid, AP_pass);
}

uint32_t remote::getTime()
{
    WiFiClient client;
    connectAP(client);
    client.println(header(AP_ip, "/time"));
    String content = parseContent(readResponse(client));

    return content.toInt();
}

String remote::getWeatherJsonStr(String psk)
{
    static const String host(F("api.seniverse.com"));
    static const String uri(F("/v3/weather/now.json"));
    static const String partialQuery(F("?language=en&location=ip&key="));

    WiFiClient client;
    connect(client, host, 80);
    client.println(header(host, uri, partialQuery + psk));
    return parseContent(readResponse(client));
}

common::weather_data remote::getWeatherData()
{
    WiFiClient client;
    connectAP(client);
    client.println(header(AP_ip, "/weather"));
    String content = parseContent(readResponse(client));

    int sep = content.indexOf(',');
    String location = content.substring(0, sep);
    content = content.substring(sep + 1);

    sep = content.indexOf(',');
    String weather = content.substring(0, sep);
    byte temp = content.substring(sep + 1).toInt();

    return {location.c_str(), weather.c_str(), temp};
}

String remote::header(IPAddress host, String uri)
{
    return header(host.toString(), uri, "");
}

String remote::header(String host, String uri)
{
    return header(host, uri, "");
}

String remote::header(IPAddress host, String uri, String query)
{
    return header(host.toString(), uri, query);
}

String remote::header(String host, String uri, String query)
{
    return String(F("GET ")) + uri + query + F(" HTTP/1.1\r\nHost: ") + host + F("\r\nConnection: close\r\n\r\n");
}
