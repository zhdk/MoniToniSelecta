
// // _____________Hardware Functions_____________

// void carrouselLock()
// {
//   sendModbusClose(Lock_CH);
//   LOG_TRACE("LOGIC: carrouselUnlockedState set to false");
//   carrouselUnlockedState = false;
// }

// void carrouselUnlock()
// {
//   sendModbusOpen(Lock_CH);
//   LOG_TRACE("HARDWARE: CarrouselDELAY");
//   delay(CarrouselDELAY);
//   LOG_TRACE("LOGIC: carrouselUnlockedState set to true");
//   carrouselUnlockedState = true;
// }


// void motorOn()
// {
//   sendModbusOpen(Motor_CH);
//   LOG_TRACE("LOGIC: motorOnState set to true");
//   motorOnState = true;
// }

// void motorOff()
// {
//   sendModbusClose(Motor_CH);
//   LOG_TRACE("LOGIC: motorOnState set to false");
//   motorOnState = false;
// }


// void lightOn()
// {
//   sendModbusOpen(Light_CH);
//   LOG_TRACE("LOGIC: lightOnState set to true");
//   lightOnState = true;
// }

// void lightOff()
// {
//   sendModbusClose(Light_CH);
//   LOG_TRACE("LOGIC: lightOnState set to false");
//   lightOnState = false;
// }


// void sireneOn()
// {
//   // sendModbusOpen(Sirene_CH);
//   LOG_TRACE("LOGIC: sireneOnState set to true");
//   sireneOnState = true;
// }

// void sireneOff()
// {
//   // sendModbusClose(Sirene_CH);
//   LOG_TRACE("LOGIC: sireneOnState set to false");
//   sireneOnState = false;
// }


// void itemUnlock(int item)
// {
//   LOG_TRACE("LOGIC: itemUnlockedState set to true");
//   itemUnlockedState = true;
//   LOG_TRACE("HARDWARE: Solenoid ,", item, " triggered");
//   sendModbusOpen(item);
// }

// void itemLock(int item)
// {
//   LOG_TRACE("LOGIC: itemUnlockedState set to false");
//   itemUnlockedState = false;
//   LOG_TRACE("HARDWARE: Solenoid ,", item, " released");
//   sendModbusClose(item);
// }


// // void errorOn()
// // {
// //   LOG_TRACE("LOGIC: errorOnState set to true");
// //   errorOnState = true;
// //   digitalWrite(Error_PIN, HIGH);
// // }

// // void errorOff()
// // {
// //   LOG_TRACE("LOGIC: errorOnState set to false");
// //   errorOnState = false;
// //   digitalWrite(Error_PIN, LOW);
// // }


// // void relay15On()
// // {
// //   digitalWrite(Relay15_CH, LOW);
// //   LOG_TRACE("HARDWARE: RELAY15 ON");
// // }

// // void relay15Off()
// // {
// //   digitalWrite(Relay15_CH, HIGH);
// //   LOG_TRACE("HARDWARE: RELAY15 OFF");
// // }

// // void relay16On()
// // {
// //   digitalWrite(Relay16_CH, LOW);
// //   LOG_TRACE("HARDWARE: RELAY16 ON");
  
// // }

// // void relay16Off()
// // {
// //   digitalWrite(Relay16_CH, HIGH);
// //   LOG_TRACE("HARDWARE: RELAY16 OFF");
// }

// /////////////////////////////////////Based on system Setup included in main file
// // static void buttonTurnSwitchHandler(uint8_t btnId, uint8_t btnState) {
// //   if (btnState == BTN_PRESSED) {
// //     LOG_TRACE("LOGIC: buttonTurnPushedState set to true");
// //     buttonTurnPushedState = true;
// //     LOG_TRACE("TIMER: Button Turn Press stopped");
// //     timerButtonTurnPress.stop();
// //     return;
// //   }

// //   // btnState == BTN_OPEN.
// //   LOG_TRACE("LOGIC: buttonTurnPushedState set to false");
// //   buttonTurnPushedState = false;
// //   LOG_TRACE("TIMER: Button Turn Press started");
// //   timerButtonTurnPress.stop();
// //   timerButtonTurnPress.start();
// // }

// // static void doorSwitchHandler(uint8_t btnId, uint8_t btnState) {
// //   if (btnState == BTN_OPEN) {
// //     LOG_TRACE("LOGIC: doorOpenState set to true");
// //     doorOpenState = true;
// //     timerDoorOpen.stop();
// //     return;
// //   }

// //   // btnState == BTN_PRESSED.
// //   LOG_TRACE("LOGIC: doorOpenState set to false");
// //   doorOpenState = false;
// //   LOG_TRACE("TIMER: Door Open Timer started");
// //   timerDoorOpen.stop();
// //   timerDoorOpen.start();
// // }

// // static void buttonOpenSwitchHandler(uint8_t btnId, uint8_t btnState) {
// //   if (btnState == BTN_PRESSED) {
// //     LOG_TRACE("LOGIC: buttonOpenPushedState set to true");
// //     buttonOpenPushedState = true;
// //     LOG_TRACE("TIMER: Button Open Press Timer stopped");
// //     timerButtonOpenPress.stop();
// //     return;
// //   }

// //   // btnState == BTN_OPEN.
// //   LOG_TRACE("LOGIC: buttonOpenPushedState set to false");
// //   buttonOpenPushedState = false;
// //   LOG_TRACE("TIMER: Button Open Press Timer started");
// //   timerButtonOpenPress.stop();
// //   timerButtonOpenPress.start();
// // }


// /////////////////////////////////////Based on system Setup included in main file
// // static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
// // static Button doorSwitch(1, doorSwitchHandler);
// // static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);


// static void updateInputs() {
//   // update() will call buttonHandler() if PIN transitions to a new state and stays there
//   // for multiple reads over 25+ ms.
//   if (vendingState == 4 || vendingState == 5) {
//     doorSwitch.update(digitalRead(Door_PIN));
//     return;
//   }
//   // buttonTurnSwitch.update(digitalRead(ButtonTurn_PIN));
//   // buttonOpenSwitch.update(digitalRead(ButtonOpen_PIN));
// }