#include <Arduboy2.h>

//==========================================================================
// Build configuration
//--------------------------------------------------------------------------
// Only one of these is allowed to be defined at a time
//#define CONFIGURATION_TEST
//#define CONFIGURATION_DEBUG
#define CONFIGURATION_RELEASE

// TEST - runs unit tests instead of the game
#if defined CONFIGURATION_TEST
  #if defined(CONFIGURATION_DEBUG) || defined (CONFIGURATION_RELEASE)
    #error Multiple configurations were defined! Only one is allowed.
  #endif
  #define TEST_BUILD
  //#define GAME_BUILD
  #define DEBUGGING_ENABLED
  
// DEBUG - runs the game with debug features enabled 
#elif defined CONFIGURATION_DEBUG
  #if defined(CONFIGURATION_TEST) || defined (CONFIGURATION_RELEASE)
    #error Multiple configurations were defined! Only one is allowed.
  #endif
  //#define TEST_BUILD
  #define GAME_BUILD
  #define DEBUGGING_ENABLED
  
// RELEASE - runs the game without any debugging
#elif defined CONFIGURATION_RELEASE
  #if defined(CONFIGURATION_TEST) || defined (CONFIGURATION_DEBUG)
    #error Multiple configurations were defined! Only one is allowed.
  #endif
  //#define TEST_BUILD
  #define GAME_BUILD
  //#define DEBUGGING_ENABLED   // Debugging is not enabled for release builds
  
#else
  #error No valid build configuration defined!
#endif

//--------------------------------------------------------------------------
// Build configuration
//==========================================================================

// Type aliases
using uint8 = uint8_t;
using int8 = int8_t;
using uint16 = uint16_t;
using int16 = int16_t;
using uint32 = uint32_t;
using int32 = int32_t;

// Aliased types for giving the illusion of type-safety
// Some day, maybe these will be replaced with a templated type-safe solution?
using GameTicks = uint8;
using BlockIndex = uint8;


// Define DEBUGGING_ENABLED to enable Asserts, Serial port output, and extra debugging
// Note: If this is defined before the types, I get compiler errors?!?
#ifdef DEBUGGING_ENABLED
char g_debugStr[80];
void DebugPrint(const __FlashStringHelper* msg) { Serial.print(msg); }
void DebugPrintLine(const __FlashStringHelper* msg) { Serial.println(msg); }
void __AssertFunction(const char* func, int line, bool condition, const __FlashStringHelper* msg = nullptr)
{
  if (!condition)
  {
    Serial.print(F("Assert Failed! "));
    Serial.print(func);
    Serial.print(F("("));
    Serial.print(line);
    Serial.print(F(") - "));
    Serial.println(msg);
  }
}
#define Assert(condition, ...) __AssertFunction(__FUNCTION__, __LINE__, (condition), ##__VA_ARGS__)
#else // #ifdef DEBUGGING_ENABLED
void DebugPrint(...) {}
void DebugPrintLine(...) {}
#define Assert(condition, ...) {}
#endif // #else // #ifdef DEBUGGING_ENABLED


// Feature flags for debugging
constexpr bool k_debugDisableGravity = false;   // Default: false; 'true' disables dropping due to gravity


// Gameplay constants

constexpr uint8 k_rotateCwButton = B_BUTTON;
constexpr uint8 k_rotateCcwButton = A_BUTTON;
constexpr uint8 k_leftButton = LEFT_BUTTON;
constexpr uint8 k_rightButton = RIGHT_BUTTON;
constexpr uint8 k_softDropButton = DOWN_BUTTON;
// Note: An unassigned button using the Input class should be 0x00, but using Arduboy2's input functions, it should be 0xFF
constexpr uint8 k_hardDropButton = 0x00;  // There's not a good button for hard drop. I'd prefer to have "hold" than hard drop
constexpr uint8 k_holdButton = UP_BUTTON;

constexpr uint8 k_frameRate = 60;
constexpr uint8 k_screenWidth = 128;
constexpr uint8 k_screenHeight = 64;

// GameTicks are the framerate-agnostic measurement of time used by gameplay logic
// Computing everything in terms of this lets the gameplay feel mostly the same
// with a wide range of frame rates.
// Keeping k_gameTicksPerSecond an integer multiple of frame rate allows for accurate sub-frame timing
constexpr GameTicks k_gameTicksPerSecond = 240;
constexpr GameTicks k_gameTicksPerFrame = k_gameTicksPerSecond / k_frameRate;
static_assert(k_gameTicksPerSecond == k_gameTicksPerFrame * k_frameRate, "Game Ticks should be an integer multiple of the frame rate");

constexpr uint8 k_gridWidth = 10;
constexpr uint8 k_gridHeight = 24;
constexpr uint8 k_defaultPieceSpawnX = 3;
constexpr uint8 k_defaultPieceSpawnY = 21;  // Top 4 rows are hidden
constexpr uint8 k_softDropSpeedScalar = 20;  // TODO: Reevaluate how soft drop speed is calculated to ensure it scales with speed correctly

// Guideline: ~0.3s before auto-repeat kicks in. 72 ticks = 0.3s. This likely wants to be an integer multiple of k_gameTicksPerFrame.
constexpr GameTicks k_autoRepeatFirstDelayTicks = 36; // 36 ticks = 9 frames @ 60fps = 0.15s
// Guideline: ~0.5s to move Tetrimino from one side to the other.
constexpr GameTicks k_autoRepeatContinueDelayTicks = 12; // 12 ticks = 3 frames @ 60fps = 0.05s

//     Top of screen (0)-- *                              *
//                         *                              *
//                         *                              *
//                           . . .
//                         *                              * -\
//                         *                              *   }- Grid Row 1
//                         *                              * -/
//   k_gridBottomPos(60)-- **** -\                        * -\
//                         ****   }- k_blockHeight (3)    *   }- Grid Row 0
//                         **** -/                        * -/
// k_borderBottomPos(63)-- ********************************
constexpr uint8 k_blockWidth = 3;
constexpr uint8 k_blockHeight = 3;
constexpr uint8 k_playspaceWidth = k_gridWidth * k_blockWidth;

constexpr uint8 k_borderLeftPos = (k_screenWidth / 2) - (k_playspaceWidth / 2) - 1;
constexpr uint8 k_borderRightPos = (k_screenWidth / 2) + (k_playspaceWidth / 2);
constexpr uint8 k_borderBottomPos = k_screenHeight - 1;

constexpr uint8 k_gridBottomPos = k_borderBottomPos - k_blockHeight;
constexpr uint8 k_gridLeftPos = (k_screenWidth / 2) - (k_playspaceWidth / 2);

constexpr uint8 k_numNextPiecesToShow = 5;
static_assert(k_numNextPiecesToShow <= 5, "Current implementation of 7-bag piece randomization doesn't support looking ahead more than 5 pieces");

