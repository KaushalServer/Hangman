#include <iostream>
#include <string>
#include <raylib.h>
#include <vector>
#include <sstream>  // Include the sstream header for stringstream
#include <cmath>
#include <raymath.h>
#include <algorithm> 

using namespace std;

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
// Game update that for long a player can go (how many words can you guess within the maxAttempts.)
// Keeping the score of it along with player name

enum GameState {
    STARTUP,
    PLAYING,
    WON,
    SETTINGS
};
// Hangman images ka banna aur girna all shuru
// Related to drawing images
vector<Texture2D> hangmanImages;
bool imagesLoaded = false;
int clickCount = 0;  // Counter for the number of clicks
bool startFalling = false;  // Flag to start the falling animation
float fallSpeed = 4.0f;  // Speed of the falling animation
vector<Vector2> positions;  // Vector to store positions of images
vector<Vector2> fallPositions;  // Vector to store falling positions
vector<float> rotations;  // Vector to store rotation angles of images

const float fallLimit = 450.0f;  // Height where the images should stop falling

// Function to initialize hangman images
void LoadHangmanImages() {
    hangmanImages.push_back(LoadTexture("assets/frame.png"));
    hangmanImages.push_back(LoadTexture("assets/noose.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-head.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-shirt.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-shorts.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-leftarm.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-rightarm.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-leftleg.png"));
    // hangmanImages.push_back(LoadTexture("assets/body01-rightleg.png"));
    hangmanImages.push_back(LoadTexture("assets/body01-head-worried.png"));  // New face texture
    hangmanImages.push_back(LoadTexture("assets/body01-head-dead.png"));  // New face texture
    imagesLoaded = true;

    // Initialize positions for the images
    positions = {
        {50, 50},    // frame
        {180, 80},   // noose
        {155, 100},  // head (adjusted to appear over the t-shirt)
        {130, 200},  // t-shirt
        {160, 300},  // shorts
        {105, 230},  // left arm (adjusted to align with t-shirt)
        {170, 220},  // right arm (adjusted to align with t-shirt)
        {145, 330},  // left leg
        // {170, 300},  // right leg
        {155, 100},   // extra face
        {155, 100}   // extra face
    };

    // Initialize falling positions (same as original positions initially)
    fallPositions = positions;

    // Initialize rotations for the images
    rotations.resize(hangmanImages.size(), 0.0f);
}

void UnloadHangmanImages() {
    for (auto& image : hangmanImages) {
        UnloadTexture(image);
    }
    hangmanImages.clear();
    imagesLoaded = false;
    clickCount = 0; // Reset click count to initial value
    startFalling = false;  // Reset falling animation
}

// Constants
const int maxIncorrectGuesses = 6;  // Number of stages in the hangman

// Global Variables
int incorrectGuesses = 0;

// Function to draw hangman images based on the number of clicks
void DrawHangmanImages(int clicks) {
    for (size_t i = -1; i < clicks && i < hangmanImages.size(); i++) {
        if ( i == 5 && clicks > 5) {  // Draw face worried when left arm appears
            DrawTexture(hangmanImages[8], positions[8].x, positions[8].y, WHITE);
        }
        DrawTexture(hangmanImages[i], positions[i].x, positions[i].y, WHITE);
        if (i == 3 && clicks > 3) {  // Redraw the head after the t-shirt to ensure it appears on top
            DrawTexture(hangmanImages[2], positions[2].x, positions[2].y, WHITE);
        }
        if (i == 8 && clicks > 8) {  // Draw face dead just before drawing the right leg
            DrawTexture(hangmanImages[9], positions[9].x, positions[9].y, WHITE);
        }
    }
}

// Function to handle incorrect guesses
void HandleIncorrectGuess() {
    incorrectGuesses++;
    if (incorrectGuesses > maxIncorrectGuesses) {
        incorrectGuesses = maxIncorrectGuesses;  // Cap at max incorrect guesses
    }
}

