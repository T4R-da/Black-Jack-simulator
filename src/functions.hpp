#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <thread>
#include <chrono>
#include <algorithm>
#include <termios.h>
#include <unistd.h>

// ANSI Colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// --- GLOBALS ---
inline int playerBalance = 1000;
inline int currentBet = 0;

enum class Rank {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
};

enum class Suit { HEARTS = 0, DIAMONDS = 1, CLUBS = 2, SPADES = 3 };

struct Card {
    Rank rank;
    Suit suit;

    void print() const {
        std::string r;
        int v = (int)rank;
        if (v <= 10) r = std::to_string(v);
        else if (v == 11) r = "J";
        else if (v == 12) r = "Q";
        else if (v == 13) r = "K";
        else if (v == 14) r = "A";

        std::string s;
        switch(suit) {
            case Suit::HEARTS:   s = "H"; break;
            case Suit::DIAMONDS: s = "D"; break;
            case Suit::CLUBS:    s = "C"; break;
            case Suit::SPADES:   s = "S"; break;
        }

        if (suit == Suit::HEARTS || suit == Suit::DIAMONDS) std::cout << RED;
        else std::cout << CYAN;

        std::cout << "[" << r << s << "] " << RESET;
    }
};

// --- TIMING UTILS ---
inline void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void sleepRandom() {
    sleepMs(1000 + (rand() % 1000));
}

// --- DECK CLASS ---
class Deck {
private:
    std::vector<Card> cards;
public:
    Deck() {
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                cards.push_back({static_cast<Rank>(r), static_cast<Suit>(s)});
            }
        }
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    Card drawCard() {
        if (cards.empty()) return {Rank::ACE, Suit::SPADES};
        Card c = cards.back();
        cards.pop_back();
        return c;
    }

    bool isEmpty() const { return cards.empty(); }
    int remaining() const { return static_cast<int>(cards.size()); }
    void reset() {
        cards.clear();
        for (int s = 0; s < 4; ++s)
            for (int r = 2; r <= 14; ++r)
                cards.push_back({static_cast<Rank>(r), static_cast<Suit>(s)});
    }
};

// --- LOGIC ---
inline int getCardValue(Rank r) {
    int v = (int)r;
    if (v >= 11 && v <= 13) return 10;
    if (v == 14) return 11;
    return v;
}

inline int calculateHandValue(const std::vector<Card>& hand) {
    int total = 0, aces = 0;
    for (const auto& c : hand) {
        total += getCardValue(c.rank);
        if (c.rank == Rank::ACE) aces++;
    }
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
    return total;
}

// --- UI HELPERS ---
inline void clearScreen() {
    std::cout << "\033[2J\033[H";
}

inline void printHeader() {
    std::cout << RED << BOLD << R"(
    =================================================================
        ____  __    ___   ________ __        _____   ________ __
       / __ )/ /   /   | / ____/ //_/       / /   | / ____/ //_/
      / __  / /   / /| |/ /   / ,<     __  / / /| |/ /   / ,<   
     / /_/ / /___/ ___ / /___/ /| |   / /_/ / ___ / /___/ /| |  
    /_____/_____/_/  |_\____/_/ |_|   \____/_/  |_\____/_/ |_|                                                       
    
    =================================================================)" << RESET << std::endl;
}

// --- Cross‑platform getch (Linux) ---
inline int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

inline void waitForEnter() {
    std::cout << "\n" << YELLOW << "Press ENTER to continue..." << RESET;
    while (true) {
        int ch = getch();
        if (ch == '\n' || ch == '\r') break;
    }
}