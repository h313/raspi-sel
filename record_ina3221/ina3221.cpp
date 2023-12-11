#include "ina3221.h"
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

inline unsigned int change_endian(unsigned int x) {
  unsigned char *ptr = (unsigned char *)&x;
  return ((ptr[0] << 8) | ptr[1]);
}

inline float shunt_to_amp(int shunt) {
  // sign change for negative value (bit 13 is sign)
  if (shunt > 4096)
    shunt = -(8192 - shunt);

  // shunt raw value to mv (40μV datasheet)
  float amp1mv = 0.0004 * shunt;

  // without external shunt R on device is 0.1 ohm
  return amp1mv / 0.1;
}

INA3221::INA3221() {
  // Setup I2C communication
  i2c = open("/dev/i2c-1", O_RDWR);
  if (i2c == -1)
    return -2;

  if(ioctl(i2c, I2C_SLAVE, 0x40) < 0) {
    close(i2c);
    return -4;
  }

  int check_vendor_id = i2c_smbus_read_word_data(i2c, 0xFF);
  if (check_vendor_id != SIGNATURE)
    return -3;

  // Switch device to measurement mode
  i2c_smbus_write_word_data(i2c, REG_RESET, 0b1111111111111111);
}

INA3221::~INA3221() {
  close(i2c);
}

std::string INA3221::read_currents() {
  std::stringstream ret;
  int shunt1, shunt2, shunt3;

  // Read from INA3221
  shunt1 = i2c_smbus_read_word_data(i2c, REG_DATA_ch1);
  shunt2 = i2c_smbus_read_word_data(i2c, REG_DATA_ch2);
  shunt3 = i2c_smbus_read_word_data(i2c, REG_DATA_ch3);

  // change endian, strip last 3 bits provide raw value
  ret << shunt_to_amp(change_endian(shunt1) / 8) << ",";
  ret << shunt_to_amp(change_endian(shunt2) / 8) << ",";
  ret << shunt_to_amp(change_endian(shunt3) / 8);

  return ret.str();
}
