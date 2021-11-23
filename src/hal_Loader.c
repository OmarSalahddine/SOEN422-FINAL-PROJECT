#include "hal_Loader.h"
#include "bsl_Uart.h"
#include "COutForAUnit.h"

static char pingPacketChecksumInvalid[] = {0x03, 0xee, Ping, 0};
static char pingPacket[] = {0x03, 0x20, Ping, 0};
static char getStatusPacket[] = {0x03, 0x23, GetStatus, 0};
static char sendDataPacket[] = {0x09, 0xBC, 0x24, 0x91, 0xFF, 0x82, 0xFF, 0x87, 0x00, 0};
static char runPacket[] = {0x03, 0x22, Run, 0};

static bool _continue;
static bool _run;

static char buffer[12];

#if defined(Host)

#endif