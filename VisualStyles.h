
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
  SolidBlack,
  SolidWhite,
  Donut,
  CenterDot,
  X,
  O,
  Plus,
  
  Count,
};

const char k_styleName0[] PROGMEM = "SolidBlack";
const char k_styleName1[] PROGMEM = "SolidWhite";
const char k_styleName2[] PROGMEM = "Donut";
const char k_styleName3[] PROGMEM = "Dot";
const char k_styleName4[] PROGMEM = "X";
const char k_styleName5[] PROGMEM = "O";
const char k_styleName6[] PROGMEM = "Plus";

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
  k_styleName6,
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

// Unpacks values that were packed in MakeSolidVisualStyle
// 'outSolidBlockIndex' will only have valid data in it if VisualStyleType is SolidBlock
VisualStyleType GetVisualStyleTypeFromFirstByte(uint8 firstByte, BlockIndex& outSolidBlockIndex)
{
  // Sign-extention shouldn't happen here since firstByte is an unsigned value
  outSolidBlockIndex = BlockIndex(firstByte >> k_visualStyleTypeBits);
  VisualStyleType type = VisualStyleType(firstByte & k_visualStyleTypeMask);
  return type;
}


constexpr uint8 PROGMEM StyleDataSolidBlack[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::SolidBlack)};
constexpr uint8 PROGMEM StyleDataSolidWhite[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::SolidWhite)};
constexpr uint8 PROGMEM StyleDataDonut[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::Donut)};
constexpr uint8 PROGMEM StyleDataCenterDot[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::CenterDot)};
constexpr uint8 PROGMEM StyleDataX[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::X)};
constexpr uint8 PROGMEM StyleDataO[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::O)};
constexpr uint8 PROGMEM StyleDataPlus[] = {MakeSolidVisualStyle(VisualStyleType::SolidBlock, BlockIndex::Plus)};

constexpr const uint8* k_visualStyles[] PROGMEM =
{
  StyleDataSolidBlack,
  StyleDataSolidWhite,
  StyleDataDonut,
  StyleDataCenterDot,
  StyleDataX,
  StyleDataO,
  StyleDataPlus
};
static_assert(countof(k_visualStyles) == uint8(VisualStyle::Count), "Make sure data matches the enum");

class VisualStyleHelper
{
public:
  VisualStyleHelper(VisualStyle visualStyle) : m_visualStyle(visualStyle) {}

  BlockIndex GetBlockForPiece(PieceIndex pieceIndex, PieceOrientation orientation, uint8 index) const;
  
private:
  VisualStyle m_visualStyle;
};

BlockIndex VisualStyleHelper::GetBlockForPiece(PieceIndex pieceIndex, PieceOrientation orientation, uint8 index) const
{
  uint8* pgm_styleData = pgm_read_word(&k_visualStyles[uint8(m_visualStyle)]);
  uint8 firstByte = pgm_read_byte(&pgm_styleData[0]);

  BlockIndex blockIndex;
  VisualStyleType styleType = GetVisualStyleTypeFromFirstByte(firstByte, blockIndex);

  if (styleType == VisualStyleType::SolidBlock)
  {
    // nothing else to do; blockIndex was already set by call to GetVisualStyleTypeFromFirstByte
  }
  else
  {
    Assert(false, F("VisualStyleType not implemented!"));
  }
  
  return blockIndex;
}
