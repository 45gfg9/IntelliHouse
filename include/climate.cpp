#include "climate.h"

#include <RTClib.h>

using namespace climate;

const static int DHT_A_PIN = 2;
const static int DHT_B_PIN = 3;

static SimpleDHT11 dht_in(DHT_A_PIN);
static SimpleDHT11 dht_out(DHT_B_PIN);

static dht_data get(SimpleDHT& dht) {
    byte t, h;
    if ((dht.read(&t, &h, nullptr)) != SimpleDHTErrSuccess) {
        t = h = -1;
    }

    return {t, h};
}

dht_data climate::getInside() {
    return get(dht_in);
}

dht_data climate::getOutside() {
    return get(dht_out);
}

class climate::Season {
    const byte INTERVAL = 3;
};

class climate::Spring : public Season {
    const byte MONTH = 2;
};

class climate::Summer : public Season {
    const byte MONTH = 5;
};

class climate::Autumn : public Season {
    const byte MONTH = 8;
};

class climate::Winter : public Season {
    const byte MONTH = 11;
}
