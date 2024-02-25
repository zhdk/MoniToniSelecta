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