// These positions aren't cleanly defined procedurally. The "magic numbers" don't accurately describe why they're needed.
// Maybe these coordinates should just be straight screen-space coordinates?
constexpr uint8 k_nextDisplayLeftPos = k_gridLeftPos + k_playspaceWidth + (2 * k_blockWidth);
constexpr uint8 k_nextDisplayBottomPos = ((2 + k_numNextPiecesToShow) * 3) * k_blockHeight;
constexpr uint8 k_holdDisplayLeft = k_gridLeftPos - (6 * k_blockWidth);
constexpr uint8 k_holdDisplayBottom = 3 * k_blockHeight;

constexpr uint8 k_maxStartingLevel = 19;

// Type-safe enum for tracking Tetrimino indices
// Note: There are 7 options, so even with an extra entry for "None", this could be stored in 3-bits
enum class PieceIndex : uint8
{
  O,
  I,
  T,
  L,
  J,
  S,
  Z,
  Count,
  Invalid = Count
};

template<typename T>
constexpr T Min(T a, T b) { return (a <= b) ? a : b; }

template<typename T>
constexpr uint8 countof(const T& a) { return sizeof(a) / sizeof(a[0]); }

enum class GameState : uint8
{
  MainMenu,
  Playing,
  GameOver,
};

enum class GameOverReason : uint8
{
  None,     // The game isn't over
  BlockOut, // A piece overlapped with a block on the grid immediately when spawned
};

// Enum of piece orientation. North (0) is default. The rest are ordered clockwise.
enum class PieceOrientation : uint8
{
  North,
  East,
  South,
  West,
  Count
};

enum class RotationDirection : uint8
{
  Clockwise,
  CounterClockwise,
  Count
};

// 2x4 pieces ("I" and "O") use one rotation formula, while
// 2x4 pieces ("T", "L", "J", "S", and "Z") use another
enum class RotationFormula : uint8
{
  Rotation2x4,
  Rotation2x3,
};

class Random
{
public:
  void SetSeed(uint32 seed) { m_seed = seed; }
private:
  uint32 m_seed;
};

class Grid
{
public:
  Grid() {}

  constexpr uint8 GetIndex(uint8 x, uint8 y) { return x + (y * k_gridWidth); }
  constexpr uint8 GetWidth() { return k_gridWidth; }
  constexpr uint8 GetHeight() { return k_gridHeight; }

  void Clear() { memset(m_grid, 0x00, sizeof(m_grid)); }

  // Note: It's not necessary to check for >= 0 because the passed in values are unsigned
  bool IsValidPosition(uint8 x, uint8 y) const { return (x < k_gridWidth) && (y < k_gridHeight); }
  BlockIndex Get(uint8 x, uint8 y) const { return m_grid[GetIndex(x, y)]; }
  void Set(uint8 x, uint8 y, BlockIndex value) { m_grid[GetIndex(x, y)] = value; }
  bool IsEmpty(uint8 x, uint8 y) const { return Get(x, y) == 0; }

#ifdef DEBUGGING_ENABLED
  void DebugPrint(const char* msg) const;
#endif // #ifdef DEBUGGING_ENABLED
  void Draw() const;
  void ProcessFullLines();
  
private:
  BlockIndex m_grid[k_gridWidth * k_gridHeight];
  static_assert(k_gridWidth * k_gridHeight <= 256, "If grid is larger than 256, grid indices will no longer fit in uint8");
};

// Packs an (x, y) rotation offset into one byte
// Bottom 4-bits are signed x offset. Top 4-bits are signed y offset.
// Supported range of this encoding is [-8..7] for each axis
class RotationOffset
{
public:
  RotationOffset(int8 deltaX, int8 deltaY) :
    m_packedOffsets((deltaY << 4) | (deltaX & 0x0f))
  {
#ifdef DEBUGGING_ENABLED
    int8 testX;
    int8 testY;
    UnpackOffset(testX, testY);
    Assert((testX == deltaX) && (testY == deltaY));
#endif // #ifdef DEBUGGING_ENABLED
  }

  void UnpackOffset(int8& outDeltaX, int8& outDeltaY) const
  {
    // Intermediate cast to int8 ensures sign-extension
    outDeltaX = int8(m_packedOffsets << 4) >> 4;
    outDeltaY = m_packedOffsets >> 4;
  }
private:
  // signed value so unpacking will be sign-extended
  int8 m_packedOffsets;
};

class RotationOffsets
{
public:
  constexpr uint8 GetNumRotationOffsets() { return k_numAlternateRotationOffsets + 1; }

  RotationOffset GetRotationOffset(PieceOrientation startingOrientation, RotationDirection direction, uint8 rotationIndex) const
  {
    if (rotationIndex == 0) { return RotationOffset(0, 0); }
    // Because rotationIndex of 0 isn't saved in m_rotationData, the array index is one off of rotationIndex
    return m_rotationData[uint8(direction)][uint8(startingOrientation)][rotationIndex - 1];
  }
  
// Public member so it can use bracket initialization
public:
  static constexpr uint8 k_numAlternateRotationOffsets = 4;
  RotationOffset m_rotationData[uint8(RotationDirection::Count)][uint8(PieceOrientation::Count)][k_numAlternateRotationOffsets];
};

// Data to describe how a certain piece should work, including its shape,
// rotation behavior, and visuals
class PieceData
{
public:
  PieceData(int defaultBlockPositions, RotationFormula rotationFormula, const RotationOffsets* rotationOffsets) :
    m_defaultBlockPositions(defaultBlockPositions),
    m_rotationFormula(rotationFormula),
    m_rotationOffsets(rotationOffsets)
  {
    // TODO: Assert that exactly four bits are set
  }

  // The number of blocks per piece is hard-coded, but the API shouldn't care
  constexpr uint8 GetNumBlocksInPiece() { return 4; }

  // orientation : Orientation of the piece to check (North is default)
  // pieceX, pieceY : (x, y) grid position of piece origin to check 
  // Returns 'true' if all blocks of this piece at this location and orientation are empty in the grid
  // Returns 'false' if something in the grid would block the piece from being here
  bool DoesPieceFitInGrid(PieceOrientation orientation, uint8 pieceX, uint8 pieceY) const;

  // blockIndex : Value in range [0 .. GetNumBlocksInPiece), identifying the block
  // outOffsetX :
  // outOffsetY : Output parameters of (x, y) offset of block with given index
  // Positive-X is right and positive-Y is up in the grid
  // (0, 0) is the bottom-left bit
  // 4567
  // 0123
  void GetBlockOffsetForIndexAndRotation(int8 blockIndex, PieceOrientation orientation, uint8& outOffsetX, uint8& outOffsetY) const;

