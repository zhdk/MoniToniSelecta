void carrouselLock()
{
    delay(CarrouselDELAY);
    control->set(Lock_CH, LOW);
    carrouselUnlockedState = false;
}

void carrouselUnlock()
{
    control->set(Lock_CH, HIGH);
    //delay(CarrouselDELAY);
    carrouselUnlockedState = true;
}


void motorOn()
{
    control->set(Motor_CH, HIGH);
    motorOnState = true;
}

void motorOff()
{
    control->set(Motor_CH, LOW);
    motorOnState = false;
}


void lightOn()
{
    control->set(Light_CH, HIGH);
    lightOnState = true;
}

void lightOff()
{
    control->set(Light_CH, LOW);
    lightOnState = false;
}


void sireneOn()
{
    control->set(Sirene_CH, HIGH);
    sireneOnState = true;
}

void sireneOff()
{
    control->set(Sirene_CH, LOW);
    sireneOnState = false;
}


void itemUnlock(uint8_t item)
{
  itemUnlockedState = true;
  switch (item)
    {
    case 1:
      Serial.println("Solenoid 1");
      control->set(Item_1_CH, HIGH);
      break;
    case 2:
      Serial.println("Solenoid 2");
      control->set(Item_2_CH, HIGH);
      break;
    case 3:
      Serial.println("Solenoid 3");
      control->set(Item_3_CH, HIGH);
      break;
    case 4:
      Serial.println("Solenoid 4");
      control->set(Item_4_CH, HIGH);
      break;
    case 5:
      Serial.println("Solenoid 5");
      control->set(Item_5_CH, HIGH);
      break;
    case 6:
      Serial.println("Solenoid 6");
      control->set(Item_6_CH, HIGH);
      break;
    case 7:
      Serial.println("Solenoid 7");
      control->set(Item_7_CH, HIGH);
      break;
    case 8:
      Serial.println("Solenoid 8");
      control->set(Item_8_CH, HIGH);
      break;
    case 9:
      Serial.println("Solenoid 9");
      control->set(Item_9_CH, HIGH);
      break;
    case 10:
      Serial.println("Solenoid 10");
      control->set(Item_10_CH, HIGH);
      break;
    default:
      break;
    }
}

void itemLock()
{
  itemUnlockedState = false;
  control->set(Item_1_CH, LOW);
  control->set(Item_2_CH, LOW);
  control->set(Item_3_CH, LOW);
  control->set(Item_4_CH, LOW);
  control->set(Item_5_CH, LOW);
  control->set(Item_6_CH, LOW);
  control->set(Item_7_CH, LOW);
  control->set(Item_8_CH, LOW);
  control->set(Item_9_CH, LOW);
  control->set(Item_10_CH, LOW);
}


void errorOn()
{
  errorOnState = true;
  digitalWrite(Error_PIN, HIGH);
}

void errorOff()
{
  errorOnState = false;
  digitalWrite(Error_PIN, LOW);
}


static void buttonTurnSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    buttonTurnPushedState = true;
    timerButtonTurnPress.stop();
    return;
  }

  // btnState == BTN_OPEN.
  buttonTurnPushedState = false;
  timerButtonTurnPress.stop();
  timerButtonTurnPress.start();
}

static void doorSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_OPEN) {
    doorOpenState = true;
    timerDoorOpen.stop();
    return;
  }

  // btnState == BTN_PRESSED.
  doorOpenState = false;
  timerDoorOpen.stop();
  timerDoorOpen.start();
}

static void buttonOpenSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    buttonOpenPushedState = true;
    timerButtonOpenPress.stop();
    return;
  }

  // btnState == BTN_OPEN.
  buttonOpenPushedState = false;
  timerButtonOpenPress.stop();
  timerButtonOpenPress.start();
}

static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
static Button doorSwitch(1, doorSwitchHandler);
static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);


static void updateInputs() {
  // update() will call buttonHandler() if PIN transitions to a new state and stays there
  // for multiple reads over 25+ ms.
  buttonTurnSwitch.update(digitalRead(ButtonTurn_PIN));
  doorSwitch.update(digitalRead(Door_PIN));
  buttonOpenSwitch.update(digitalRead(ButtonOpen_PIN));
}