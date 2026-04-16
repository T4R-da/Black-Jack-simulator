#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <conio.h>
#include <windows.h>

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

struct Card {
    Rank rank;
    std::string suit;

    void print() const {
        std::string r;
        int v = (int)rank;
        if (v <= 10) r = std::to_string(v);
        else if (v == 11) r = "J";
        else if (v == 12) r = "Q";
        else if (v == 13) r = "K";
        else if (v == 14) r = "A";

        // Hearts and Diamonds are Red
        if (suit == "H" || suit == "D") std::cout << RED;
        else std::cout << RESET;
        
        std::cout << "[" << r << suit << "] " << RESET;
    }
};

// --- LOGIC FUNCTIONS ---

inline int getCardValue(Rank r) {
    int v = (int)r;
    if (v >= 11 && v <= 13) return 10; // J, Q, K = 10
    if (v == 14) return 11;            // Ace = 11
    return v;
}

inline void cardseed() {
    std::srand(static_cast<unsigned>(std::time(0)));
}

inline Card drawCard() {
    std::string suits[] = {"H", "D", "C", "S"};
    Rank r = static_cast<Rank>((std::rand() % 13) + 2);
    return { r, suits[std::rand() % 4] };
}

inline int calculateHandValue(const std::vector<Card>& hand) {
    int total = 0;
    int aces = 0;
    for (const auto& c : hand) {
        total += getCardValue(c.rank);
        if (c.rank == Rank::ACE) aces++;
    }
    // Ace logic: if total > 21, subtract 10 for every Ace held
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
    return total;
}

// --- UI HELPERS ---

inline void clearScreen() { system("cls"); }

inline void printHeader() {
    std::cout << RED << BOLD << "========================================" << RESET << std::endl;
    std::cout << RED << BOLD << "          BLACKJACK CASINO ENGINE       " << RESET << std::endl;
    std::cout << RED << BOLD << "========================================" << RESET << std::endl;
}

inline void waitForEnter() {
    std::cout << "\n" << YELLOW << "Press ENTER to continue..." << RESET;
    while (_getch() != 13);
}
