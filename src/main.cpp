#include "functions.hpp"
#include <thread> // Required for sleep
#include <chrono> // Required for seconds

// Optional: miniaudio implementation
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void refreshUI(const std::vector<Card>& pHand, const std::vector<Card>& dHand, std::string phase, bool revealDealer) {
    clearScreen();
    printHeader();
    std::cout << YELLOW << "Current Bet: $" << currentBet << " | Balance: $" << playerBalance - currentBet << RESET << "\n";
    std::cout << GREEN << BOLD << "PHASE: " << phase << RESET << "\n\n";

    // Dealer Display
    std::cout << BOLD << "DEALER HAND: " << RESET;
    if (!revealDealer) {
        if (!dHand.empty()) dHand[0].print();
        std::cout << "[??] (Hidden)";
    } else {
        for (const auto& c : dHand) c.print();
        std::cout << " (Total: " << calculateHandValue(dHand) << ")";
    }

    // Player Display
    std::cout << "\n\n" << BOLD << "YOUR HAND:   " << RESET;
    for (const auto& c : pHand) c.print();
    int pVal = calculateHandValue(pHand);
    std::cout << " (Total: " << pVal << ")";

    if (pVal > 21) std::cout << RED << BOLD << " -> BUSTED!" << RESET;
    std::cout << "\n----------------------------------------\n";
}

void playGame(ma_engine* pMainEngine) { // Added engine parameter
    // 1. Betting
    if (playerBalance <= 0) playerBalance = 200; // Charity
    while (true) {
        clearScreen();
        printHeader();
        std::cout << GREEN << "BALANCE: $" << playerBalance << RESET << "\n";
        std::cout << "Enter your bet: $";
        std::cin >> currentBet;
        if (currentBet > 0 && currentBet <= playerBalance) break;
        std::cout << RED << "Invalid bet!" << RESET; Sleep(1000);
    }

    // 2. Initial Deal
    std::vector<Card> pHand = { drawCard(), drawCard() };
    std::vector<Card> dHand = { drawCard(), drawCard() };

    // 3. Player Turn
    bool playerBusted = false;
    while (true) {
        refreshUI(pHand, dHand, "YOUR TURN (H: Hit, S: Stand)", false);
        if (calculateHandValue(pHand) > 21) { playerBusted = true; break; }
        if (calculateHandValue(pHand) == 21) break;

        char choice = _getch();
        if (choice == 'h' || choice == 'H') pHand.push_back(drawCard());
        else if (choice == 's' || choice == 'S') break;
    }

    // 4. Dealer Turn
    if (!playerBusted) {
        while (calculateHandValue(dHand) < 17) {
            refreshUI(pHand, dHand, "DEALER DRAWS...", true);
            Sleep(800);
            dHand.push_back(drawCard());
        }
    }

    // 5. Resolution
    refreshUI(pHand, dHand, "FINAL RESULTS", true);
    int pFinal = calculateHandValue(pHand);
    int dFinal = calculateHandValue(dHand);

    std::cout << "\n";
    if (pFinal > 21) {
        std::cout << RED << "BUST! You lose $" << currentBet << RESET << "\n";
        playerBalance -= currentBet;

        // --- LOSE SOUND (FART) ---
        ma_engine_stop(pMainEngine);
        ma_engine engine2;
        if (ma_engine_init(NULL, &engine2) == MA_SUCCESS) {
            ma_engine_play_sound(&engine2, "fart.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ma_engine_uninit(&engine2);
        }
        ma_engine_start(pMainEngine);

    } else if (dFinal > 21 || pFinal > dFinal) {
        std::cout << GREEN << BOLD << "YOU WIN! Gained $" << currentBet << RESET << "\n";
        playerBalance += currentBet;

        // --- WIN SOUND (JACKPOT) ---
        ma_engine_stop(pMainEngine);
        ma_engine engine3;
        if (ma_engine_init(NULL, &engine3) == MA_SUCCESS) {
            ma_engine_play_sound(&engine3, "jackpot.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ma_engine_uninit(&engine3);
        }
        ma_engine_start(pMainEngine);

    } else if (dFinal > pFinal) {
        std::cout << RED << "HOUSE WINS! Lost $" << currentBet << RESET << "\n";
        playerBalance -= currentBet;

        // --- LOSE SOUND (FART) ---
        ma_engine_stop(pMainEngine);
        ma_engine engine2;
        if (ma_engine_init(NULL, &engine2) == MA_SUCCESS) {
            ma_engine_play_sound(&engine2, "fart.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ma_engine_uninit(&engine2);
        }
        ma_engine_start(pMainEngine);

    } else {
        std::cout << YELLOW << "PUSH! Bet returned." << RESET << "\n";
    }
    waitForEnter();
}

int main() {
    cardseed();
    
    // Audio Initialization
    ma_engine engine;
    bool audioEnabled = false;
    if (ma_engine_init(NULL, &engine) == MA_SUCCESS) {
        ma_engine_play_sound(&engine, "sound.wav", NULL); 
        audioEnabled = true;
    }

    int choice = 0;
    std::string options[] = {"Start Game", "Rules", "Exit"};

    while (true) {
        clearScreen();
        printHeader();
        std::cout << YELLOW << "ACCOUNT BALANCE: $" << playerBalance << RESET << "\n\n";

        for (int i = 0; i < 3; i++) {
            if (i == choice) std::cout << GREEN << BOLD << "  > " << options[i] << " <" << RESET << std::endl;
            else std::cout << "    " << options[i] << std::endl;
        }

        int key = _getch();
        if (key == 224) { // Arrow Keys
            key = _getch();
            if (key == 72) choice = (choice - 1 + 3) % 3; // Up
            if (key == 80) choice = (choice + 1) % 3;     // Down
        } else if (key == 13) { // Enter
            if (choice == 0) playGame(&engine); // Pass the engine here
            else if (choice == 1) {
                clearScreen();
                std::cout << "RULES:\n1. Get closer to 21 than dealer.\n2. Dealer hits until 17.\n3. Ace is 1 or 11.\n";
                waitForEnter();
            }
            else break;
        }
    }

    if (audioEnabled) ma_engine_uninit(&engine);
    return 0;
}