  // Returns the RotationOffsets data associated with this PieceData. It will be null if there aren't alternate rotations for the piece.
  const RotationOffsets* GetRotationOffsets() const { return m_rotationOffsets; }

  void Draw(uint8 x, uint8 y, PieceOrientation orientation, BlockIndex blockIndex, uint8 leftAnchorScreenPos, uint8 bottomAnchorScreenPos) const;
  
private:
  // Data that describes alternative rotation offsets for the piece.
  // Used to support wall kick, floor kick, and other non-default rotations
  const RotationOffsets* m_rotationOffsets;
  // Bit mask describing piece's blocks in its default orientation.
  uint8 m_defaultBlockPositions;
  // Note: There are only two options, so this could be reduced to just one bit
  RotationFormula m_rotationFormula;
};

class CurrentPiece
{
public:
  inline const PieceData& GetPieceData() const;

  // Minimal code to clear the CurrentPiece. If m_pieceIndex is Invalid, all other data is considered invalid too.
  // Everything else is expected to be initialized in SpawnNewPiece
  void Reset()
  {
    m_pieceIndex = PieceIndex::Invalid;
    m_holdPiece = PieceIndex::Invalid;
    m_holdActionAvailable = true;
    m_canSoftDrop = false;
  }

  // Spawns a new piece at the top of the grid.
  // Returns 'true' if piece spawned without errors
  // Returns 'false' if there were any problems (ie. game over condition)
  bool SpawnNewPiece(PieceIndex knownNextPiece = PieceIndex::Invalid);
  bool IsValidPiece() { return m_pieceIndex != PieceIndex::Invalid; }
  void Draw() const;
  void DrawShadow() const;
  void MoveDown(bool trySoftDrop);
  void DoHardDrop();
  bool TryMove(int8 deltaX, int8 deltaY);
  // rotationDirection: clockwise or counter-clockwise
  bool TryRotate(RotationDirection rotationDirection);
  // Tries to hold the current piece. If it can, the current piece is saved in m_holdPiece and then invalidated
  // and the hold piece is returned
  PieceIndex TryHold();

protected:
  // Writes the current piece to the grid and invalidates the current piece
  void LockPieceInGrid();
  
private:
  PieceIndex m_pieceIndex;
  PieceIndex m_holdPiece;
  uint8 m_x;
  uint8 m_y;
  PieceOrientation m_orientation;
  // How many GameTicks until this piece falls to the next line
  GameTicks m_ticksToFall;
  bool m_holdActionAvailable;
  // Tracks whether soft drop can be applied to this piece. Necessary to avoid a single soft-drop input hold from unintentionally affecting subsequent pieces.
  bool m_canSoftDrop;
};

// Manages the next piece and whatever randomization method is used to pick them
class Next
{
public:
#ifdef TEST_BUILD
  static void UnitTest();
  // Prints the current state of the next queue
  void DebugPrint() const;
#endif // #ifdef TEST_BUILD
  
  void Reset();
  PieceIndex GetNextPiece();
  void Draw(BlockIndex blockIndex) const;
  
private:
  void ShuffleBag(uint8 startingIndex);
private:
  PieceIndex m_next[14];
  uint8 m_index;
};

class Controller
{
public:
  void Reset() { m_isSoftDrop = 0; }
  void ProcessInput();
  bool IsSoftDrop() const { return m_isSoftDrop; }

private:
  // Helper function for handling horizontal auto-repeat timing
  static uint8 ProcessMoveHorizontal(uint8 button, uint8& out_ticksUntilAutoRepeat);
  
private:
  GameTicks m_ticksUntilAutoRepeatLeft;
  GameTicks m_ticksUntilAutoRepeatRight;
  bool m_isSoftDrop;
  bool m_hardDropButtonWasDown;
};

class GameMode
{
public:
  // Returns how many GameTicks it takes for a piece to fall one line with no input
  GameTicks GetFallTime() const;

  void Reset() { m_level = 0; }
  void SetLevel(uint8 level) { m_level = level; }
  void NextLevel() { m_level++; }
private:
  uint8 m_level;
};

class Menus
{
public:
  void Reset()
  {
    m_selectedIndex = 0;
    m_startingLevel = 0;
  }

  void Loop();
  void ProcessInput();

private:
  uint8 m_selectedIndex;
  uint8 m_startingLevel;
};

class Input
{
public:
  void Update();
  // Returns true if the current state of the button is down, ignoring any history
  bool IsButtonDown(uint8 button) const { return (button & m_currentButtonDownFlags); }
  // Returns true if the button is down now, but wasn't last frame
  bool WasButtonPressed(uint8 button) const { return (button & m_currentButtonDownFlags & ~m_previousButtonDownFlags); }
  // Returns true if the button is down this frame and was also down last frame
  bool WasButtonHeld(uint8 button) const { return (button & m_currentButtonDownFlags & m_previousButtonDownFlags); }
  // Returns true if the button is up this frame, but was down last frame
  bool WasButtonReleased(uint8 button) const { return (button & ~m_currentButtonDownFlags & m_previousButtonDownFlags); }

private:
  static bool SampleRawInput(uint8 buttons);

private:
  uint8 m_currentButtonDownFlags = 0;
  uint8 m_previousButtonDownFlags = 0;
};

// The global object that contains and manages all other objects
// At the time of writing, not everything is contained within Global, but things are moving that way
class Global
{
public:
  static bool SampleRawInput(uint8 buttons);

  void Loop();
  const Input& GetInput() const { return m_input; }
private:
  Input m_input;
};

// TODO: Move this out of dynamic memory
const char* k_menuItems[] = {"Play", "Mode", "Level", "Skin", "Shadow"};


//==========================================================================
// Global variables
//--------------------------------------------------------------------------
class Arduboy2 arduboy;
Global g;

GameState g_gameState;
class Grid g_grid;
class CurrentPiece g_currentPiece;
class Next g_next;
class Controller g_controller;
class GameMode g_gameMode;
class Menus g_menus;

// "I" Piece Rotations
const RotationOffsets k_rotationOffsetsI = {{
  {
    // Clockwise Rotations
    {{-2, 0}, { 1, 0}, {-2, -1}, { 1,  2}},  // North -> East
    {{-1, 0}, { 2, 1}, {-1,  2}, { 2, -1}},  // East -> South
    {{ 2, 0}, {-1, 0}, { 2,  1}, {-1, -2}},  // South -> West
    {{ 1, 0}, {-2, 0}, { 1, -2}, {-2,  1}}   // West -> North
  },
  {
    // Counter-Clockwise Rotations
    {{-1, 0}, { 2, 0}, {-2,  2}, { 2, -1}},  // North -> West
    {{ 2, 0}, {-1, 0}, { 2,  1}, {-1, -2}},  // East -> North
    {{ 1, 0}, {-2, 0}, { 1, -2}, {-2,  1}},  // South -> East
    {{-2, 0}, { 1, 0}, {-2, -1}, { 1,  2}}   // West -> South
  }
}};

