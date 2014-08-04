#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include <stdint.h>
#include <linux/spi/spidev.h>

extern int spi_open(const char *dev, uint8_t mode, uint8_t bpw, uint32_t speed);
extern int spi_close(int fd);
extern int spi_transfer(int fd, struct spi_ioc_transfer *msgs, int n);
extern int spi_message(int fd, char *tx, char *rx, uint32_t len, uint16_t delay, bool cs_change);

#endif
