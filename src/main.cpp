#include "functions.hpp"
#include <thread>
#include <chrono>

// Optional: miniaudio implementation
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void refreshUI(const std::vector<Card>& pHand, const std::vector<Card>& dHand, std::string phase, bool revealDealer, int deckSize = 52) {
    clearScreen();
    printHeader();
    std::cout << YELLOW << "Deck: " << deckSize << " cards | Bet: $" << currentBet 
              << " | Balance: $" << playerBalance << RESET << "\n";
    std::cout << GREEN << BOLD << "PHASE: " << phase << RESET << "\n\n";

    // Dealer Display
    std::cout << BOLD << "DEALER HAND: " << RESET;
    if (!revealDealer) {
        if (!dHand.empty()) {
            dHand[0].print();
            std::cout << "[??] (Hidden)";
        }
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
    if (pVal == 21 && pHand.size() == 2) std::cout << GREEN << BOLD << " -> BLACKJACK!" << RESET;
    std::cout << "\n----------------------------------------\n";
}

void playGame(ma_engine* pMainEngine) {
    // Initialize deck for this hand
    Deck deck;
    deck.shuffle();
    
    // 1. Betting
    if (playerBalance <= 0) {
        std::cout << YELLOW << "You're broke! Here's $200 charity." << RESET << std::endl;
        playerBalance = 200;
        sleepMs(1500);
    }
    
    while (true) {
        clearScreen();
        printHeader();
        std::cout << GREEN << "BALANCE: $" << playerBalance << RESET << "\n";
        std::cout << "Enter your bet: $";
        if (!(std::cin >> currentBet)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << RED << "Invalid input!" << RESET << std::endl;
            sleepMs(1000);
            continue;
        }
        if (currentBet > 0 && currentBet <= playerBalance) break;
        std::cout << RED << "Invalid bet!" << RESET; 
        sleepMs(1000);
    }
    std::cin.ignore(1000, '\n');

    // 2. Initial Deal (alternating like real casino: Player, Dealer, Player, Dealer)
    std::vector<Card> pHand;
    std::vector<Card> dHand;
    
    pHand.push_back(deck.drawCard());
    dHand.push_back(deck.drawCard());
    pHand.push_back(deck.drawCard());
    dHand.push_back(deck.drawCard());

    // Check for Blackjacks (21 on first 2 cards)
    int pVal = calculateHandValue(pHand);
    int dVal = calculateHandValue(dHand);
    
    refreshUI(pHand, dHand, "INITIAL DEAL", false, deck.remaining());

    // 3. Check immediate Blackjacks
    if (pVal == 21 && dVal == 21) {
        std::cout << YELLOW << "Both have Blackjack! PUSH." << RESET << std::endl;
        sleepMs(2000);
        
        // Draw sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "draw.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2)); 
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);
        waitForEnter();
        return;
    }
    else if (dVal == 21) {
        std::cout << RED << "Dealer has Blackjack! You lose $" << currentBet << RESET << std::endl;
        playerBalance -= currentBet;
        sleepMs(2000);
        
        // Lose sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "fart.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);
        waitForEnter();
        return;
    }
    else if (pVal == 21) {
        int blackjackPay = static_cast<int>(currentBet * 1.5); // Blackjack pays 3:2
        std::cout << GREEN << "BLACKJACK! You win $" << blackjackPay << RESET << std::endl;
        playerBalance += blackjackPay;
        sleepMs(2000);
        
        // Win sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "jackpot.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);
        waitForEnter();
        return;
    }

    // 4. Player Turn
    bool playerBusted = false;
    while (true) {
        refreshUI(pHand, dHand, "YOUR TURN (H: Hit, S: Stand, D: Double Down)", false, deck.remaining());
        
        pVal = calculateHandValue(pHand);
        if (pVal > 21) { 
            playerBusted = true; 
            break; 
        }

        char choice = _getch();
        if (choice == 'h' || choice == 'H') {
            if (!deck.isEmpty()) {
                pHand.push_back(deck.drawCard());
                // Brief delay to show card being dealt
                sleepMs(500);
            }
        }
        else if (choice == 's' || choice == 'S') {
            break;
        }
        else if ((choice == 'd' || choice == 'D') && pHand.size() == 2 && (playerBalance >= currentBet * 2)) {
            // Double down: double bet, take exactly one more card
            currentBet *= 2;
            if (!deck.isEmpty()) {
                pHand.push_back(deck.drawCard());
                sleepMs(500);
            }
            refreshUI(pHand, dHand, "DOUBLE DOWN - Standing...", false, deck.remaining());
            sleepMs(1000);
            break;
        }
    }

    // 5. Dealer Turn
    if (!playerBusted) {
        dVal = calculateHandValue(dHand);
        while (dVal < 17) {
            refreshUI(pHand, dHand, "DEALER DRAWING...", true, deck.remaining());
            sleepRandom(); // 1-2 second "thinking" delay
            
            if (!deck.isEmpty()) {
                dHand.push_back(deck.drawCard());
                dVal = calculateHandValue(dHand);
                sleepMs(800); // Show the card
            } else {
                break;
            }
        }
    }

    // 6. Resolution
    refreshUI(pHand, dHand, "FINAL RESULTS", true, deck.remaining());
    int pFinal = calculateHandValue(pHand);
    int dFinal = calculateHandValue(dHand);

    std::cout << "\n";
    
    if (playerBusted || pFinal > 21) {
        std::cout << RED << "BUST! You lose $" << currentBet << RESET << "\n";
        playerBalance -= currentBet;

        // Lose sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "fart.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);

    } else if (dFinal > 21) {
        std::cout << GREEN << BOLD << "DEALER BUSTS! You win $" << currentBet << RESET << "\n";
        playerBalance += currentBet;

        // Win sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "jackpot.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);

    } else if (pFinal > dFinal) {
        std::cout << GREEN << BOLD << "YOU WIN! Gained $" << currentBet << RESET << "\n";
        playerBalance += currentBet;

        // Win sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "jackpot.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);

    } else if (dFinal > pFinal) {
        std::cout << RED << "HOUSE WINS! Lost $" << currentBet << RESET << "\n";
        playerBalance -= currentBet;

        // Lose sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "fart.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);

    } else {
        std::cout << YELLOW << "PUSH! Bet returned." << RESET << "\n";

        // Draw sound
        ma_engine_stop(pMainEngine);
        ma_engine engineEffect;
        if (ma_engine_init(NULL, &engineEffect) == MA_SUCCESS) {
            ma_engine_play_sound(&engineEffect, "draw.wav", NULL);
            std::this_thread::sleep_for(std::chrono::seconds(2)); 
            ma_engine_uninit(&engineEffect);
        }
        ma_engine_start(pMainEngine);
    }
    
    waitForEnter();
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Audio Initialization with looping background music
    ma_engine engine;
    ma_sound bgSound;
    bool audioEnabled = false;
    
    if (ma_engine_init(NULL, &engine) == MA_SUCCESS) {
        // Setup looping background music
        if (ma_sound_init_from_file(&engine, "sound.wav", MA_SOUND_FLAG_STREAM, NULL, NULL, &bgSound) == MA_SUCCESS) {
            ma_sound_set_looping(&bgSound, true);
            ma_sound_start(&bgSound);
            audioEnabled = true;
        }
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
            if (choice == 0) playGame(&engine); 
            else if (choice == 1) {
                clearScreen();
                printHeader();
                std::cout << "RULES:\n";
                std::cout << "1. Get closer to 21 than dealer without busting.\n";
                std::cout << "2. Dealer hits until 17 or higher.\n";
                std::cout << "3. Ace counts as 11 or 1 (flexible).\n";
                std::cout << "4. Blackjack (Ace + 10-value card) pays 3:2.\n";
                std::cout << "5. Press H to Hit, S to Stand, D to Double Down.\n";
                waitForEnter();
            }
            else break;
        }
    }

    // Cleanup
    if (audioEnabled) {
        ma_sound_stop(&bgSound);
        ma_sound_uninit(&bgSound);
        ma_engine_uninit(&engine);
    }
    return 0;
}