// "T" Piece Rotations
const RotationOffsets k_rotationOffsetsT = {{
  {
    // Clockwise Rotations
    {{-1, 0}, {-1,  1}, { 0,  0}, {-1, -2}},  // North -> East
    {{ 1, 0}, { 1,  1}, { 0,  2}, { 1,  2}},  // East -> South
    {{ 1, 0}, { 0,  0}, { 0,  2}, { 1,  2}},  // South -> West
    {{-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}   // West -> North
  },
  {
    // Counter-Clockwise Rotations
    {{ 1, 0}, { 1,  1}, { 0,  0}, { 1, -2}},  // North -> West
    {{ 1, 0}, { 1, -1}, { 0,  2}, { 1,  2}},  // East -> North
    {{-1, 0}, { 0,  0}, { 0,  2}, {-1,  2}},  // South -> East
    {{-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}   // West -> South
  }
}};

// "L" and "J" Piece Rotations
const RotationOffsets k_rotationOffsetsLJSAndZ = {{
  {
    // Clockwise Rotations
    {{-1, 0}, {-1,  1}, { 0, -2}, {-1, -2}},  // North -> East
    {{ 1, 0}, { 1, -1}, { 0,  2}, { 1,  2}},  // East -> South
    {{ 1, 0}, { 1,  1}, { 0, -2}, { 1, -2}},  // South -> West
    {{-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}   // West -> North
  },
  {
    // Counter-Clockwise Rotations
    {{ 1, 0}, { 1,  1}, { 0, -2}, { 1, -2}},  // North -> West
    {{ 1, 0}, { 1, -1}, { 0,  2}, { 1,  2}},  // East -> North
    {{-1, 0}, {-1,  1}, { 0, -2}, {-1, -2}},  // South -> East
    {{-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}   // West -> South
  }
}};

class PieceData g_pieceData[] = {
  // O = 0110 0110 = 0x66
  {0x66, RotationFormula::Rotation2x4, nullptr},
  // I = 1111 = 0xF0
  {0xF0, RotationFormula::Rotation2x4, &k_rotationOffsetsI},
  // T = 0010 0111 = 0x27
  {0x27, RotationFormula::Rotation2x3, &k_rotationOffsetsT},
  // L = 0100 0111 = 0x47
  {0x47, RotationFormula::Rotation2x3, &k_rotationOffsetsLJSAndZ},
  // J = 0001 0111 = 0x17
  {0x17, RotationFormula::Rotation2x3, &k_rotationOffsetsLJSAndZ},
  // S = 0110 0011 = 0x63
  {0x63, RotationFormula::Rotation2x3, &k_rotationOffsetsLJSAndZ},
  // Z = 0011 0110 = 0x36
  {0x36, RotationFormula::Rotation2x3, &k_rotationOffsetsLJSAndZ},
};
static_assert(countof(g_pieceData) == uint8(PieceIndex::Count));

//--------------------------------------------------------------------------
// Global variables
//==========================================================================

//==========================================================================
// Entry points for GAME_BUILD
//--------------------------------------------------------------------------
#ifdef GAME_BUILD

void setup()
{
#ifdef DEBUGGING_ENABLED
  Serial.begin(9600);
  while(!Serial); // wait for serial port to connect. Needed for native USB
#endif // #ifdef DEBUGGING_ENABLED
  arduboy.begin();
  arduboy.setFrameRate(k_frameRate);
  ResetGame();
}

void loop()
{
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
  {
    return;
  }

  g.Loop();

  arduboy.display();
}

#endif // #ifdef GAME_BUILD
//--------------------------------------------------------------------------
// Entry points for GAME_BUILD
//==========================================================================

//==========================================================================
// Entry points for TEST_BUILD
//--------------------------------------------------------------------------
#ifdef TEST_BUILD

#define RunTest(test) __RunTestHelper(F(#test), test)
// Forward declare testing function
void __RunTestHelper(void(*testFunction)());

// Global test counters
int g_testPassCount;
int g_testFailCount;

#define TestVerify(condition) { if(condition){g_testPassCount++;}else{g_testFailCount++;Serial.print(F("Test Failed("));Serial.print(__LINE__);Serial.print(F("): "));Serial.println(F(#condition));}}

void setup()
{
#ifdef DEBUGGING_ENABLED
  Serial.begin(9600);
  while(!Serial); // wait for serial port to connect. Needed for native USB
#endif // #ifdef DEBUGGING_ENABLED
  arduboy.begin();
  arduboy.setFrameRate(k_frameRate);
}

void loop()
{
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
  {
    return;
  }

  static uint8 s_frameNum = 0;
  switch(s_frameNum)
  {
    case 0: arduboy.clear(); break;
    case 1: RunTest(Next::UnitTest); break;
    case 2: RunTest(TestFailure); break;
    case 3: break;
  }
  if (s_frameNum < 255) { s_frameNum++; }
  
  arduboy.display();
}

void TestFailure()
{
  TestVerify(1 + 1 == 2);
  TestVerify(uint8(uint8(255) + uint8(1)) == 0);
  TestVerify(1 + 7 == 93);  // Fail
  TestVerify(2 * 2 == 4);
}

void __RunTestHelper(const __FlashStringHelper* testName, void(*testFunction)())
{
  g_testPassCount = 0;
  g_testFailCount = 0;
  (*testFunction)();
  if (g_testFailCount == 0)
  {
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);
  }
  else
  {
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
  }
  arduboy.print(testName);
  arduboy.print(F(": "));
  arduboy.print(g_testPassCount);
  arduboy.print(F("/"));
  arduboy.println(g_testPassCount + g_testFailCount);
}

#endif // #ifdef TEST_BUILD
//--------------------------------------------------------------------------
// Entry points for TEST_BUILD
//==========================================================================

void Global::Loop()
{
  m_input.Update();
  
  switch (g_gameState)
  {
    case GameState::MainMenu:
      g_menus.Loop();
    break;
    case GameState::Playing:
      PlayingLoop();
      break;
    case GameState::GameOver:
      GameOverLoop();
      break;
  }
}


void Input::Update()
{
  m_previousButtonDownFlags = m_currentButtonDownFlags;
  m_currentButtonDownFlags = 0x00;
  for (uint8 i = 0; i < 8; i++)
  {
    uint8 buttonFlag = 1 << i;
    if (SampleRawInput(buttonFlag))
    {
      m_currentButtonDownFlags |= buttonFlag;
    }
  }
}

// static
bool Input::SampleRawInput(uint8 buttons)
{
  return arduboy.pressed(buttons);
}

void ResetGame()
{
  arduboy.clear();
  
  g_grid.Clear();
  g_gameMode.Reset();
  // TODO: This should be incorporated into GameMode
  g_gameState = GameState::MainMenu;

  g_next.Reset();
  g_currentPiece.Reset();
  g_controller.Reset();

  g_menus.Reset();
}

void Menus::Loop()
{
  ProcessInput();

  const Input& input = g.GetInput();
  switch (m_selectedIndex)
  {
    case 0: // "Play"
      if (input.WasButtonPressed(A_BUTTON) | input.WasButtonPressed(B_BUTTON))
      {
        // TODO: Clean this up! There needs to be a better way to manage this state
        // Dirty workaround since m_startingLevel gets cleared in ResetGame()
        uint8 startingLevel = m_startingLevel;
        ResetGame();
        g_gameMode.SetLevel(startingLevel);
        g_gameState = GameState::Playing;
      }
      break;
    case 1: // "Mode"
      break;
    case 2: // "Level"
      if ((m_startingLevel < k_maxStartingLevel) && (input.WasButtonPressed(B_BUTTON) || input.WasButtonPressed(RIGHT_BUTTON)))
      {
        m_startingLevel++;
      }
      if ((m_startingLevel > 0) && (input.WasButtonPressed(A_BUTTON) || input.WasButtonPressed(LEFT_BUTTON)))
      {
        m_startingLevel--;
      }
      break;
    case 3: // "Skin"
      break;
    case 4: // "Shadow"
      break;
  }

  arduboy.clear();
  // TODO: Clean this up. State transitions should be cleaner and not have to rely on hacks like this to clear the screen
  // Early-out if we're not in the main menu game state anymore.
  if (g_gameState != GameState::MainMenu)
  {
    return;
  }
  
  for (uint8 i = 0; i < countof(k_menuItems); i++)
  {
    arduboy.print(m_selectedIndex == i ? F("> ") : F("   "));
    arduboy.print(k_menuItems[i]);
    switch (i)
    {
      case 2:
        arduboy.print(F(" ["));
        arduboy.print(m_startingLevel);
        arduboy.print(F("]"));
        break;
    }
    arduboy.println();
  }
}

void Menus::ProcessInput()
{
  const Input& input = g.GetInput();
  if (input.WasButtonPressed(UP_BUTTON))
  {
    m_selectedIndex += countof(k_menuItems) - 1;
  }
  if (input.WasButtonPressed(DOWN_BUTTON))
  {
    m_selectedIndex++;
  }
  m_selectedIndex = m_selectedIndex % countof(k_menuItems);
}

void PlayingLoop()
{
  GameOverReason gameOverReason = GameOverReason::None;

  // "Hold" piece support
  PieceIndex knownNextPiece = PieceIndex::Invalid;
  if (g.GetInput().WasButtonPressed(k_holdButton))
  {
    // Hold is allowed to be used once per drop. It won't do anything if it's already been used.
    knownNextPiece = g_currentPiece.TryHold();
  }
  
  if (!g_currentPiece.IsValidPiece())
  {
    // Test for, and remove full lines
    g_grid.ProcessFullLines();
    const bool spawnSuccess = g_currentPiece.SpawnNewPiece(knownNextPiece);
    if (!spawnSuccess)
    {
      gameOverReason = GameOverReason::BlockOut;
    }
  }

  if (gameOverReason == GameOverReason::None)
  {
    g_controller.ProcessInput();
    g_currentPiece.MoveDown(g_controller.IsSoftDrop());
  }
  else
  {
    // TODO: Should I add more proper state management? ie. a real state machine?
    g_gameState = GameState::GameOver;
  }

  // TODO: Don't draw the entire grid every frame when it hasn't changed
  g_grid.Draw();
  g_currentPiece.DrawShadow();
  g_currentPiece.Draw();
}

void GameOverLoop()
{
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.setCursorX((k_screenWidth - (9 * 5)) / 2);
  arduboy.setCursorY((k_screenHeight - 7) / 2);
  arduboy.print(F("Game Over"));

  if (g.GetInput().WasButtonReleased(A_BUTTON) || g.GetInput().WasButtonReleased(B_BUTTON))
  {
    ResetGame();
  }
}

static void DrawBlock(uint8 x, uint8 y, BlockIndex block, uint8 leftAnchorScreenPos, uint8 bottomAnchorScreenPos)
{
  const uint8 left = leftAnchorScreenPos + (x * k_blockWidth);
  const uint8 top = bottomAnchorScreenPos - (y * k_blockHeight);
  if (block == 0)
  {
    arduboy.fillRect(left, top, k_blockWidth, k_blockHeight, BLACK);
  }
  else if (block == 1)
  {
    arduboy.drawRect(left, top, k_blockWidth, k_blockHeight, WHITE);
    arduboy.drawPixel(left + 1, top + 1, BLACK);
  }
  else
  {
    // For now, this case is mostly for debugging if there's unexpected data in the grid.
    // Eventually there will be a variety of block visuals
    arduboy.drawRect(left, top, k_blockWidth, k_blockHeight, BLACK);
    arduboy.drawPixel(left + 1, top + 1, WHITE);
  }
}

#ifdef DEBUGGING_ENABLED
void Grid::DebugPrint(const char* msg) const
{
  Serial.print(__FUNCTION__);
  Serial.println(msg);
  for (uint8 y = 0; y < k_gridHeight; y++)
  {
    for (uint8 x = 0; x < k_gridWidth; x++)
    {
      Serial.print(Get(x, y));
    }
    Serial.println();
  }
  Serial.println();
}
#endif // #ifdef DEBUGGING_ENABLED

void Grid::Draw() const
{
  // Draw border lines
  arduboy.drawLine(k_borderLeftPos, 0, k_borderLeftPos, k_borderBottomPos, WHITE);
  arduboy.drawLine(k_borderRightPos, 0, k_borderRightPos, k_borderBottomPos, WHITE);
  arduboy.drawLine(k_borderLeftPos, k_borderBottomPos, k_borderRightPos, k_borderBottomPos, WHITE);
  
  // Draw blocks
  for (uint8 y = 0; y < k_gridHeight; y++)
  {
    for (uint8 x = 0; x < k_gridWidth; x++)
    {
      DrawBlock(x, y, Get(x, y), k_gridLeftPos, k_gridBottomPos);
    }
  }
}

void Grid::ProcessFullLines()
{
  uint8 numCleared = 0;
  for (uint8 y = 0; y < k_gridHeight; y++)
  {
    bool lineFull = true;
    for (uint8 x = 0; x < k_gridWidth; x++)
    {
      BlockIndex block = Get(x, y);
      // Copy block to row below if any lines were cleared. Set is a no-op if numCleared is 0
      Set(x, y - numCleared, block);
      if (block == 0)
      {
        lineFull = false;
      }
    }
    if (lineFull)
    {
      numCleared++;
    }
  }

  // Make sure the lines above the highest one copied down gets zeroed out
  for (uint8 y = k_gridHeight - numCleared; y < k_gridHeight; y++)
  {
    for (uint8 x = 0; x < k_gridWidth; x++)
    {
      Set(x, y, 0);
    }
  }
}

bool PieceData::DoesPieceFitInGrid(PieceOrientation orientation, uint8 pieceX, uint8 pieceY) const
{
  uint8 blockOffsetX;
  uint8 blockOffsetY;
  for (uint8 blockIndex = 0; blockIndex < GetNumBlocksInPiece(); blockIndex++)
  {
    GetBlockOffsetForIndexAndRotation(blockIndex, orientation, blockOffsetX, blockOffsetY);
    uint8 testX = pieceX + blockOffsetX;
    uint8 testY = pieceY + blockOffsetY;
    if (!(g_grid.IsValidPosition(testX, testY) && g_grid.IsEmpty(testX, testY)))
    {
      // Something is blocking this piece; early-out
      return false;
    }
  }
  // Nothing blocked the piece
  return true;
}

void PieceData::GetBlockOffsetForIndexAndRotation(int8 blockIndex, PieceOrientation orientation, uint8& outOffsetX, uint8& outOffsetY) const
{
  // WARNING - Bad data or bad input will cause this to hang!
  int8 bitIndex = -1;
  do
  {
    do
    {
      bitIndex++;
      Assert(bitIndex < 8, F("m_defaultBlockPositions and/or blockIndex were bad"));
    } while ((m_defaultBlockPositions & (uint8(0x01) << bitIndex)) == 0);
    blockIndex--;
  } while (blockIndex >= 0);

  uint8 m4 = bitIndex & 0x03;   // m4 = bitIndex % 4;
  uint8 d4 = bitIndex / 4;      // d4 = bitIndex / 4;

  Assert(m_rotationFormula == RotationFormula::Rotation2x4 || m_rotationFormula == RotationFormula::Rotation2x3);
  // Rotation of 2x3 blocks is very similar to 2x4. Rather than having two separate
  // calculations, the differences are encapsulated in a few targeted fixups
  int8 fixup = (m_rotationFormula == RotationFormula::Rotation2x4) ? 0 : -1;
  switch (orientation)
  {
    case PieceOrientation::North:
      outOffsetX = m4;
      outOffsetY = d4 + 1;
      break;
    case PieceOrientation::East:
      outOffsetX = d4 + 1;
      outOffsetY = (3 - m4) + fixup;
      break;
    case PieceOrientation::South:
      outOffsetX = (3 - m4) + fixup;
      outOffsetY = (2 - d4) + fixup;
      break;
    case PieceOrientation::West:
      outOffsetX = (2 - d4) + fixup;
      outOffsetY = m4;
      break;
  }
}

void PieceData::Draw(uint8 x, uint8 y, PieceOrientation orientation, BlockIndex blockIndex, uint8 leftAnchorScreenPos, uint8 bottomAnchorScreenPos) const
{
  uint8 dx;
  uint8 dy;
  for (uint8 i = 0; i < GetNumBlocksInPiece(); i++)
  {
    GetBlockOffsetForIndexAndRotation(i, orientation, dx, dy);
    DrawBlock(x + dx, y + dy, blockIndex, leftAnchorScreenPos, bottomAnchorScreenPos);
  }
}

const PieceData& CurrentPiece::GetPieceData() const
{
  return g_pieceData[uint8(m_pieceIndex)];
}

bool CurrentPiece::SpawnNewPiece(PieceIndex knownNextPiece)
{
  if (knownNextPiece != PieceIndex::Invalid)
  {
    // Assume this came from the hold piece, so the hold action doesn't get reset
    m_pieceIndex = knownNextPiece;
    DebugPrint(F("NextPieceFromHold"));
  }
  else
  {
    // Pull next piece from 7-bag (or other randomization abstraction)
    m_pieceIndex = g_next.GetNextPiece();
  }
  m_x = k_defaultPieceSpawnX;
  m_y = k_defaultPieceSpawnY;
  m_orientation = PieceOrientation::North;
  m_ticksToFall = g_gameMode.GetFallTime();

#ifdef DEBUGGING_ENABLED
  const char* k_pieces[] = {"O", "I", "T", "L", "J", "S", "Z"};
  Serial.println(k_pieces[uint8(m_pieceIndex)]);
#endif // #ifdef DEBUGGING_ENABLED
  
  // Check if new piece overlaps with anything on the board
  return GetPieceData().DoesPieceFitInGrid(m_orientation, m_x, m_y);
}

void CurrentPiece::Draw() const
{
  if (m_pieceIndex != PieceIndex::Invalid)
  {
    const BlockIndex blockIndex = 1;
    GetPieceData().Draw(m_x, m_y, m_orientation, blockIndex, k_gridLeftPos, k_gridBottomPos);
  }
}

void CurrentPiece::DrawShadow() const
{
  // TODO: Merge this function with Draw()
  if (m_pieceIndex != PieceIndex::Invalid)
  {
    const PieceData& pieceData = GetPieceData();
    uint8 shadowY = m_y;
    while (pieceData.DoesPieceFitInGrid(m_orientation, m_x, shadowY - 1))
    {
      shadowY--;
    }
    if (shadowY != m_y)
    {
      const BlockIndex shadowBlockIndex = 2;
      pieceData.Draw(m_x, shadowY, m_orientation, shadowBlockIndex, k_gridLeftPos, k_gridBottomPos);
    }
  }
}

void CurrentPiece::MoveDown(bool trySoftDrop)
{
  if (k_debugDisableGravity)
  {
    if (!trySoftDrop)
    {
      return;
    }
  }

  uint8 ticksToSubtract = k_gameTicksPerFrame;
  if (trySoftDrop)
  {
    if (m_canSoftDrop)
    {
      ticksToSubtract *= k_softDropSpeedScalar;
    }
  }
  else
  {
    // Allow soft dropping after a single frame of not soft dropping.
    // This forces the user to release the soft drop button between pieces.
    m_canSoftDrop = true;
  }

  // Loop to handle moving multiple lines per update
  while (ticksToSubtract > 0)
  {
    if (ticksToSubtract < m_ticksToFall)
    {
      // Piece doesn't move. Subtract off time and return.
      m_ticksToFall -= ticksToSubtract;
      break;
    }
    else
    {
      if (TryMove(0, -1))
      {
        // Piece moved one line down. Subtract any remaining ticks to fall from the total to count and continue looping.
        ticksToSubtract -= m_ticksToFall;
        m_ticksToFall = g_gameMode.GetFallTime();
      }
      else
      {
        // Piece couldn't fall. Lock it in, zero-out the current piece, and return.
        LockPieceInGrid();
        break;
      }
    }
  }
}

void CurrentPiece::DoHardDrop()
{
  DebugPrintLine(F("HardDrop"));
  while (TryMove(0, -1))
  {
  }
  LockPieceInGrid();
}

// Return 'true' if move was successful
bool CurrentPiece::TryMove(int8 deltaX, int8 deltaY)
{
  if (GetPieceData().DoesPieceFitInGrid(m_orientation, m_x + deltaX, m_y + deltaY))
  {
    // All the checks passed; move the piece
    m_x += deltaX;
    m_y += deltaY;
    // Move succeeded; return true
    return true;
  }
  // Move failed; return false
  return false;
}

// Return 'true' if rotation was successful
bool CurrentPiece::TryRotate(RotationDirection rotationDirection)
{
  PieceOrientation testOrientation = PieceOrientation(int8(m_orientation) + ((rotationDirection == RotationDirection::Clockwise) ? +1 : -1));
  testOrientation = PieceOrientation((int8(testOrientation) + int8(PieceOrientation::Count)) % int8(PieceOrientation::Count));

  const PieceData& pieceData = GetPieceData();
  const RotationOffsets* rotationOffsets = pieceData.GetRotationOffsets();
  const uint8 numRotationsToTest = (rotationOffsets == nullptr) ? 1 : rotationOffsets->GetNumRotationOffsets();
  for (int rotationIndex = 0; rotationIndex < numRotationsToTest; rotationIndex++)
  {
    int8 deltaX = 0;
    int8 deltaY = 0;
    if (rotationOffsets != nullptr)
    {
      RotationOffset offset = rotationOffsets->GetRotationOffset(m_orientation, rotationDirection, rotationIndex);
      offset.UnpackOffset(deltaX, deltaY);
#ifdef DEBUGGING_ENABLED
      sprintf(g_debugStr,
        "Piece:%d (%d, %d) Orient:%s Rot:%s[%d] delta(%d, %d)\n",
        int(m_pieceIndex),
        int(m_x),
        int(m_y),
        m_orientation == PieceOrientation::North ? ("N") : (m_orientation == PieceOrientation::East ? ("E") : (m_orientation == PieceOrientation::South ? ("S") : ("W"))),
        (rotationDirection == RotationDirection::Clockwise) ? ("cw") : ("ccw"),
        int(rotationIndex),
        int(deltaX),
        int(deltaY)
      );
      Serial.print(g_debugStr);
#endif // #ifdef DEBUGGING_ENABLED
    }
    
    if (GetPieceData().DoesPieceFitInGrid(testOrientation, m_x + deltaX, m_y + deltaY))
    {
      m_x += deltaX;
      m_y += deltaY;
      m_orientation = testOrientation;
      // Rotation succeeded; return true
      return true;
    }
  }
  
  // Rotation failed; return false
  return false;
}

PieceIndex CurrentPiece::TryHold()
{
  PieceIndex oldHoldPiece = PieceIndex::Invalid;
  if (m_holdActionAvailable)
  {
    // Disable the hold actio until it gets reset
    m_holdActionAvailable = false;
    // Return the hold piece so the caller can respawn it at the top
    oldHoldPiece = m_holdPiece;
    // Save the current piece in the hold slot
    m_holdPiece = m_pieceIndex;
    // Set the current piece to Invalid so it can be respawned
    m_pieceIndex = PieceIndex::Invalid;

    // Update the Hold display
    // NOTE: This only works if this code block is the only place m_holdPiece is modified during gameplay
    if (oldHoldPiece != PieceIndex::Invalid)
    {
      const BlockIndex clearBlock = 0;
      g_pieceData[uint8(oldHoldPiece)].Draw(0, 0, PieceOrientation::North, clearBlock, k_holdDisplayLeft, k_holdDisplayBottom);
    }
    Assert(m_holdPiece != PieceIndex::Invalid);
    const BlockIndex nextBlock = 1;
    g_pieceData[uint8(m_holdPiece)].Draw(0, 0, PieceOrientation::North, nextBlock, k_holdDisplayLeft, k_holdDisplayBottom);
    
    DebugPrintLine(F("Hold"));
  }
  return oldHoldPiece;
}

void CurrentPiece::LockPieceInGrid()
{
  // Write all the blocks to the grid
  const PieceData& pieceData = GetPieceData();
  for (uint8 blockIndex = 0; blockIndex < pieceData.GetNumBlocksInPiece(); blockIndex++)
  {
    uint8 blockOffsetX;
    uint8 blockOffsetY;
    pieceData.GetBlockOffsetForIndexAndRotation(blockIndex, m_orientation, blockOffsetX, blockOffsetY);
    g_grid.Set(m_x + blockOffsetX, m_y + blockOffsetY, 1);
  }

  // Invalidate the piece now that it's been written to the grid
  m_pieceIndex = PieceIndex::Invalid;
  
  // The hold action gets reset whenever a piece is locked down
  m_holdActionAvailable = true;
  m_canSoftDrop = false;
}

//==========================================================================
// Unit tests for - Next class
//--------------------------------------------------------------------------
#ifdef TEST_BUILD
void Next::UnitTest()
{
  Next test;
  memset(&test, 0x00, sizeof(test));
  TestVerify(test.m_next[0] == PieceIndex(0));

  // Verify initial shuffle contains one of each piece
  test.Reset();
  for (uint8 i = 0; i < uint8(PieceIndex::Count); i++)
  {
    PieceIndex piece = PieceIndex(i);
    bool pieceFound = false;
    for (uint8 j = 0; j < uint8(PieceIndex::Count); j++)
    {
      if (test.m_next[j] == piece)
      {
        pieceFound = true;
      }
    }
    TestVerify(pieceFound);
  }

  uint8 pieceCounts[uint8(PieceIndex::Count)] = {0};
  const uint8 bagIterations = 50;
  for (uint8 iterations = 0; iterations < bagIterations; iterations++)
  {
    for (uint8 i = 0; i < uint8(PieceIndex::Count); i++)
    {
      test.DebugPrint();
      pieceCounts[uint8(test.GetNextPiece())]++;
    }
  
    // Make sure all pieces were chosen the same number of times
    bool allChosen = true;
    for (uint8 i = 0; i < countof(pieceCounts); i++)
    {
      TestVerify(pieceCounts[i] == iterations + 1);
    }
  }
}

void Next::DebugPrint() const
{
  const char* k_pieces[] = {"O", "I", "T", "L", "J", "S", "Z"};
  for (uint8 i = 0; i < countof(m_next); i++)
  {
    if (i == m_index)
    {
      Serial.print(F("["));
    }
    Serial.print(k_pieces[uint8(m_next[i])]);
    if (i == m_index)
    {
      Serial.print(F("]"));
    }
  }
  Serial.println();
}
#endif // #ifdef TEST_BUILD
//--------------------------------------------------------------------------
// Unit tests for - Next class
//==========================================================================

void Next::Reset()
{
  // TODO: Randomize things better. Maybe take into acount player input?
  arduboy.initRandomSeed();
  // Initialize two bags
  ShuffleBag(0);
  ShuffleBag(7);
  m_index = 0;
}

PieceIndex Next::GetNextPiece()
{
  // Hacky way to clear the previous "Next" display. First clear the old display, then draw the new one.
  // It's nice in that it only updates the display when something changes, but drawing in GetNextPiece feels dirty.
  Draw(0);
  PieceIndex nextPiece = m_next[m_index];
  m_index++;
  // TODO: Make this magic number a constant or computed?
  // If the index has progressed this far, another bag needs to be randomized
  if (m_index >= 7)
  {
    Assert(m_index == 7);
    // This is likely less code than a loop to copy, or to treat the whole think like a ring buffer
    memmove(m_next, m_next + m_index, (countof(m_next) * sizeof(*m_next)) - m_index);
    ShuffleBag(7);
    m_index = 0;
  }
#ifdef GAME_BUILD
  // Update the Next display in game builds
  // TODO: Figure out a nicer way to do this and not have drawing code in the middle of GetNextPiece
  Draw(1);
#endif // #ifdef GAME_BUILD
  return nextPiece;
}

void Next::Draw(BlockIndex blockIndex) const
{
  for (uint8 i = 0; i < k_numNextPiecesToShow; i++)
  {
    const PieceData& pieceData = g_pieceData[uint8(m_next[m_index + i])];
    // TODO: What block index should be used for the Next display?
    // TODO: Formalize the position of these draws
    pieceData.Draw(0, (1 + k_numNextPiecesToShow - i) * 3, PieceOrientation::North, blockIndex, k_nextDisplayLeftPos, k_nextDisplayBottomPos);
  }
}

void Next::ShuffleBag(uint8 startingIndex)
{
  Assert(startingIndex <= countof(m_next) - uint8(PieceIndex::Count));
  // Put one of each piece in the "bag"
  for (uint8 i = 0; i < uint8(PieceIndex::Count); i++)
  {
    m_next[i + startingIndex] = PieceIndex(i);
  }

  // Shuffle the bag
  for (uint8 i = 0; i < uint8(PieceIndex::Count); i++)
  {
    uint8 swapIndex = random(i, uint32(PieceIndex::Count));
    PieceIndex temp = m_next[i + startingIndex];
    m_next[i + startingIndex] = m_next[swapIndex + startingIndex];
    m_next[swapIndex + startingIndex] = temp;
  }
}

// button: The input button to query (ie. LEFT_BUTTON, or RIGHT_BUTTON
// moveDirection: Expected to be -1 if input moves piece left and +1 if input moves piece right
// out_ticksUntilAutoRepeat: in/out parameter of the variable tracking auto repeat time
// returns: number of spaces to move
uint8 Controller::ProcessMoveHorizontal(uint8 button, uint8& out_ticksUntilAutoRepeat)
{
  int8 moveAmount = 0;
  if (g.GetInput().IsButtonDown(button))
  {
    // A value of '0' here indicates the button wasn't down the previous frame
    if (out_ticksUntilAutoRepeat == 0)
    {
      // Initial press may have a different repeat delay
      out_ticksUntilAutoRepeat = k_autoRepeatFirstDelayTicks;
      moveAmount++;
    }
    else
    {
      // Loop here to handle edge cases. Ensure out_ticksUntilAutoRepeat is never 0 and handle multiple repeats in one frame.
      while (out_ticksUntilAutoRepeat <= k_gameTicksPerFrame)
      {
        // Auto-repeat timer triggers this frame. Move the piece and adjust the timer.
        out_ticksUntilAutoRepeat += k_autoRepeatContinueDelayTicks;
        moveAmount++;
      }
      // Count down the timer
      out_ticksUntilAutoRepeat -= k_gameTicksPerFrame;
    }
  }
  else
  {
    // Button is released, reset auto-repeat timer
    out_ticksUntilAutoRepeat = 0;
  }
  return moveAmount;
}

void Controller::ProcessInput()
{
  // Handle horizontal input and movement
  int8 moveAmount = 0;
  moveAmount -= ProcessMoveHorizontal(k_leftButton, m_ticksUntilAutoRepeatLeft);
  moveAmount += ProcessMoveHorizontal(k_rightButton, m_ticksUntilAutoRepeatRight);
  
  int8 moveDelta = (moveAmount < 0) ? -1 : +1;
  while (moveAmount != 0)
  {
    g_currentPiece.TryMove(moveDelta, 0);
    moveAmount -= moveDelta;
  }

  // Handle rotation input
  const Input& input = g.GetInput();
  if (input.WasButtonPressed(k_rotateCwButton))
  {
    g_currentPiece.TryRotate(RotationDirection::Clockwise);
  }

  if (input.WasButtonPressed(k_rotateCcwButton))
  {
    g_currentPiece.TryRotate(RotationDirection::CounterClockwise);
  }

  // Handle drop input
  m_isSoftDrop = input.IsButtonDown(k_softDropButton);

  bool hardDropButtonDown = input.IsButtonDown(k_hardDropButton);
  if (hardDropButtonDown && !m_hardDropButtonWasDown)
  {
    g_currentPiece.DoHardDrop();
  }
  m_hardDropButtonWasDown = hardDropButtonDown;
}

uint8 GameMode::GetFallTime() const
{
  // Formula for calculating fall speed based on level is...
  // (0.8 - ((level - 1) * 0.007))^(level-1) = seconds/line
  // This table assumes 240 Ticks/Second
  // Level 1 = 240 Ticks/Line = 1 Second/Line
  // Level 2 = 190 Ticks/Line = 0.793 Seconds/Line, etc...
  // See off-line calculations here - https://docs.google.com/spreadsheets/d/1j6nqMDazV5cpRodXMpWqeaF-EM9KFNCSMc6DYBvs_V8/edit#gid=0
  static constexpr PROGMEM uint8 k_fallSpeeds[] = {
    240, 190, 148, 113, 85, 63, 46, 32, 23, 15, 10, 7, 4, 3, 2, 1, 1, 0
  };
  constexpr uint8 k_numFallSpeeds = countof(k_fallSpeeds) - 1;
  
  // If m_level is bigger than the above array supports, clamp to the last (fastest) value
  return pgm_read_byte_near(k_fallSpeeds + Min(m_level, k_numFallSpeeds));
}
