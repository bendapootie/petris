
// These are the first two bits of the first byte of visual style data
// Depending on the type, the other 6 bits may be used for additional data
enum class VisualStyleType : uint8
{
  SolidBlock              = 0x00,
  PerBlockNoRotation      = 0x01,
  PerBlockWithRotation    = 0x02,
  PerOrientationAndBlock  = 0x03,

  Count,
};
constexpr uint8 k_visualStyleTypeBits = 2;
constexpr uint8 k_visualStyleTypeMask = (1 << k_visualStyleTypeBits) - 1;
static_assert(int(VisualStyleType::Count) <= (1 << k_visualStyleTypeBits), "VisualStyleType needs to fit in the bits available");


enum class VisualStyle : uint8
{
  Solid,
  Donut,
  CenterDot,
  X,
  O,
  Plus,
  
  Count
};

const char k_styleName0[] PROGMEM = "Solid";
const char k_styleName1[] PROGMEM = "Donut";
const char k_styleName2[] PROGMEM = "Dot";
const char k_styleName3[] PROGMEM = "X";
const char k_styleName4[] PROGMEM = "O";
const char k_styleName5[] PROGMEM = "Plus";

// For accessing an array of strings in program memory, see...
// http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
PGM_P const k_styleNames[] PROGMEM =
{
  k_styleName0,
  k_styleName1,
  k_styleName2,
  k_styleName3,
  k_styleName4,
  k_styleName5,
};
static_assert(countof(k_styleNames) == uint8(VisualStyle::Count), "Make sure data matches the enum");


// Helper for generating solid visual styles
// These are ones that can be described by a single byte
// Bits [0..1] are the VisualStyleType and bits [2..7] are the BlockIndex
constexpr uint8 MakeSolidVisualStyle(VisualStyleType type, BlockIndex block)
{
  static_assert(uint8(BlockIndex::Count) <= (1 << (8 - k_visualStyleTypeBits)), "Verify all BlockIndex entries can fit in the remaining bits");
  return uint8(type) | (uint8(block) << k_visualStyleTypeBits);
}

constexpr uint8 PROGMEM StyleDataSolid[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::SolidWhite)};
constexpr uint8 PROGMEM StyleDataDonut[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::Donut)};
constexpr uint8 PROGMEM StyleDataCenterDot[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::CenterDot)};
constexpr uint8 PROGMEM StyleDataX[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::X)};
constexpr uint8 PROGMEM StyleDataO[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::O)};
constexpr uint8 PROGMEM StyleDataPlus[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::Plus)};

constexpr const uint8* k_visualStyles[] PROGMEM =
{
  StyleDataSolid,
  StyleDataDonut,
  StyleDataCenterDot,
  StyleDataX,
  StyleDataO,
  StyleDataPlus
};
static_assert(countof(k_visualStyles) == uint8(VisualStyle::Count), "Make sure data matches the enum");

/*
enum class BlockIndex : uint8
{
  Empty             = 0x00,   // Same as SolidBlack
  SolidBlack        = 0x00,
  SolidWhite        = 0x01,
  Donut             = 0x02,
  CenterDot         = 0x03,
  X                 = 0x04,
  O                 = 0x05,
  Plus              = 0x06,
  Corners           = 0x07,
  
  TronSquareCapN    = 0x08,
  TronSquareCapE    = 0x09,
  TronSquareCapS    = 0x0A,
  TronSquareCapW    = 0x0B,
  
  TronSquareCornerNW = 0x0C,
  TronSquareCornerNE = 0x0D,
  TronSquareCornerSE = 0x0E,
  TronSquareCornerSW = 0x0F,
  
  TronSquareNS      = 0x10,
  TronSquareEW      = 0x11,
  TronSquareSN      = 0x12,
  TronSquareWE      = 0x13,
  
  TronSquareTN      = 0x14,
  TronSquareTE      = 0x15,
  TronSquareTS      = 0x16,
  TronSquareTW      = 0x17,
  
  TronAngledCapN    = 0x18,
  TronAngledCapE    = 0x19,
  TronAngledCapS    = 0x1A,
  TronAngledCapW    = 0x1B,
  
  TronAngledCornerNW = 0x1C,
  TronAngledCornerNE = 0x1D,
  TronAngledCornerSE = 0x1E,
  TronAngledCornerSW = 0x1F,
  
  SimpleDitherCapN   = 0x20,
  SimpleDitherCapE   = 0x21,
  SimpleDitherCapS   = 0x22,
  SimpleDitherCapW   = 0x23,
  
  SimpleDitherCornerNW = 0x24,
  SimpleDitherCornerNE = 0x25,
  SimpleDitherCornerSE = 0x26,
  SimpleDitherCornerSW = 0x27,

  ShadedDitherLargeCapNW = 0x28,
  ShadedDitherLargeCapNE = 0x29, // Unused
  ShadedDitherLargeCapSE = 0x2A, // Unused
  ShadedDitherLargeCapSW = 0x2B,
  
  ShadedDitherSmallCapNW = SimpleDitherCornerNW,
  ShadedDitherSmallCapNE = SimpleDitherCornerNE,
  ShadedDitherSmallCapSE = SimpleDitherCornerSE,
  ShadedDitherSmallCapSW = SimpleDitherCornerSW,

  ShadedDitherMediumCapNW = 0x2C,
  ShadedDitherMediumCapNE = 0x2D,
  ShadedDitherMediumCapSE = 0x2E, // Unused
  ShadedDitherMediumCapSW = 0x2F,

  Count
};

constexpr uint8 PROGMEM BlockSprites[] =
{
};
*/