// Function to draw falling hangman images with rotation
void DrawFallingImages() {
    for (size_t i = 1; i < hangmanImages.size(); i++) {
        if (i == 1) {
            DrawTexture(hangmanImages[i], fallPositions[i].x, fallPositions[i].y, WHITE); // Draw the noose normally
        } else {
            DrawTexturePro(
                hangmanImages[i],
                {0, 0, (float)hangmanImages[i].width, (float)hangmanImages[i].height},
                {fallPositions[i].x + hangmanImages[i].width / 2, fallPositions[i].y + hangmanImages[i].height / 2, (float)hangmanImages[i].width, (float)hangmanImages[i].height},
                {(float)hangmanImages[i].width / 2, (float)hangmanImages[i].height / 2},
                rotations[i],
                WHITE
            );
        }
    }
}

// Function to update the falling positions
void UpdateFallingPositions() {
    for (size_t i = 2; i < fallPositions.size(); i++) { // Skip the frame (index 0) and noose (index 1)
        if (fallPositions[i].y + hangmanImages[i].height < fallLimit) {
            fallPositions[i].y += fallSpeed;  // Increase the y-position to simulate falling
            rotations[i] += 1.0f;  // Increase rotation angle
        }
    }
}

// Hangman Images ka banna aur girna khatam

// Function to draw the hanging area
void AreaForHanging(int incorrectGuesses) {
    // const int hangmanImageWidth = 200;
    // const int hangmanImageHeight = 200;
    const int hangmanImageX = 50;
    const int hangmanImageY = 50;

    // Load hangman images if not already loaded
    if (hangmanImages.empty()) {
        LoadHangmanImages();
    }

    // Ensure incorrectGuesses is within valid range
    incorrectGuesses = Clamp(incorrectGuesses, 0, maxIncorrectGuesses);
    // cout<<incorrectGuesses;

    // Draw the corresponding hangman image
    if (incorrectGuesses >= 0 && incorrectGuesses < hangmanImages.size()) {
        Texture2D hangmanImage = hangmanImages[incorrectGuesses];
        DrawTexture(hangmanImage, hangmanImageX, hangmanImageY, WHITE);
    }
}

void AreaForLetters(const string& targetWord, const vector<char>& guessedLetters, Texture2D letterSpacer, Font customFont){
    // this will be responsible for displaying the guessing word
 int boxWidth = 320;
    int boxHeight = 80;
    int boxX = 400;
    int boxY = 100;
    int letterSize = 20;
    int maxLetters = targetWord.size();
    int spacing = boxWidth / maxLetters; // Calculate spacing based on the word length

    int xStart = boxX + 5 + (spacing - letterSize) / 2; // Adjust starting position based on spacing
    int yStart = boxY + boxHeight / 2 - letterSize / 2;

    for (size_t i = 0; i < targetWord.size(); i++) {
        if (guessedLetters[i] != '\0') {
            // Draw the guessed letter
            char letter[2] = {guessedLetters[i], '\0'};
            Vector2 position = {xStart + static_cast<float>(i * spacing), static_cast<float>(yStart)};
            DrawTextEx(customFont, letter, position, letterSize, 1, BLACK);
        }
        // Draw the underscore line with an image
        int lineYPosition = boxY + boxHeight - 20;
        DrawTexture(letterSpacer, xStart + i * spacing - 5, lineYPosition, WHITE);
    }
}

