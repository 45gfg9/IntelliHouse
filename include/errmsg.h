#pragma once

#include <avr/pgmspace.h>

extern const char *SHERR_P PROGMEM;
extern const char *SHERR_CON_P PROGMEM;
extern const char *SHERR_HTTP_CON_P PROGMEM;
extern const char *SHERR_HTTP_GET_P PROGMEM;
extern const char *SHERR_NC_P PROGMEM;

#define SHERR FPSTR(SHERR_P)
#define SHERR_CON FPSTR(SHERR_CON_P)
#define SHERR_HTTP_CON FPSTR(SHERR_HTTP_CON_P)
#define SHERR_HTTP_GET FPSTR(SHERR_HTTP_GET_P)
#define SHERR_NC FPSTR(SHERR_NC_P)
