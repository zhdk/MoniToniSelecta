//// _____________MODBUS Functions_____________

// void sendModbusOpen(int relayNumber) {
//   unsigned char relayNumberAddress = relayNumber - 1;
//   unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
//   unsigned int   checksumModbus;
//   unsigned char sendByteModbus;

//   cmdModbus[2] = 0;
//   cmdModbus[3] = relayNumberAddress;
//   cmdModbus[4] = 0xFF;
//   cmdModbus[5] = 0;
//   checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
//   cmdModbus[6] = checksumModbus & 0xFF;
//   cmdModbus[7] = checksumModbus >> 8;
//   for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
//     Serial2.write(cmdModbus[sendByteModbus]);
//   }
//   delay(100);
// }

// void sendModbusClose(int relayNumber) {
//   unsigned char relayNumberAddress = relayNumber - 1;
//   unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
//   unsigned int   checksumModbus;
//   unsigned char sendByteModbus;

//   cmdModbus[2] = 0;
//   cmdModbus[3] = relayNumberAddress;
//   cmdModbus[4] = 0;
//   cmdModbus[5] = 0;
//   checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
//   cmdModbus[6] = checksumModbus & 0xFF;
//   cmdModbus[7] = checksumModbus >> 8;
//   for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
//     Serial2.write(cmdModbus[sendByteModbus]);
//   }
//   delay(100);
// }

// // select command to receive by setting argument cmd. 0 = receive address, 1 = receive baudrate, 
// void receiveModbus(int cmd) {

// }