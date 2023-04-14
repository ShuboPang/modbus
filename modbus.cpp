#include "modbus.h"
#include "crc.h"

void printByteToHex(const char* note, const unsigned char* source,
                    int source_len, const char* file, const char* function,
                    int line);

///
/// \brief ByteToHexStr  字节流转换为十六进制字符串，每个字节直接用空格分割
/// \param source        字节数组
/// \param dest          HEX数组
/// \param sourceLen     字节长度
///
void ByteToHexStr(const unsigned char* source, char* dest, int source_len) {
  uint16_t i;
  unsigned char highByte, lowByte;

  for (i = 0; i < source_len; i++) {
    highByte = source[i] >> 4;
    lowByte = source[i] & 0x0f;

    highByte += 0x30;

    if (highByte > 0x39) {
      dest[i * 3] = highByte + 0x07;
    } else {
      dest[i * 3] = highByte;
    }

    lowByte += 0x30;
    if (lowByte > 0x39) {
      dest[i * 3 + 1] = lowByte + 0x07;
    } else {
      dest[i * 3 + 1] = lowByte;
    }

    dest[i * 3 + 2] = ' ';
  }
  return;
}

///
/// \brief HexStrToByte 十六进制字符串转换为字节流
/// \param source
/// \param dest
/// \param sourceLen
///
void HexStrToByte(const char* source, unsigned char* dest, int source_len) {
  uint16_t i;
  unsigned char highByte, lowByte;

  for (i = 0; i < source_len; i += 2) {
    highByte = toupper(source[i]);
    lowByte = toupper(source[i + 1]);

    if (highByte > 0x39) {
      highByte -= 0x37;
    } else {
      highByte -= 0x30;
    }

    if (lowByte > 0x39) {
      lowByte -= 0x37;
    } else {
      lowByte -= 0x30;
    }

    dest[i / 2] = (highByte << 4) | lowByte;
  }
  return;
}

///
/// \brief printByteToHex
/// \param note
/// \param source
/// \param source_len
/// \param file
/// \param function
/// \param line
///
void printByteToHex(const char* note, const unsigned char* source,
                    int source_len, const char* file, const char* function,
                    int line) {
  char* hex_str = new char[source_len * 3 + 1];
  ByteToHexStr(source, hex_str, source_len);
  std::cout << file << ":" << function << "(line:" << line << "):      " << note
            << "  HEX(" << source_len << "):" << hex_str << std::endl;
  delete[] hex_str;
}

Modbus::Modbus() { modbus_type_ = kModbusRtu; }

Modbus::Modbus(Modbus::ModbusType type) { modbus_type_ = type; }

unsigned int Modbus::addHeaderAndTailMessage(unsigned char* ptr,
                                             unsigned char len) {
  if (modbus_type_ == kModbusTcp) {
    return addTcpHeader(ptr, len);
  } else if (modbus_type_ == kModbusRtu) {
    return Crc::addCrc16(ptr, len);
  }
}

unsigned int Modbus::addTcpHeader(unsigned char* ptr, unsigned char len) {
  unsigned char tmp[MODBUS_MAX_PRIVATE_BUFFER_LEN] = {0};
  memcpy(tmp + 6, ptr, len);
  memset(ptr, 0, len + 6);
  memcpy(ptr, tmp, len + 6);
  ptr[1] = 1;
  ptr[5] = len;
  //  ptr[6] = len & 0xff;
  return len + 6;
}
