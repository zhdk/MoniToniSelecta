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
  carrouselLock();
  motorOff();
  lightOff();
  sireneOff();
  itemLock();

  if (buttonTurnPushedState) {
    vendingState = 1; // Idle
    Serial.println("Idle");
    //delay()---
    return;
  }
}


void vendingIdle() {
  //implement sleep timer

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
    /*
    if (timerButtonTurnPress.read() > UserInputDELAY) {
      vendingState = 3; // Validate
      Serial.println("Validate");
      return;
    }
    */
    vendingState = 2; // Validate
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
    requestActive = false;
    vendingState = 4; // Collect
    Serial.println("Collect");
    return;
  }

  if (buttonTurnPushedState) {
    requestActive = false;
    vendingState = 1; // Idle
    Serial.println("Idle");
    return;
  }

}


void vendingCollect(){
  // Purchase Timeout
  if (timerPurchaseTimeout.read() > PurchaseTimeoutDELAY) {
    timerPurchaseTimeout.stop();
    transactionActive = false;
    vendingState = 1; // Idle
    Serial.println("Idle");
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
    closeRequest();
    vendingState = 1; // Idle
    Serial.println("Idle");
    return;
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
  Serial.println("Sleep");
}
