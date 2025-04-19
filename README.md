# Fruit Fall Frenzy

Fruit Fall Frenzy is a hand-drawn style arcade game where players protect a magical fruit tree from a mischievous spirit by catching falling fruits and avoiding bombs. The game increases in difficulty over time and includes power-ups to enhance gameplay.

## 🎮 Game Overview

- **Theme**: Childhood imagination brought to life
- **Style**: Hand-drawn, black outline with grid background
- **Target Audience**: All ages, especially children
- **Image**: [Game Sreenshot](assets/screenshot.png)

## 🧩 Game Flow

1. **Menu Scene**  
   - Floating clouds animation  
   - Angel and Devil animated characters  
   - Use UP/DOWN keys to select  
   - Acknowledgements screen accessible  

2. **Game Scene**  
   - Player controls a basket to catch fruits  
   - Timer, current score, highest score displayed  
   - Power-ups and bombs appear  
   - Text popups guide the player  
   - Screen flashing for effects  

3. **Game Over Scene**  
   - Shows current and highest scores  
   - Displays fruit collection statistics  
   - Angel and devil animations

## 🛠️ Key Features & Logic

### Power-ups:
- 🧲 **Magnet**: Attracts all fruits for 5 seconds  
- ⚡️ **Slow Down**: Slows falling speed for 5 seconds  
- ⏰ **Time Bonus**: Adds +10 seconds to the timer  
- 💬 Text descriptions are shown at the bottom of the screen

### 💣 Bomb Effects:
- Explosion with red screen flash  
- Freezes player movement for 2 seconds  
- -30 seconds from timer and -40 points  
- "No" voice and sad face animation

### 🍍 Frenzy Mode:
- Triggered by catching a pineapple  
- Yellow screen flash and pineapple dance  
- More and faster fruits/bombs  

### 🧺 Transparent Basket:
- Implemented with 3 basket layers (back, logic, front)  
- Fruits appear to fall into basket visually  
- Caught fruits stored, old ones removed for performance  

### ⏳ Game Progression:
- Controlled by elapsed game time  
- Dynamic difficulty (more fruits, bombs, power-ups)  
- Popup messages for game hints and events

## 📂 Folder Structure

```bash
Project Root/
│
├── assets/                    # Images, fonts, music, sound effects
├── Documents/                 # Game Design Document               
├── config.txt/                # Game config file
├── FruitFallFrenzy/           # Main source code folder
│   ├── Animation.cpp/.h       # Handles animation logic
│   ├── Assets.cpp/.h          # Manages game resources (textures, fonts, etc.)
│   ├── Command.cpp/.h         # Input command system
│   ├── Components.h           # Defines game components (position, velocity, etc.)
│   ├── Entity.cpp/.h          # Entity class implementation
│   ├── EntityManager.cpp/.h   # Controls entity creation and destruction
│   ├── GameEngine.cpp/.h      # Core game engine logic and game loop
│   ├── json.hpp               # JSON parser library
│   └── highscore.txt          # Stores high score records
├── README.md            # Project overview
└── ...
```

## 🔧 Tech Stack

- **Language:** C++ (Tested with Visual Studio 2022)
- **Library:** SFML(Simple and Fast Multimedia Library)
- **Assets:** Hand-drawn sprites, downloaded sound effects and music(see below)

## 🚀 Getting Started

1. Clone the repository
2. Build the project using your preferred C++ IDE (with SFML linked)
3. Run the executable

-

## 🙏 Acknowledgements

- A heartfelt thank you to my instructor, Dr. David Burchill, for helping me overcome technical challenges and providing insightful guidance.
- Thank you to my instructor, Mr. Andrew Blakney, not only for his creative suggestions, but also for consistently guiding and supervising our weekly sprint reviews throughout the semester. His feedback kept the project on track and helped me improve step by step.
- Thanks to my classmate, Mr. Paul Marte, for helping me debug tricky issues and offering thoughtful suggestions during development.
- Sound & Music from [Freesound](https://freesound.org)


## 👩‍💻 Author

Ariel Liu — NBCC Game Development Project

---

Enjoy the game and let your childhood imagination fly!
