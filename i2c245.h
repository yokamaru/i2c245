//
//  i2c245.h
//

#include <ftdi.h>
#include <time.h>

/**
 * Default delay length
 */
enum {
    DELAY_NSEC = 6000000
};

/**
 * Device handle
 */
struct ftdi_context ftdic;

/**
 * Pin assign
 */
static struct {
    int scl;
    int sda_in;
    int sda_out;
} pin_assign;

/**
 * Delay length[nanosec]
 */
static struct {
    time_t sec;
    long nsec;
} tv_delay;

int i2c245_init_device(int vendor, int product, int scl, int sda_in, int sda_out);
int i2c245_close_device();
int i2c245_start();
int i2c245_stop();
int i2c245_write(unsigned char *data);
int i2c245_read_nack(unsigned char *read_data);
int i2c245_read_ack(unsigned char *read_data);
int i2c245_set_delay(time_t sec, long nsec);

static int set_scl_high(unsigned char *buf, int size);
static int set_scl_low(unsigned char *buf, int size);
static int set_sda_high(unsigned char *buf, int size);
static int set_sda_low(unsigned char *buf, int size);
static int get_sda();
static int getbyte_sda(unsigned char *state_buf, int state_buf_size,
                       unsigned char *read_data);
static int delay(double multiple);
