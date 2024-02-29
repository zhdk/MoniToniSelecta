// _____________State Logic_____________

void vendingSleep() {
  LOG_TRACE("HARDWARE: Stop Sleep Timer");
  timerSleep.stop();
  LOG_TRACE("HARDWARE: Lock Carrousel");
  carrouselLock();
  LOG_TRACE("HARDWARE: Turn Off Motor");
  motorOff();
  LOG_TRACE("HARDWARE: Turn Off Light");
  lightOff();
  LOG_TRACE("HARDWARE: Turn Off Sirene");
  sireneOff();
  LOG_TRACE("HARDWARE: Lock All Items");
  itemLock();

  if (buttonTurnPushedState) {
    LOG_TRACE("LOGIC: Turn Button Pushed");
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 1; // Idle
    LOG_INFO("STATE: Switching to Idle State");
    return;
  }
  if (buttonOpenPushedState) {
    LOG_TRACE("LOGIC: Open Button Pushed");
    vendingState = 3; // Validate
    timerServerTimeout.stop();
    LOG_DEBUG("TIMER: Server Timeout Timer started");
    timerServerTimeout.start();
    LOG_INFO("STATE: Switching to Validate State");
    return;
  }
}


void vendingIdle() {
  if (timerSleep.read() > SleepDELAY) {
    LOG_TRACE("LOGIC: Sleep Timer greater than SleepDELAY");
    timerSleep.stop();
    LOG_DEBUG("TIMER: Sleep Timer stopped");
    vendingState = 0; // Sleep
    LOG_INFO("STATE: Switching to Sleep State");
    return;
  }

  LOG_TRACE("HARDWARE: Lock Carrousel");
  carrouselLock();
  LOG_TRACE("HARDWARE: Turn Off Motor");
  motorOff();
  LOG_TRACE("HARDWARE: Turn On Light");
  lightOn();
  LOG_TRACE("HARDWARE: Turn Off Sirene");
  sireneOff();
  LOG_TRACE("HARDWARE: Lock All Items");
  itemLock();

  if (buttonTurnPushedState) {
    LOG_TRACE("LOGIC: Turn Button Pushed");
    vendingState = 2; // Turn
    LOG_INFO("STATE: Switching to Turn State");
    return;
  }

  if (buttonOpenPushedState) {
    LOG_TRACE("LOGIC: Open Button Pushed");
    vendingState = 3; // Validate
    timerServerTimeout.stop();
    timerServerTimeout.start();
    LOG_DEBUG("TIMER: Server Timeout Timer started");
    LOG_INFO("STATE: Switching to Validate State");
    return;
  }
}


void vendingTurn() {
  if (!carrouselUnlockedState) {
    LOG_TRACE("LOGIC: carrouselUnlockedState false");
    LOG_DEBUG("HARDWARE: Unlock carrousel");
    carrouselUnlock();
    return;
  }

  if (buttonTurnPushedState) {
    LOG_TRACE("LOGIC: Turn Button Pushed");
    if (!motorOnState) {
      LOG_TRACE("LOGIC: motorOnState false");
      LOG_DEBUG("HARDWARE: Turn Motor On");
      motorOn();
      return;
    }
    return;
  }
  else {
    LOG_DEBUG("HARDWARE: Lock carrousel");
    carrouselLock();
    delay(MotorTurnDELAY);
    LOG_TRACE("HARDWARE: MotorTurnDELAY");
    LOG_DEBUG("HARDWARE: Turn Motor Off");
    motorOff();
    //carrouselLock(); -- NOTE: double and not necessary?
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 1; // Idle
    LOG_INFO("STATE: Switching to Idle State");
    return;
  }
}


void vendingValidate() {
  // Server Timeout
  if (timerServerTimeout.read() > ServerTimeout) {
    LOG_TRACE("LOGIC: Server Timeout Timer larger then ServerTimeout");
    timerServerTimeout.stop();
    LOG_DEBUG("TIMER: Server Timeout Timer stopped");
    LOG_DEBUG("LOGIC: transactionActive set to false");
    transactionActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    requestActive = false;
    vendingState = 6; // Error
    LOG_INFO("STATE: Switching to Error State");
    return;
  }
  
  // Call permissionRequest-function when there is no acitve Transaction
  if (!transactionActive) {
    LOG_TRACE("LOGIC: transactionActive is false");
    permissionRequest();
    LOG_DEBUG("LOGIC: call permissionRequest");
    requestActive = true;
    LOG_DEBUG("LOGIC: requestActive set to true");
  }
  // Active Transaction --> Stop timerServerTimeout, Set logic variables & switch State
  else if(transactionActive) {
    LOG_TRACE("LOGIC: transactionActive is true");
    timerServerTimeout.stop();
    LOG_DEBUG("TIMER: Server Timeout Timer stopped");
    requestActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    vendingState = 4; // Collect
    LOG_INFO("STATE: Switching to Collect State");
    return;
  }
}


