#include "remote.hxx"

const char *remote::EX_ssid = nullptr;
const char *remote::EX_pass = nullptr;
const char *remote::AP_ssid = "SH_FFF";
const char *remote::AP_pass = "nullptr!";

IPAddress remote::begin()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(EX_ssid, EX_pass);
    while (WiFi.status() != WL_CONNECTED)
        delay(100);
    WiFi.softAP(AP_ssid, AP_pass);
    return WiFi.softAPIP();
}

wl_status_t remote::connect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_ssid, AP_pass);

    return WiFi.status();
}
