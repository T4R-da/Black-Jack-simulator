# Terminal Blackjack Engine (Linux Port)

A high-performance, interactive Blackjack (21) simulation built for the Linux terminal. This engine features a smart Dealer AI, dynamic hand-summing logic (Ace adjustment), and an interactive menu system.

Platform: Linux (POSIX)  
Language: C++

---

## Features

- Arrow-Key Navigation: Smooth main menu system using standard keyboard arrows and Enter.
- Intelligent Hand Logic:
  - MaxRank Calculation: Automatically calculates hand totals in real-time.
  - Dynamic Ace Adjustment: Aces automatically switch from 11 to 1 if the hand total exceeds 21.
- Dealer AI: The CPU follows professional casino rules—hitting on any total below 17 and standing once 17 or higher is reached.
- Real-Time UI: A specialized "Refresh UI" system that updates the screen as cards are dealt, keeping the dealer's hole card hidden until the showdown.
- Economy System: Place custom bets, track your balance, and receive "charity" funds if your balance hits zero.
- Audio Integration: Uses the miniaudio library to play background audio or effects via sound.wav.
- Cross-platform terminal handling: Uses termios and ANSI escape codes for raw input and screen clearing, making it compatible with any modern Linux terminal.

---

## How to Play

1. Place Your Bet: Enter an amount within your current balance.
2. Player Phase:
   - Press [H] to Hit (Ask the dealer for another card).
   - Press [S] to Stand (Keep your current total and end your turn).
3. Dealer Phase: The dealer reveals their hidden card and draws until they reach at least 17.
4. Win/Loss: Beat the dealer's total without going over 21 to win the pot.

---

## Setup & Compilation

### Requirements
- OS: Linux (or any POSIX system with termios support)
- Compiler: G++ (GCC) or Clang with C++17 support
- Audio File: A sound.wav file must be in the root directory (optional; audio is skipped if missing).
- Dependencies: miniaudio.h (single-header) must be in the project folder.

### Compilation
To compile via the command line using GCC:

```bash
g++ -std=c++17 main.cpp -o blackjack -lpthread -ldl
```

The executable `blackjack` can then be run with:

```bash
./blackjack
```

---

## Notes

- The game uses ANSI escape sequences for colored output and screen clearing. Ensure your terminal supports them (most do).
- Arrow keys and single-character input are handled via termios, so no external libraries are required beyond the standard POSIX ones.
- Audio is optional; if `sound.wav` is not found, the game runs silently without errors.