void UpdateGuessedLetters(const string& targetWord, vector<char>& guessedLetters) {
    for (int key = 'A'; key <= 'Z'; ++key) {
        if (IsKeyPressed(key)) {
            char pressedChar = static_cast<char>(key);
            bool isCorrectGuess = false;
            for (size_t i = 0; i < targetWord.size(); i++) {
                if (toupper(targetWord[i]) == pressedChar) {
                    guessedLetters[i] = pressedChar;
                    isCorrectGuess = true;
                }
            }

            if (!isCorrectGuess) {
                // DrawText("No",50,50,20,BLACK);
                if (imagesLoaded && clickCount < hangmanImages.size()) {
                    clickCount++;  // Increment click count for each click
            }

            // Start falling animation when the right leg is displayed
                if (clickCount == 9) {
                    startFalling = true;
                }
                    // Update falling positions if the animation is triggered
                if (startFalling) {
                    UpdateFallingPositions();
                }

                if (imagesLoaded) {
                    DrawTexture(hangmanImages[0], positions[0].x, positions[0].y, WHITE);  // Always draw the frame
                    if (startFalling) {
                        DrawFallingImages();  // Draw the falling images
                    } else {
                        DrawHangmanImages(clickCount);  // Draw the images based on click count
                    }
                }
            }
        }
    }
}

string PickRandomWord(const vector<string>& wordList) {
    srand(time(0)); // Seed the random number generator with the current time
    int randomIndex = rand() % wordList.size();
    return wordList[randomIndex];
}

bool keyEnabled[26];

void InitializeKeyEnabled(string pressed = "") {
    static bool initialized = false; // static ke chkker mein  2 din barbad
    // Check if the pressed parameter is not empty and if it is "Pressed", reinitialize
    if (pressed == "Pressed") {
        initialized = false;
    }

    if (!initialized) {
        for (int i = 0; i < 26; ++i) {
            keyEnabled[i] = true; // Initialize all keys to be enabled initially
        }
        initialized = true;
    }
}



void AreaForKeyboard(Font customFont, Texture2D keyBackgroundEnabled, Texture2D keyBackgroundDisabled, int asciiValue = 0) {
    const int startX = 500;  // Adjusted start position to move keyboard to the left
    const int startY = 250;
    const int keyWidth = keyBackgroundEnabled.width;
    const int keyHeight = keyBackgroundEnabled.height;
    const int keySpacing = 10;
    const int fontSize = 25;

    struct KeyPosition {
        char character;
        Vector2 position;
    };
    KeyPosition keyPositions[] = {
        // First row
        {'A', {startX, startY}}, {'B', {startX + keyWidth + keySpacing, startY}}, {'C', {startX + 2 * (keyWidth + keySpacing), startY}},
        {'D', {startX + 3 * (keyWidth + keySpacing), startY}}, {'E', {startX + 4 * (keyWidth + keySpacing), startY}}, {'F', {startX + 5 * (keyWidth + keySpacing), startY}},
        {'G', {startX + 6 * (keyWidth + keySpacing), startY}},

        // Second row
        {'H', {startX, startY + keyHeight + keySpacing}}, {'I', {startX + keyWidth + keySpacing, startY + keyHeight + keySpacing}}, {'J', {startX + 2 * (keyWidth + keySpacing), startY + keyHeight + keySpacing}},
        {'K', {startX + 3 * (keyWidth + keySpacing), startY + keyHeight + keySpacing}}, {'L', {startX + 4 * (keyWidth + keySpacing), startY + keyHeight + keySpacing}}, {'M', {startX + 5 * (keyWidth + keySpacing), startY + keyHeight + keySpacing}},
        {'N', {startX + 6 * (keyWidth + keySpacing), startY + keyHeight + keySpacing}},

        // Third row
        {'O', {startX, startY + 2 * (keyHeight + keySpacing)}}, {'P', {startX + keyWidth + keySpacing, startY + 2 * (keyHeight + keySpacing)}}, {'Q', {startX + 2 * (keyWidth + keySpacing), startY + 2 * (keyHeight + keySpacing)}},
        {'R', {startX + 3 * (keyWidth + keySpacing), startY + 2 * (keyHeight + keySpacing)}}, {'S', {startX + 4 * (keyWidth + keySpacing), startY + 2 * (keyHeight + keySpacing)}}, {'T', {startX + 5 * (keyWidth + keySpacing), startY + 2 * (keyHeight + keySpacing)}},
        {'U', {startX + 6 * (keyWidth + keySpacing), startY + 2 * (keyHeight + keySpacing)}},

        // Fourth row
        {'V', {startX + keyWidth + keySpacing, startY + 3 * (keyHeight + keySpacing)}}, {'W', {startX + 2 * (keyWidth + keySpacing), startY + 3 * (keyHeight + keySpacing)}},
        {'X', {startX + 3 * (keyWidth + keySpacing), startY + 3 * (keyHeight + keySpacing)}}, {'Y', {startX + 4 * (keyWidth + keySpacing), startY + 3 * (keyHeight + keySpacing)}},
        {'Z', {startX + 5 * (keyWidth + keySpacing), startY + 3 * (keyHeight + keySpacing)}}
    };

    InitializeKeyEnabled();

    // Update the state of the pressed key
    if (asciiValue != 0) {
        char character = static_cast<char>(asciiValue);
        int index = character - 'A'; // Convert character to index (A -> 0, B -> 1, ...)
        if (index >= 0 && index < 26) {
            keyEnabled[index] = false; // Disable the pressed key
        }
    }

    // Draw the keys with updated colors using the keyEnabled array
    for (int i = 0; i < 26; ++i) {

        // Draw the appropriate background image based on the key's state
        Texture2D keyBackground = (keyEnabled[i]) ? keyBackgroundEnabled : keyBackgroundDisabled;
        // PrintTexture2DInfo(keyBackground);
        DrawTexture(keyBackground, keyPositions[i].position.x, keyPositions[i].position.y, WHITE);

        // Draw the key character on top of the background image
        DrawTextEx(customFont, &keyPositions[i].character, 
                   {keyPositions[i].position.x + keyWidth / 2 - MeasureTextEx(customFont, &keyPositions[i].character, fontSize, 1).x / 2, 
                    keyPositions[i].position.y + keyHeight / 2 - MeasureTextEx(customFont, &keyPositions[i].character, fontSize, 1).y / 2}, 
                   fontSize, 1, BLACK);
    }

}


