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
    Serial.println("sleep --> Idle");
    return;
  }
  if (buttonOpenPushedState) {
    vendingState = 3; // Validate
    timerServerTimeout.stop();
    timerServerTimeout.start();
    Serial.println("idle --> Validate");
    return;
  }
}


void vendingIdle() {
  if (timerSleep.read() > SleepDELAY) {
    timerSleep.stop();
    vendingState = 0; // Sleep
    Serial.println("idle --> Sleep");
    return;
  }

  carrouselLock();
  motorOff();
  lightOn();
  sireneOff();
  itemLock();

  if (buttonTurnPushedState) {
    vendingState = 2; // Turn
    Serial.println("idle --> Turn");
    return;
  }
  if (buttonOpenPushedState) {
    vendingState = 3; // Validate
    timerServerTimeout.stop();
    timerServerTimeout.start();
    Serial.println("idle --> Validate");
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
    Serial.println("turn --> Idle");
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
    Serial.println("validate --> Error");
    return;
  }
  

  if (!transactionActive) {
    permissionRequest();
    Serial.println("didrequest");
    requestActive = true;
  }
  else if(transactionActive) {
    timerServerTimeout.stop();
    requestActive = false;
    vendingState = 4; // Collect
    Serial.println("validate --> Collect");
    return;
  }
}


void vendingCollect(){
  // Purchase Timeout
  //if (timerPurchaseTimeout.read() > PurchaseTimeoutDELAY) {
  if (!permissionRequest()) {
    //timerPurchaseTimeout.stop();
    transactionActive = false;
    timerSleep.stop();
    timerSleep.start();

    vendingState = 6; // Error
    Serial.println("collect --> error");
    return;
  }

  if (!itemUnlockedState && !doorOpenState) {
    itemUnlock(item);
    timerServerTimeout.start(); //NU
    return;
  }

  if (doorOpenState && transactionActive) {
    delay(openDoorDELAY);
    itemLock();
    if( completeRequest() ) {
      //timerPurchaseTimeout.stop();
      timerServerTimeout.stop(); //NU
      timerServerTimeout.start(); //NU
      timerDoorOpen.start();
      transactionActive = false;
      vendingState = 5; // Finished
      Serial.println("collect --> finished");
      return;
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      timerServerTimeout.stop();
      vendingState = 6; // Error
      Serial.println("collect --> error");
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
      Serial.println("Finished --> error");
      return;
    }
  }
}


void vendingError() {
  timerServerTimeout.stop();
  //timerPurchaseTimeout.stop();
  carrouselLock();
  motorOff();
  lightOn();
  sireneOff();
  itemLock();

  // Error LED
  for (int i = 0; i < BLINKS; i++) {
    errorOn();
    delay(200);
    errorOff();
    delay(100);
  }

  transactionActive = false;
  requestActive = false;
  vendingState = 1; // Sleep
  Serial.println("Error --> Idle");
}
