#include "th.h"

const static int DHT_A_PIN = 2;
const static int DHT_B_PIN = 3;

struct th::data {
    byte temp;
    byte humid;
};

static SimpleDHT11 dht_in(DHT_A_PIN);
static SimpleDHT11 dht_out(DHT_B_PIN);

static th::data get(SimpleDHT& dht) {
    byte t, h;
    if ((dht.read(&t, &h, nullptr)) != SimpleDHTErrSuccess) {
        t = h = -1;
    }

    return {t, h};
}

th::data th::getInside() {
    return get(dht_in);
}

th::data th::getOutside() {
    return get(dht_out);
}
