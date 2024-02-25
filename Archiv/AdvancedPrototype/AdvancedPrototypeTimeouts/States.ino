// _LOGIC_VARIABLES_
//
// vendingState - 0 = Sleep, 1 = Idle, 2 = Turn, 3 = Validate, 4 = Collect, 5 = Finished, 6 = Error
//
// permission bool
// completed  bool
// closed bool
//
// item int
//
// doorOpen bool
// buttonTurnPushed bool
// transactionActive  bool
//
// _FUNCTIONS_
//
// void carrouselLock()
// void carrouselUnlock()
//
// void motorOn()
// void motorOff()
//
// void lightOn()
// void lightOff()
//
// void sireneOn()
// void sireneOff()
//
// void itemUnlock(uint8_t item)
// void itemLock()
//
// void errorOn()
// void errorOff()


void vendingSleep() {
  timerSleep.stop();
  carrouselLock();
  motorOff();
  lightOff();
  sireneOff();
  itemLock();

  if (buttonTurnPushedState) {
    timerSleep.stop();
    timerSleep.start();
    vendingState = 1; // Idle
    Serial.println("Idle");
    return;
  }
}


void vendingIdle() {
  if (timerSleep.read() > SleepDELAY) {
    timerSleep.stop();
    vendingState = 0; // Sleep
    Serial.println("Sleep");
    return;
  }

  carrouselLock();
  motorOff();
  lightOn();
  sireneOff();
  itemLock();

  if (buttonTurnPushedState) {
    vendingState = 2; // Turn
    Serial.println("Turn");
    return;
  }
  if (buttonOpenPushedState) {
    vendingState = 3; // Validate
    Serial.println("Validate");
    return;
  }
}


void vendingTurn() {
  if (!carrouselUnlockedState) {
    carrouselUnlock();
    return;
  }

  if (buttonTurnPushedState) {
    if (!motorOnState) {
      motorOn();
      return;
    }
    return;
  }
  else {
    carrouselLock();
    delay(MotorTurnDELAY);
    motorOff();
    carrouselLock();
    timerSleep.stop();
    timerSleep.start();

    vendingState = 1; // Idle
    Serial.println("Idle");
    return;
  }
}


void vendingValidate() {
  // Server Timeout
  
  if (timerServerTimeout.read() > ServerTimeout) {
    timerServerTimeout.stop();
    transactionActive = false;
    requestActive = false;
    vendingState = 6; // Error
    Serial.println("Error");
    return;
  }
  

  if (!transactionActive) {
    permissionRequest();
    requestActive = true;
  }
  else if(transactionActive) {
    timerServerTimeout.stop();
    requestActive = false;
    vendingState = 4; // Collect
    Serial.println("Collect");
    return;
  }
}


void vendingCollect(){
  // Purchase Timeout
  if (timerPurchaseTimeout.read() > PurchaseTimeoutDELAY) {
    timerPurchaseTimeout.stop();
    transactionActive = false;
    timerSleep.stop();
    timerSleep.start();

    vendingState = 1; // Idle
    Serial.println("collect --> Idle");
    return;
  }

  if (!itemUnlockedState && !doorOpenState) {
    itemUnlock(item);
    return;
  }

  if (doorOpenState && transactionActive) {
    delay(openDoorDELAY);
    itemLock();
    if( completeRequest() ) {
      timerPurchaseTimeout.stop();
      timerServerTimeout.start();
      transactionActive = false;
      vendingState = 5; // Finished
      Serial.println("Finished");
      return;
    }
  }
}


void vendingFinished() {
  if (timerDoorOpen.read() > DoorOpenSireneDELAY) {
    sireneOn();
    timerDoorOpen.stop();
  }

  if (!doorOpenState) {
    sireneOff();
    timerDoorOpen.stop();
    if ( closeRequest()) {
      timerServerTimeout.stop();
      timerSleep.stop();
      timerSleep.start();
      vendingState = 1; // Idle
      Serial.println("Finished --> Idle");
      return;
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      timerServerTimeout.stop();
      vendingState = 6; // Error
      Serial.println("Error Finished");
      return;
    }
  }
}


void vendingError() {
  carrouselLock();
  motorOff();
  lightOff();
  sireneOff();
  itemLock();
  errorOn();

  vendingState = 0; // Sleep
  Serial.println("Error --> Sleep");
}
