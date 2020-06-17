#include "climate.hxx"

const static int DHT_PIN = 8;
static SimpleDHT11 dht(DHT_PIN);

climate::dht_data climate::get()
{
    byte t, h;
    if ((dht.read(&t, &h, nullptr)) != SimpleDHTErrSuccess)
    {
        t = h = -1;
    }

    return {t, h};
}

climate::season climate::fromMonth(uint8_t month)
{
    return climate::season((month + 10) % 12 / 3);
}
