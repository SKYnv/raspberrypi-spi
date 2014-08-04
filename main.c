#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "spi.h"

#define BUFSIZE 1024

#define FRAME_START 0x01
#define FRAME_STOP  0x02
#define FRAME_NEXT  0x03

char *
bufinit(char *buf, size_t len)
{
    return memset(buf, 0, len);
}

size_t
buflen(char *buf, char *p)
{
    return p - buf;
}

char *
str(char *p, const char *data, size_t len)
{
    return memcpy(p, data, len) + len;
}

char *
frame(char *p, uint8_t type, const char *data, uint8_t len)
{
    *(p++) = type;
    *(p++) = len;
    return str(p, data, len);
}

char *
cmd(char *p, uint8_t ttl, uint8_t cmd, uint16_t reg, uint32_t val)
{
    *(p++) = 0x10 | (ttl & 0x0f);
    *(p++) = cmd;
    *(p++) = (reg >>  8) & 0xff;
    *(p++) =        reg  & 0xff;
    *(p++) = (val >> 24) & 0xff;
    *(p++) = (val >> 16) & 0xff;
    *(p++) = (val >>  8) & 0xff;
    *(p++) =        val  & 0xff;
    return p;
}

int
test(int spi, int n)
{
    char tx[BUFSIZE];
    char rx[BUFSIZE];

    char *p;

    bufinit(rx, BUFSIZE);

    p = bufinit(tx, BUFSIZE);
    p = frame(p, FRAME_START, "biltong", 8);
    p = frame(p, FRAME_STOP,  "biltong", 8);
    size_t l = buflen(tx, p);

    if (spi_message(spi, tx, rx, l + n, 0, false) < 0)
    {
        perror("spi_message");
        return -1;
    }

    printf("%d:%02x%02x%02x%02x %-8.8s\n", n, rx[n+0], rx[n+1], rx[n+2], rx[n+3], rx+n+2);

    return (memcmp(tx, rx + n, l) == 0);
}

int
test2(int spi, int n, int i, uint32_t v)
{
    char tx[BUFSIZE];
    char rx[BUFSIZE];

    char *p;

    bufinit(rx, BUFSIZE);

    p = bufinit(tx, BUFSIZE);
    p = cmd(p, i, 1, 0, v);
    size_t l = buflen(tx, p);

    if (spi_message(spi, tx, rx, l + n, 0, false) < 0)
    {
        perror("spi_message");
        return -1;
    }

    return 0;
}

void
dump(char *p, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%02x ", p[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
        else if ((i + 1) % 4 == 0)
            printf("  ");
    }
    if (n % 8)
        printf("\n");
}

int
test3(int spi, int n, uint32_t v)
{
    char tx[BUFSIZE];
    char rx[BUFSIZE];

    char *p;

    bufinit(rx, BUFSIZE);

    p = bufinit(tx, BUFSIZE);
    p = cmd(p, 1, 1, 0, v);
    p = cmd(p, 2, 1, 0, v);
    size_t l = buflen(tx, p);

    if (spi_message(spi, tx, rx, l + n, 0, false) < 0)
    {
        perror("spi_message");
        return -1;
    }

    tx[0] = 0x10;
    tx[8] = 0x10;

    if (memcmp(tx, rx + 2, l) == 0)
        printf("MATCH\n");
    else
        dump(rx, l + n);
    printf(".\n");

    return 0;
}

int
main(void)
{
    int spi = spi_open("/dev/spidev0.0", SPI_MODE_0, 8, 400000);

    assert(spi >= 0);

    /*
    for (int i = 0; i < 10; i++)
    {
        if (test(spi, i) > 0)
            printf("%d MATCH\n", i);
        usleep(100000);
    }
    */
    for (int i = 0; i < 50; i++)
    {
        test3(spi, 8, 0xffff);
        usleep(40000);
        test3(spi, 8, 0);
        usleep(60000);
    }

    spi_close(spi);

    return 0;
}