// Function to check if the guessed word matches the target word
bool IsWinner(const string& targetWord, const vector<char>& guessedLetters) {

    for (size_t i = 0; i < targetWord.size(); i++) {
        if (toupper(guessedLetters[i]) != toupper(targetWord[i])) {
            return false;
        }
    }
    return true;
}

void DrawWinScreen(Font customFont) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Draw a semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RAYWHITE, 0.5f));

    // Draw the winning message in the center of the screen
    const char* winText = "Congratulations!\n\n\n\n\nYou guesssed the word \n\n\n     DYNAMIC";
    int textWidth = MeasureText(winText, 40);
    int textHeight = 40 * 2; // Assuming each line of text is 40 pixels high
    Vector2 textPosition = {(screenWidth - textWidth) / 2, (screenHeight - textHeight) / 2};

    DrawTextEx(customFont, winText, textPosition, 40, 1, BLACK);
}

void DrawStartupScreen(Texture2D logo, Texture2D settingsButton, Texture2D middleButton) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Draw the logo a bit lower from the top center of the screen
    int logoY = 0;  // Adjust this value as needed to move the logo down
    DrawTexture(logo, (screenWidth - logo.width) / 2, logoY, WHITE);

    // Draw the settings button towards the left and top from the bottom right corner of the screen
    int settingsButtonOffsetX = 20;  // Adjust these values to position the settings button
    int settingsButtonOffsetY = 20;
    // cout<<"Setting buttton Pos X"<<screenWidth - settingsButton.width - settingsButtonOffsetX;
    // cout<<"Setting buttton Pos Y"<<screenHeight - settingsButton.height - settingsButtonOffsetY;
    DrawTexture(settingsButton, screenWidth - settingsButton.width - settingsButtonOffsetX, screenHeight - settingsButton.height - settingsButtonOffsetY, WHITE);

    // Draw the middle button centered on the screen, scaled to half size
    float scale = 0.8f;
    Vector2 middleButtonPosition = {
        (screenWidth - middleButton.width * scale) / 2,
        (screenHeight - middleButton.height * scale) / 2
    };
    DrawTextureEx(middleButton, middleButtonPosition, 0.0f, scale, WHITE);
}

