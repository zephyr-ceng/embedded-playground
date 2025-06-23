#ifndef EEPROM_H
#define EEPROM_H

void EEPROM_IIC_Init(void);
void EEPROM_IIC_WriteByte(uint8_t deviceAddr, uint16_t memAddr, uint8_t data);
void EEPROM_IIC_WriteBytes(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint8_t length);
uint8_t EEPROM_IIC_ReadByte(uint8_t deviceAddr, uint16_t memAddr);
void EEPROM_IIC_ReadBytes(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint8_t length);
void EEPROM_IIC_WritePages(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint16_t length, uint8_t pageSize);
void EEPROM_IIC_ReadPages(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint16_t length, uint8_t pageSize);
#endif // EEPROM_H
