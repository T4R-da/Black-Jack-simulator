# 🃏 Terminal Blackjack Engine

A high-performance, interactive Blackjack (21) simulation built for the Windows Terminal. This engine features a smart Dealer AI, dynamic hand-summing logic (Ace adjustment), and an interactive menu system.

![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![Language](https://img.shields.io/badge/Language-C++-blue.svg)

---

## ✨ Features

*   **Arrow-Key Navigation:** Smooth main menu system using standard keyboard arrows and Enter.
*   **Intelligent Hand Logic:** 
    *   **MaxRank Calculation:** Automatically calculates hand totals in real-time.
    *   **Dynamic Ace Adjustment:** Aces automatically switch from 11 to 1 if the hand total exceeds 21.
*   **Dealer AI:** The CPU follows professional casino rules—hitting on any total below 17 and standing once 17 or higher is reached.
*   **Real-Time UI:** A specialized "Refresh UI" system that updates the screen as cards are dealt, keeping the dealer's hole card hidden until the showdown.
*   **Economy System:** Place custom bets, track your balance, and receive "charity" funds if your balance hits zero.
*   **Audio Integration:** Uses the `miniaudio` library to play background audio or effects via `sound.wav`.

---

## 🎮 How to Play

1.  **Place Your Bet:** Enter an amount within your current balance.
2.  **Player Phase:** 
    *   Press **[H]** to Hit (Ask the dealer for another card).
    *   Press **[S]** to Stand (Keep your current total and end your turn).
3.  **Dealer Phase:** The dealer reveals their hidden card and draws until they reach at least 17.
4.  **Win/Loss:** Beat the dealer's total without going over 21 to win the pot!

---

## 🛠 Setup & Compilation

### Requirements
*   **OS:** Windows (Uses `<windows.h>` and `<conio.h>`)
*   **Compiler:** G++ (MinGW) or MSVC (Visual Studio)
*   **Audio File:** A `sound.wav` file must be in the root directory.
*   **Dependencies:** `miniaudio.h` (single-header) must be in the project folder.

### Compilation
To compile via the command line using MinGW:

```bash
g++ main.cpp -o Blackjack.exe -lwinmm -lole32