void DrawSettingsScreen(Texture2D soundOnIcon, Texture2D soundOffIcon, Texture2D homeButton, GameState *gameState, Font customFont, bool &isSoundOn) {
    int screenWidth = GetScreenWidth();
    // int screenHeight = GetScreenHeight();

    // Draw settings screen title
    Vector2 titlePosition = {(screenWidth - MeasureTextEx(customFont, "Settings", 40, 1).x) / 2 - 100, 40};
    DrawTextEx(customFont, "Settings", titlePosition, 80, 1, DARKGRAY);

    // Draw "Sound" text and icon
    const char *soundText = "Sound";
    Vector2 soundTextSize = MeasureTextEx(customFont, soundText, 20, 1);
    int soundIconWidth = isSoundOn ? soundOnIcon.width : soundOffIcon.width;
    int spacing = 150;
    int totalWidth = soundTextSize.x + spacing; // Add 20 for spacing  + soundIconWidth
    Vector2 soundTextPosition = {(screenWidth - totalWidth) / 2 - 100, 200};
    DrawTextEx(customFont, soundText, soundTextPosition, 40, 1, DARKGRAY);

    // Draw sound icon
    Vector2 soundIconPosition = {soundTextPosition.x + soundTextSize.x + spacing, 170 + (soundTextSize.y - soundOnIcon.height) / 2 + 50};
    DrawTexture(isSoundOn ? soundOnIcon : soundOffIcon, soundIconPosition.x, soundIconPosition.y, WHITE);

    // Check if mouse is over the sound icon and toggle sound state on click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        if (CheckCollisionPointRec({(float)mouseX, (float)mouseY}, {(float)soundIconPosition.x, (float)soundIconPosition.y, (float)soundIconWidth, (float)soundOnIcon.height})) {
            isSoundOn = !isSoundOn;
        }
    }

    // Draw "Background Color" text
    const char *backgroundColorText = "BG Color";
    Vector2 backgroundColorTextSize = MeasureTextEx(customFont, backgroundColorText, 20, 1);
    Vector2 backgroundColorTextPosition = {(screenWidth - (backgroundColorTextSize.x + spacing)) / 2 - 100, 340};
    DrawTextEx(customFont, backgroundColorText, backgroundColorTextPosition, 40, 1, DARKGRAY);

    // Calculate the position for the rectangles to the right of the text
    int rectX = backgroundColorTextPosition.x + backgroundColorTextSize.x + spacing;
    int rectY = backgroundColorTextPosition.y;

    // Draw blue rectangle
    DrawRectangle(rectX, rectY - 10, 50, 50, BLUE);

    // Draw magenta rectangle
    DrawRectangle(rectX + 150, rectY - 10, 50, 50, MAGENTA);

    // Draw Home button
    int homeButtonX = screenWidth - homeButton.width - 40;
    int homeButtonY = 40;
    DrawTexture(homeButton, homeButtonX, homeButtonY, WHITE);

    // Check if mouse is over the Home button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();
        if (CheckCollisionPointRec({(float)mouseX, (float)mouseY}, {(float)homeButtonX, (float)homeButtonY, (float)homeButton.width, (float)homeButton.height})) {
            *gameState = STARTUP;
        }
    }
}

bool IsMouseOverTexture(Texture2D texture, int posX, int posY) {
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    return (mouseX > posX) && (mouseX < posX + texture.width) && (mouseY > posY) && (mouseY < posY + texture.height);
}


