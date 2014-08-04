#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/spi/spidev.h>

int
spi_open(const char *dev, uint8_t mode, uint8_t bpw, uint32_t speed)
{
    int fd = open(dev, O_RDWR);

    if (fd < 0) return -1;

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) return -1;
    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0) return -1;

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bpw) < 0) return -1;
    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bpw) < 0) return -1;

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) return -1;
    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) return -1;

    return fd;
}

int
spi_close(int fd)
{
    return close(fd);
}

int
spi_transfer(int fd, struct spi_ioc_transfer *msgs, int n)
{
    return ioctl(fd, SPI_IOC_MESSAGE(n), msgs);
}

int
spi_message(int fd, char *tx, char *rx, uint32_t len, uint16_t delay, bool cs_change)
{
    struct spi_ioc_transfer msg =
    {
        .tx_buf         = (unsigned long)tx,
        .rx_buf         = (unsigned long)rx,
        .len            = len,
        .delay_usecs    = delay,
        .cs_change      = cs_change,
    };

    return spi_transfer(fd, &msg, 1);
}
