#include <algorithm>
#include <bitset>
#include <cstdint>
#include <ios>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define BG_RESET "\033[49m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

// Card Stacks:
// Our Hand
// Opp Hand
// Our side + Opp side:
//   Air
//   Land
//   Sea

// Card is defined as following
// bit 5: faceup?
// bit 3 - 4: theater
// bit 0 - 2: value
// --000000 is an unknown (face down) card
// --100001 is the Air 1
// --111010 is the Lnd 6
// --110111 is the Sea 3
using Card = std::int8_t;

bool isFaceUp(const Card &card) { return card >> 5; }

std::int8_t getTheater(const Card &card) { return (card >> 3) & 0b000011; }

std::int8_t getValue(const Card &card) { return card & 0b000111; }

std::string printCard(const Card &card) {
  std::string face[2] = {"↓", "↑"};
  std::string theaters[3] = {"Air", "Lnd", "Sea"};
  std::string theaterColors[3] = {WHITE, GREEN, BLUE};

  std::string out = "";

  if (isFaceUp(card) != 0) {
    out += theaterColors[getTheater(card)];
  }
  out += face[isFaceUp(card)] + "-" + theaters[getTheater(card)] + "-" +
         std::to_string(getValue(card)) + RESET;
  return out;
}

Card newCard(int faceUp, int theater, int value) {
  theater &= 0b00000011;
  value &= 0b00000111;

  Card card = 0;
  card |= faceUp << 5;
  card |= theater << 3;
  card |= value;
  return card;
}

using Theater = std::vector<Card>;
using MiddleTheater = std::bitset<2>;

// Hand is defined as set/unset for each possible card
using Hand = std::bitset<18>;

Hand randomHand() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distribution(0, 17);

  Hand hand = std::bitset<18>();

  while (hand.count() < 6) {
    int randomIndex = distribution(gen);
    hand.set(randomIndex);
  }

  return hand;
}

struct GameState {
  Hand hand;
  Theater ownTheaters[3];
  Theater oppTheaters[3];
  MiddleTheater mt;

  // defines if containment is active or not
  bool containment;

  // 3 bits to define barricade location, 3 bits to define support location
  // ignore the 2 highest place bits
  int8_t barricadeSupport;
};

GameState defaultGameState(std::string mt) {
  GameState state;
  state.hand = randomHand();

  state.mt = std::bitset<2>(mt);

  return state;
};

std::string colorText(std::string str, std::string fg = "",
                      std::string bf = "") {
  return fg + bf + str + RESET + BG_RESET;
}

void printGameState(GameState state) {
  // Generating Strings for Opp Cards

  int maxCards = 0;
  for (int i = 0; i < 3; ++i) {
    int newLen = state.oppTheaters[i].size();
    maxCards = maxCards > newLen ? maxCards : newLen;
  }

  std::string oppCardsPrint[maxCards];
  for (int i = 0; i < maxCards; ++i) {
    std::string row = " ";

    for (int j = 0; j < 3; j++) {
      if (i >= state.oppTheaters[j].size())
        row += "       ";
      else
        row += printCard(state.oppTheaters[j][i]);

      if (j < 2)
        row += "   ";
    }
  }

  // Printing the Cards
  std::string airBoard = colorText("   AIR   ", BLACK, BG_WHITE);
  std::string lndBoard = colorText("   LND   ", BLACK, BG_GREEN);
  std::string seaBoard = colorText("   SEA   ", BLACK, BG_BLUE);

  std::string titles = "";

  if (state.mt != 1) {
    titles += airBoard + "  ";

    if (state.mt != 2) {
      titles += seaBoard + "  ";
      titles += lndBoard + "  ";
    } else {
      titles += lndBoard + "  ";
      titles += seaBoard + "  ";
    }
  } else {
    titles += lndBoard + "  ";
    titles += airBoard + "  ";
    titles += seaBoard + "  ";
  }

  std::cout << titles << std::endl;
}

int main() {
  Card card = 0b00101001;
  printCard(card);

  GameState state = defaultGameState("10");
  printGameState(state);
  std::cout << state.mt << std::endl;
}
