#include "../inc/GameManager.h"

GameManager::GameManager()
{
    //ctor
}

void GameManager::DoGame()
{
  std::cout << "Welcome to OOPoker\n\
Copyright (c) 2010-2014 Lode Vandevenne" << std::endl << std::endl;

  char c = 0;


  std::cout << "Choose Game Type\n\
1: human + AI's\n\
2: human + AI heads-up\n\
3: AI battle\n\
4: AI battle heads-up\n\
r: random game (human)\n\
c: calculator\n\
u: unit test" << std::endl;
  c = getChar();
  int gameType = 1;
  if(c == '1') gameType = 1;
  else if(c == '2') gameType = 2;
  else if(c == '3') gameType = 3;
  else if(c == '4') gameType = 4;
  else if(c == 'r') gameType = 5;
  else if(c == 'c')
  {

    std::cout << "Choose Calculator\n\
1: Pot Equity\n\
2: Showdown" << std::endl;

    char c2 = getChar();
    if(c2 == '1') runConsolePotEquityCalculator();
    else runConsoleShowdownCalculator();
    return;
  }
  else if(c == 'u')
  {
    doUnitTest();
    return;
  }
  else if(c == 'q') return;



  Rules rules;
  rules.buyIn = 1000;
  rules.bigBlind = 10;
  rules.ante = 0;
  rules.allowRebuy = false;
  rules.fixedNumberOfDeals = 100;

  std::cout << std::endl << std::endl;

  std::cout << "Choose Game Win Condition\n\
1: last remaining (no rebuys)\n\
2: rebuys, fixed 100 deals\n\
3: rebuys, fixed 1000 deals\n\
4: rebuys, fixed 10000 deals\n\
c: rebuys, fixed custom amount of deals" << std::endl;
  c = getChar();
  if(c == '1') { rules.allowRebuy = false; rules.fixedNumberOfDeals = 0; }
  else if(c == '2') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 100; }
  else if(c == '3') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 1000; }
  else if(c == '4') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 10000; }
  else if(c == 'c')
  {
    std::string s;

    std::cout << "enter number of deals: ";
    s = getLine();

    rules.allowRebuy = true;
    rules.fixedNumberOfDeals = strtoval<int>(s);
  }
  else if(c == 'q') return;

  std::cout << std::endl << std::endl;

  HostTerminal host;
  Game game(&host);

  if(gameType != 5)
  {
    std::cout << "choose betting structure (buy-in, small, big)\n\
1: 1000, 5, 10\n\
2: 1000, 10, 20\n\
3: 1000, 50, 100\n\
4: 1000, 100, 200\n\
5: 100000, 5, 10\n\
6: 100000, 10, 20\n\
7: 100000, 50, 100\n\
8: 100000, 100, 200\n\
9: 1000, 5, 10, ante 1\n\
c: custom" << std::endl;
    c = getChar();
    if(c == '1')      {rules.buyIn = 1000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 0; }
    else if(c == '2') {rules.buyIn = 1000; rules.smallBlind = 10; rules.bigBlind = 20; rules.ante = 0; }
    else if(c == '3') {rules.buyIn = 1000; rules.smallBlind = 50; rules.bigBlind = 100; rules.ante = 0; }
    else if(c == '4') {rules.buyIn = 1000; rules.smallBlind = 100; rules.bigBlind = 200; rules.ante = 0; }
    else if(c == '5') {rules.buyIn = 100000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 0; }
    else if(c == '6') {rules.buyIn = 100000; rules.smallBlind = 10; rules.bigBlind = 20; rules.ante = 0; }
    else if(c == '7') {rules.buyIn = 100000; rules.smallBlind = 50; rules.bigBlind = 100; rules.ante = 0; }
    else if(c == '8') {rules.buyIn = 100000; rules.smallBlind = 100; rules.bigBlind = 200; rules.ante = 0; }
    else if(c == '9') {rules.buyIn = 1000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 1; }
    else if(c == 'c')
    {
      std::string s;

      std::cout << "enter buy-in amount: ";
      s = getLine();
      rules.buyIn = strtoval<int>(s);

      std::cout << "enter small blind: ";
      s = getLine();
      rules.smallBlind = strtoval<int>(s);

      std::cout << "enter big blind: ";
      s = getLine();
      rules.bigBlind = strtoval<int>(s);

      std::cout << "enter ante: ";
      s = getLine();
      rules.ante = strtoval<int>(s);

    }
    else if(c == 'q') return;
  }
  else
  {
    if(getRandom() < 0.2)
    {
      rules.ante = getRandom(1, 5);
    }
    rules.bigBlind = getRandom(10, 200);
    //make nice round number
    rules.bigBlind = getNearestRoundNumber(rules.bigBlind);
    rules.smallBlind = rules.bigBlind / 2;

    rules.buyIn = getRandom(500, 200000);
    rules.buyIn = getNearestRoundNumber(rules.buyIn);
  }

  game.setRules(rules);

  std::vector<Event> events;

  std::vector<Player> players;
  std::vector<Observer*> observers;

  game.addObserver(new ObserverLog("log.txt"));

  if(gameType == 1) //Human + AI's
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    //choose the AI players here
    game.addPlayer(Player(new AIRandom(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 2) //Human + AI heads-up
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    //choose the AI player here
    game.addPlayer(Player(new AIBlossom(), "Blossom"));//game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 3) //AI Battle
  {
    //game.addObserver(new ObserverTerminalQuiet());
    game.addObserver(new ObserverTerminal());

    //choose the AI players here (AISmart, AIRandom, AICall, ...)
    game.addPlayer(Player(new AIBlossom(), "Blossom"));//game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));

    //for benchmarking game logic with only AICall bots
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));

  }
  else if(gameType == 4) //AI heads-up
  {
    game.addObserver(new ObserverTerminalQuiet());

    //choose two AI players here
    game.addPlayer(Player(new AIBlossom(), "Blossom"));//game.addPlayer(Player(new AIRandom(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 5) //random game (human)
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    size_t num = getRandom(1, 9);
    for(size_t i = 0; i < num; i++)
    {
      game.addPlayer(Player(getRandom() < 0.1 ? (AI*)(new AIRandom()) : (AI*)(new AISmart()), getRandomName()));
    }
  }

  game.doGame();
}
