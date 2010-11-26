//
//  i2c245.h
//

/**
 * Default delay length
 */
enum {
    DELAY_NSEC = 6000000
};

/**
 * Device handle
 */
static struct ftdi_context ftdic;

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
    int nsec;
} tv_nsec;

int i2c245_init_device(int vendor, int product, int scl, int sda_in, int sda_out);
int i2c245_close_device();
int i2c245_start();
int i2c245_stop();
int i2c245_set_delay(int nsec);

static int set_scl_high();
static int set_scl_low();
static int set_sda_high();
static int set_sda_low();
static int get_sda();
static int delay(double multiple);
