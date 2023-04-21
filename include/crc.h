#ifndef UTILS_CRC_H_
#define UTILS_CRC_H_

#include <stdint.h>

class Crc {
 public:
  Crc();

  ///
  /// \brief crc16    计算Crc16
  /// \param source   数据
  /// \param len      数据长度
  /// \return         crc
  ///
  static uint16_t crc16(const uint8_t* source, uint16_t len);

  ///
  /// \brief crc16Check   判断Crc是否正确
  /// \param source       数据
  /// \param len          数据长度 不带Crc的数据长度
  /// \param crc          实际Crc
  /// \return   Crc是否正确
  ///
  static bool crc16Check(const uint8_t* source, uint16_t len, uint16_t crc);

  ///
  /// \brief addCrc16     为数据添加CRC
  /// \param source       数据指针
  /// \param len          数据长度
  /// \return             带CRC的数据长度
  ///
  static uint16_t addCrc16(uint8_t* source, uint16_t len);
};

#endif  // UTILS_CRC_H_