int main() {
    const int screenWidth = 960;
    const int screenHeight = 540;

    vector<string> wordList = {"Written", "Dynamic", "Target", "Random", "Example"};

    // Pick a random word
    string targetWord = PickRandomWord(wordList);
    vector<char> guessedLetters(targetWord.size(), '\0');


    bool isSoundOn = true; // Variable to keep track of sound state


    GameState gameState = STARTUP;

    InitWindow(screenWidth, screenHeight, "HangMan!");
    SetTargetFPS(60);
    Texture2D bgImage = LoadTexture("assets/background-blue.png");
    Texture2D gameLogoImage = LoadTexture("assets/gamelogo.png");
    Texture2D settingsImage = LoadTexture("assets/settings-btn.png");
    Texture2D playBtnImage = LoadTexture("assets/play-btn.png");
    Texture2D homeButton = LoadTexture("assets/home-btn.png");
    Texture2D soundOnButton = LoadTexture("assets/toggle-on.png");
    Texture2D soundOffButton = LoadTexture("assets/toggle-off.png");
    Texture2D keyEnabledBackground = LoadTexture("assets/letter-bg.png");
    Texture2D keyDisabledBackground = LoadTexture("assets/letter-bg2.png");
    Texture2D letterSpacer = LoadTexture("assets/letter-spacer1.png");
    Texture2D bgImage2 = LoadTexture("assets/background-pink.png");
    LoadHangmanImages();

    // Load custom font
    Font customFont = LoadFont("fontStyle/TitanOne-Regular.ttf");

    while (!WindowShouldClose()) {
        int key = GetKeyPressed();

        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(bgImage, 0, 0, WHITE);

        switch (gameState) {
            case STARTUP:
                DrawStartupScreen(gameLogoImage, settingsImage, playBtnImage);
                
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    
                    // Check if the settings button is clicked
                    int settingsButtonX = screenWidth - settingsImage.width - 40;
                    int settingsButtonY = screenHeight - settingsImage.height - 40;
                    if (IsMouseOverTexture(settingsImage, settingsButtonX, settingsButtonY)) {
                        gameState = SETTINGS;
                    }

                    // Check if the play button is clicked
                    int playButtonX = (screenWidth - playBtnImage.width * 0.5) / 2;
                    int playButtonY = (screenHeight - playBtnImage.height * 0.5) / 2;
                    if (IsMouseOverTexture(playBtnImage, playButtonX, playButtonY)) {
                        gameState = PLAYING;
                    }
                }
                break;
                
            case PLAYING:

                // improvement don.t check everytime because waste of time and space. ( only when keypress)
                UpdateGuessedLetters(targetWord, guessedLetters);

                if (IsWinner(targetWord, guessedLetters)) { // only when jab saari spaces bhar gyi hain
                    gameState = WON;
                }

                AreaForHanging(incorrectGuesses);
                AreaForLetters(targetWord, guessedLetters, letterSpacer, customFont);
                AreaForKeyboard(customFont, keyEnabledBackground, keyDisabledBackground, key);
                break;

            case WON:
                DrawWinScreen(customFont);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)) {
                    // Reset game state
                    // meaning ki either clicked or pressed.
                    InitializeKeyEnabled("Pressed");
                    gameState = PLAYING;
                    // Reset any necessary variables like guessedLetters, incorrectGuesses, etc.
                    guessedLetters.clear();
                    guessedLetters.resize(targetWord.size(), '\0');
                    incorrectGuesses = 0;
                    // Pick a new random word
                    targetWord = PickRandomWord(wordList);

                }
                break;
                
            case SETTINGS:
                DrawSettingsScreen(soundOnButton, soundOffButton, homeButton, &gameState, customFont, isSoundOn);
                break;
        }

        EndDrawing();
    }

    UnloadTexture(bgImage);
    UnloadTexture(gameLogoImage);
    UnloadTexture(settingsImage);
    UnloadTexture(playBtnImage);
    UnloadTexture(homeButton);
    UnloadTexture(soundOnButton);
    UnloadTexture(soundOffButton);
    UnloadTexture(letterSpacer);
    UnloadTexture(bgImage2);
    UnloadHangmanImages();

    UnloadFont(customFont);

    CloseWindow();
    return 0;
}