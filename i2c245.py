import ctypes

class i2c245:
    def __init__(
        self,
        vendor = 0x0403,
        product = 0x6001,
        scl = 0,
        sda_in = 5,
        sda_out = 4):

        """
        Initialize FT245RL
        """
        self.lib = ctypes.CDLL('libi2c245.so')
        self.open_device(vendor, product, scl, sda_in, sda_out)

    def open_device(
        self,
        vendor = 0x0403,
        product = 0x6001,
        scl = 0,
        sda_in = 5,
        sda_out = 4):
        self.lib.i2c245_init_device(vendor, product, scl, sda_in, sda_out)

    def close_device(self):
        self.lib.i2c245_close_device()

    def start(self):
        self.lib.i2c245_start()

    def stop(self):
        self.lib.i2c245_stop()

    def write(self):
        self.lib.i2c245_write()

    def read_nack(self):
        self.lib.i2c245_read_nack()

    def read_ack(self):
        self.lib.i2c245_read_ack()

    def set_delay(self):
        self.lib.i2c245_set_delay()