void vendingCollect(){
  //if (timerPurchaseTimeout.read() > PurchaseTimeoutDELAY) { -- NOTE: Purchase Timeout handled through MoniToni --> timerPurchaseTimeout not necessary
  if (!permissionRequest()) {
    LOG_TRACE("LOGIC: permissionRequest returns false");
    //timerPurchaseTimeout.stop();  -- NOTE: see previous note
    transactionActive = false;
    LOG_DEBUG("LOGIC: transactionActive set to false");
    requestActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 6; // Error
    LOG_INFO("STATE: Switching to Error State");
    return;
  }
  // _____________HIGHT PRIORITY TO DO_____________
  // -- NOTE: if statement to check permissionRequest might not be needed --> permissionRequest already triggered in previous state

  if (!itemUnlockedState && !doorOpenState) {
    LOG_TRACE("LOGIC: itemUnlockedState && doorOpenState are false");
    LOG_DEBUG("HARDWARE: Unlock Item: " + item);
    itemUnlock(item);
    timerServerTimeout.start(); //NU
    LOG_DEBUG("TIMER: Sleep Timer started");
    return;
  }

  if (doorOpenState && transactionActive) {
    LOG_TRACE("LOGIC: doorOpenState && transactionActive are true");
    delay(openDoorDELAY);
    LOG_TRACE("HARDWARE: openDoorDELAY");
    LOG_DEBUG("HARDWARE: Lock Items");
    itemLock();
    if( completeRequest() ) {
      LOG_TRACE("LOGIC: completeRequest returns true");
      //timerPurchaseTimeout.stop();   -- NOTE: see previous note
      timerServerTimeout.stop(); //NU
      timerServerTimeout.start(); //NU
      LOG_DEBUG("TIMER: Server Timeout Timer started");
      timerDoorOpen.start();
      LOG_DEBUG("TIMER: Door Open Timer started");
      transactionActive = false;
      LOG_DEBUG("LOGIC: transactionActive set to false");
      vendingState = 5; // Finished
      LOG_INFO("STATE: Switching to Finished State");
      return;
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      LOG_TRACE("LOGIC: Server Timeout Timer greater than ServerTimeout");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      vendingState = 6; // Error
      LOG_INFO("STATE: Switching to Error State");
      return;
    }
  }
}


void vendingFinished() {
  if (timerDoorOpen.read() > DoorOpenSireneDELAY) {
    LOG_TRACE("LOGIC: Door Open Timer greater than DoorOpenSireneDELAY");
    LOG_DEBUG("HARDWARE: Turn Sirene On");
    sireneOn();
    timerDoorOpen.stop();
    LOG_DEBUG("TIMER: Door Open Timer stopped");
  }

  if (!doorOpenState) {
    LOG_TRACE("LOGIC: doorOpenState is false");
    LOG_DEBUG("HARDWARE: Turn sirene off");
    sireneOff();
    timerDoorOpen.stop();
    LOG_DEBUG("TIMER: Door Open Timer stopped");
    if ( closeRequest() ) {
      LOG_TRACE("LOGIC: closeRequest returned false");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      timerSleep.stop();
      timerSleep.start();
      LOG_DEBUG("TIMER: Sleep Timer started");
      vendingState = 1; // Idle
      LOG_INFO("STATE: Switching to Idle State");
      return;
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      LOG_TRACE("LOGIC: Server Timeout Timer greater than ServerTimeout");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      vendingState = 6; // Error
      LOG_INFO("STATE: Switching to Error State");
      return;
    }
  }
}


void vendingError() {
  LOG_ERROR("ERROR: Error State entered");
  timerServerTimeout.stop();
  LOG_DEBUG("TIMER: Server Timeout Timer stopped");
  //timerPurchaseTimeout.stop();
  LOG_TRACE("HARDWARE: Lock Carrousel");
  carrouselLock();
  LOG_TRACE("HARDWARE: Turn Motor Off");
  motorOff();
  LOG_TRACE("HARDWARE: Turn Lights On");
  lightOn();
  LOG_TRACE("HARDWARE: Turn Sirene Off");
  sireneOff();
  LOG_TRACE("HARDWARE: Lock Items");
  itemLock();

  // Error LED
  LOG_DEBUG("ERROR: Flashing Error Light");
  for (int i = 0; i < BLINKS; i++) {
    errorOn();
    delay(200);
    errorOff();
    delay(100);
  }

  transactionActive = false;
  LOG_DEBUG("LOGIC: requestActive set to false");
  requestActive = false;
  LOG_DEBUG("LOGIC: vendingActive set to false");
  vendingState = 1; // Sleep
  LOG_INFO("STATE: Switching to Sleep State");
}
