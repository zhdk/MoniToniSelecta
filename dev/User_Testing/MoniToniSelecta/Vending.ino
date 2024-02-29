// _____________Finite State Machine - Possible States: 0 = Sleep / 1 = Idle / 2 = Turn / 3 = Validate / 4 = Collect / 5 = Finished / 6 = Error_____________

void vending(int state) {
  switch (state){
    // 0 = Sleep
    case 0:
      vendingSleep();
      break;
    // 1 = Idle
    case 1:
      vendingIdle();
      break;
    // 2 = Turn
    case 2:
      vendingTurn();
      break;
    // 3 = Validate
    case 3:
      vendingValidate();
      break;
    // 4 = Collect
    case 4:
      vendingCollect();
      break;
    // 5 = Finished
    case 5:
      vendingFinished();
      break;
    // 6 = Error
    case 6:
      vendingError();
      break;
  }
}