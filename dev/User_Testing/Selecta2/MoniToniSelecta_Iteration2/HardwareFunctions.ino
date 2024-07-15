// _____________Hardware Functions_____________

void carrouselLock()
{
    digitalWrite(Lock_CH, HIGH);
    LOG_TRACE("LOGIC: carrouselUnlockedState set to false");
    carrouselUnlockedState = false;
}

void carrouselUnlock()
{
    digitalWrite(Lock_CH, LOW);
    LOG_TRACE("HARDWARE: CarrouselDELAY");
    delay(CarrouselDELAY);
    LOG_TRACE("LOGIC: carrouselUnlockedState set to true");
    carrouselUnlockedState = true;
}


void motorOn()
{
    digitalWrite(Motor_CH, LOW);
    LOG_TRACE("LOGIC: motorOnState set to true");
    motorOnState = true;
}

void motorOff()
{
    digitalWrite(Motor_CH, HIGH);
    LOG_TRACE("LOGIC: motorOnState set to false");
    motorOnState = false;
}


void lightOn()
{
    digitalWrite(Light_CH, LOW);
    LOG_TRACE("LOGIC: lightOnState set to true");
    lightOnState = true;
}

void lightOff()
{
    digitalWrite(Light_CH, HIGH);
    LOG_TRACE("LOGIC: lightOnState set to false");
    lightOnState = false;
}


void sireneOn()
{
    digitalWrite(Sirene_CH, LOW);
    LOG_TRACE("LOGIC: sireneOnState set to true");
    sireneOnState = true;
}

void sireneOff()
{
    digitalWrite(Sirene_CH, HIGH);
    LOG_TRACE("LOGIC: sireneOnState set to false");
    sireneOnState = false;
}


void itemUnlock(uint8_t item)
{
  LOG_TRACE("LOGIC: itemUnlockedState set to true");
  itemUnlockedState = true;
  switch (item)
    {
    case 1:
      LOG_TRACE("HARDWARE: Solenoid 1 triggered");
      digitalWrite(Item_1_CH, LOW);
      break;
    case 2:
      LOG_TRACE("HARDWARE: Solenoid 2 triggered");
      digitalWrite(Item_2_CH, LOW);
      break;
    case 3:
      LOG_TRACE("HARDWARE: Solenoid 3 triggered");
      digitalWrite(Item_3_CH, LOW);
      break;
    case 4:
      LOG_TRACE("HARDWARE: Solenoid 4 triggered");
      digitalWrite(Item_4_CH, LOW);
      break;
    case 5:
      LOG_TRACE("HARDWARE: Solenoid 5 triggered");
      digitalWrite(Item_5_CH, LOW);
      break;
    case 6:
      LOG_TRACE("HARDWARE: Solenoid 6 triggered");
      digitalWrite(Item_6_CH, LOW);
      break;
    case 7:
      LOG_TRACE("HARDWARE: Solenoid 7 triggered");
      digitalWrite(Item_7_CH, LOW);
      break;
    case 8:
      LOG_TRACE("HARDWARE: Solenoid 8 triggered");
      digitalWrite(Item_8_CH, LOW);
      break;
    case 9:
      LOG_TRACE("HARDWARE: Solenoid 9 triggered");
      digitalWrite(Item_9_CH, LOW);
      break;
    case 10:
      LOG_TRACE("HARDWARE: Solenoid 10 triggered");
      digitalWrite(Item_10_CH, LOW);
      break;
    default:
      break;
    }
}

void itemLock()
{
  LOG_TRACE("LOGIC: itemUnlockedState set to false");
  itemUnlockedState = false;
  LOG_TRACE("HARDWARE: Lock All Items (Setting PINS HIGH)");
  digitalWrite(Item_1_CH, HIGH);
  digitalWrite(Item_2_CH, HIGH);
  digitalWrite(Item_3_CH, HIGH);
  digitalWrite(Item_4_CH, HIGH);
  digitalWrite(Item_5_CH, HIGH);
  digitalWrite(Item_6_CH, HIGH);
  digitalWrite(Item_7_CH, HIGH);
  digitalWrite(Item_8_CH, HIGH);
  digitalWrite(Item_9_CH, HIGH);
  digitalWrite(Item_10_CH, HIGH);
}


void errorOn()
{
  LOG_TRACE("LOGIC: errorOnState set to true");
  errorOnState = true;
  digitalWrite(Error_PIN, HIGH);
}

void errorOff()
{
  LOG_TRACE("LOGIC: errorOnState set to false");
  errorOnState = false;
  digitalWrite(Error_PIN, LOW);
}


void relay15On()
{
  digitalWrite(Relay15_CH, LOW);
  LOG_TRACE("HARDWARE: RELAY15 ON");
}

void relay15Off()
{
  digitalWrite(Relay15_CH, HIGH);
  LOG_TRACE("HARDWARE: RELAY15 OFF");
}

void relay16On()
{
  digitalWrite(Relay16_CH, LOW);
  LOG_TRACE("HARDWARE: RELAY16 ON");
  
}

void relay16Off()
{
  digitalWrite(Relay16_CH, HIGH);
  LOG_TRACE("HARDWARE: RELAY16 OFF");
}

static void buttonTurnSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    LOG_TRACE("LOGIC: buttonTurnPushedState set to true");
    buttonTurnPushedState = true;
    LOG_TRACE("TIMER: Button Turn Press stopped");
    timerButtonTurnPress.stop();
    return;
  }

  // btnState == BTN_OPEN.
  LOG_TRACE("LOGIC: buttonTurnPushedState set to false");
  buttonTurnPushedState = false;
  LOG_TRACE("TIMER: Button Turn Press started");
  timerButtonTurnPress.stop();
  timerButtonTurnPress.start();
}

static void doorSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_OPEN) {
    LOG_TRACE("LOGIC: doorOpenState set to true");
    doorOpenState = true;
    timerDoorOpen.stop();
    return;
  }

  // btnState == BTN_PRESSED.
  LOG_TRACE("LOGIC: doorOpenState set to false");
  doorOpenState = false;
  LOG_TRACE("TIMER: Door Open Timer started");
  timerDoorOpen.stop();
  timerDoorOpen.start();
}

static void buttonOpenSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    LOG_TRACE("LOGIC: buttonOpenPushedState set to true");
    buttonOpenPushedState = true;
    LOG_TRACE("TIMER: Button Open Press Timer stopped");
    timerButtonOpenPress.stop();
    return;
  }

  // btnState == BTN_OPEN.
  LOG_TRACE("LOGIC: buttonOpenPushedState set to false");
  buttonOpenPushedState = false;
  LOG_TRACE("TIMER: Button Open Press Timer started");
  timerButtonOpenPress.stop();
  timerButtonOpenPress.start();
}

static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
static Button doorSwitch(1, doorSwitchHandler);
static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);


static void updateInputs() {
  // update() will call buttonHandler() if PIN transitions to a new state and stays there
  // for multiple reads over 25+ ms.
  if (vendingState == 4 || vendingState == 5) {
    doorSwitch.update(digitalRead(Door_PIN));
    return;
  }
  buttonTurnSwitch.update(digitalRead(ButtonTurn_PIN));
  buttonOpenSwitch.update(digitalRead(ButtonOpen_PIN));
}