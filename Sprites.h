
constexpr uint8 ___ = 0x00;
constexpr uint8 __O = 0x01;
constexpr uint8 _O_ = 0x02;
constexpr uint8 _OO = 0x03;
constexpr uint8 O__ = 0x04;
constexpr uint8 O_O = 0x05;
constexpr uint8 OO_ = 0x06;
constexpr uint8 OOO = 0x07;

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

  LineCapN = 0x30,
  LineCapE = 0x31,
  LineCapS = 0x32,
  LineCapW = 0x33,

  LineCornerNE = 0x34,
  LineCornerSE = 0x35,
  LineCornerSW = 0x36,
  LineCornerNW = 0x37,

  LineTeeN = 0x38,
  LineTeeE = 0x39,
  LineTeeS = 0x3A,
  LineTeeW = 0x3B,

  LineStraightNS = 0x3C,
  LineStraightEW = 0x3D,
  LineStraightSN = 0x3E,
  LineStraightWE = 0x3F,

  Count
};

constexpr uint8 PROGMEM BlockSprites[] =
{
  // width, height,
  3, 8,
  
  // [0] BlockSprite::SolidBlack
  ___,
  ___,
  ___,
  
  // [1] BlockSprite::SolidWhite
  OOO,
  OOO,
  OOO,

  // [2] BlockSprite::Donut
  OOO,
  O_O,
  OOO,
  
  // [3] BlockIndex::CenterDot
  ___,
  _O_,
  ___,
  
  // [4] BlockSprite::X
  O_O,
  _O_,  // 'x' dither
  O_O,

  // [5] BlockSprite::O
  _O_,
  O_O,  // 'o' dither
  _O_,

  // [6] BlockSprite::Plus
  _O_,
  OOO,
  _O_,

  // [7] BlockSprite::Corners
  O_O,
  ___,
  O_O,


  // [0x08] BlockSprite::TronSquareCapN
  OOO,  // North ->
  __O,  // North cap
  OOO,

  // [0x09] BlockSprite::TronSquareCapE
  O_O,
  O_O,  // East cap
  OOO,

  // [0x0A] BlockSprite::TronSquareCapS
  OOO,
  O__,  // South cap
  OOO,

  // [0x0B] BlockSprite::TronSquareCapW
  OOO,
  O_O,  // West cap
  O_O,
  

  // [0x0C] BlockSprite::TronSquareCornerNW
  OOO,
  __O,  // NW corner
  O_O,

  // [0x0D] BlockSprite::TronSquareCornerNE
  O_O,
  __O,  // NE corner
  OOO,

  // [0x0E] BlockSprite::TronSquareCornerSE
  O_O,
  O__,  // SE corner
  OOO,

  // [0x0F] BlockSprite::TronSquareCornerSW
  OOO,
  O__,  // SW corner
  O_O,


  // [0x10] BlockSprite::TronSquareNS
  OOO,
  ___,  // N-S block
  OOO,

  // [0x11] BlockSprite::TronSquareEW
  O_O,
  O_O,  // E-W block
  O_O,

  // [0x12] BlockSprite::TronSquareSN
  OOO,
  ___,  // Same as N-S block, but here to simplify rotation code
  OOO,

  // [0x13] BlockSprite::TronSquareWE
  O_O,
  O_O,  // Same as E-W block, but here to simplify rotation code
  O_O,


  // [0x14] BlockSprite::TronSquareTN
  O_O,
  O__,  // 'T' facing North
  O_O,

  // [0x15] BlockSprite::TronSquareTE
  OOO,
  ___,  // 'T' facing East
  O_O,

  // [0x16] BlockSprite::TronSquareTS
  O_O,
  __O,  // 'T' facing South
  O_O,

  // [0x17] BlockSprite::TronSquareTW
  O_O,
  ___,  // 'T' facing West
  OOO,


  // [0x18] BlockSprite::TronAngledCapN
  OO_,  // North ->
  __O,  // North cap
  OOO,

  // [0x19] BlockSprite::TronAngledCapE
  O_O,
  O_O,  // East cap
  _OO,

  // [0x1A] BlockSprite::TronAngledCapS
  OOO,
  O__,  // South cap
  _OO,

  // [0x1B] BlockSprite::TronAngledCapW
  OO_,
  O_O,  // West cap
  O_O,


  // [0x1C] BlockSprite::TronAngledCornerNW
  OO_,
  __O,  // NW corner
  O_O,

  // [0x1D] BlockSprite::TronAngledCornerNE
  O_O,
  __O,  // NE corner; Same as TronSquareCornerNE
  OOO,

  // [0x1E] BlockSprite::TronAngledCornerSE
  O_O,
  O__,  // SE corner
  _OO,

  // [0x1F] BlockSprite::TronAngledCornerSW
  OOO,
  O__,  // SW corner; Same as TronSquareCornerSW
  O_O,


  // [0x20] BlockSprite::SimpleDitherCapN
  _OO,
  O_O,  // North cap
  _OO,

  // [0x21] BlockSprite::SimpleDitherCapE
  _O_,
  O_O,  // East cap
  OOO,

  // [0x22] BlockSprite::SimpleDitherCapS
  OO_,
  O_O,  // South cap
  OO_,

  // [0x23] BlockSprite::SimpleDitherCapW
  OOO,
  O_O,  // West cap
  _O_,


  // [0x24] BlockSprite::SimpleDitherCornerNW
  _OO,
  O_O,  // NW corner
  _O_,

  // [0x25] BlockSprite::SimpleDitherCornerNE
  _O_,
  O_O,  // NE corner
  _OO,

  // [0x26] BlockSprite::SimpleDitherCornerSE
  _O_,
  O_O,  // SE corner
  OO_,

  // [0x27] BlockSprite::SimpleDitherCornerSW
  OO_,
  O_O,  // SW corner
  _O_,


  // [0x28] BlockSprite::ShadedDitherLargeCapNW
  OOO,
  O_O,  // NW large cap
  _OO,

  // [0x29] BlockSprite::ShadedDitherLargeCapNE
  _OO,
  O_O,  // NE large cap; Unused
  OOO,

  // [0x2A] BlockSprite::ShadedDitherLargeCapSE
  OO_,
  O_O,  // SE large cap; Unused
  OOO,

  // [0x2B] BlockSprite::ShadedDitherLargeCapSW
  OOO,
  O_O,  // SW large cap
  OO_,


  // Alias for the same blocks
  // [0x24] : BlockSprite::ShadedDitherSmallCapNW = SimpleDitherCornerNW
  // [0x25] : BlockSprite::ShadedDitherSmallCapNE = SimpleDitherCornerNE
  // [0x26] : BlockSprite::ShadedDitherSmallCapSE = SimpleDitherCornerSE
  // [0x27] : BlockSprite::ShadedDitherSmallCapSW = SimpleDitherCornerSW


  // [0x2C] BlockSprite::ShadedDitherMediumCapNW
  OOO,
  _OO,  // NW medium cap
  O_O,

  // [0x2D] BlockSprite::ShadedDitherMediumCapNE
  O_O,
  _OO,  // NE medium cap
  OOO,

  // [0x2E] BlockSprite::ShadedDitherMediumCapSE
  O_O,
  OO_,  // SE medium cap; Unused
  OOO,

  // [0x2F] BlockSprite::ShadedDitherMediumCapSW
  OOO,
  OO_,  // SW medium cap
  O_O,

  // LineCapN = 0x30,
  ___,
  OO_,
  ___,
  
  // LineCapE = 0x31,
  _O_,
  _O_,
  ___,

  // LineCapS = 0x32,
  ___,
  _OO,
  ___,
  
  // LineCapW = 0x33,
  ___,
  _O_,
  _O_,

  // LineCornerNE = 0x34,
  _O_,
  OO_,
  ___,
  
  // LineCornerSE = 0x35,
  _O_,
  _OO,
  ___,
  
  // LineCornerSW = 0x36,
  ___,
  _OO,
  _O_,

  // LineCornerNW = 0x37,
  ___,
  OO_,
  _O_,

  // LineTeeN = 0x38,
  _O_,
  _OO,
  _O_,
  
  // LineTeeE = 0x39,
  ___,
  OOO,
  _O_,
  
  // LineTeeS = 0x3A,
  _O_,
  OO_,
  _O_,
  
  // LineTeeW = 0x3B,
  _O_,
  OOO,
  ___,


  // LineStraightNS = 0x3C,
  ___,
  OOO,
  ___,
  
  // LineStraightEW = 0x3D,
  _O_,
  _O_,
  _O_,
  
  // LineStraightSN = 0x3E,
  ___,
  OOO,
  ___,
  
  // LineStraightWE = 0x3F,
  _O_,
  _O_,
  _O_,

};

static_assert(sizeof(BlockSprites) == int(BlockIndex::Count) * BlockSprites[0] * (BlockSprites[1] / 8) + 2,
  "Sanity check to make sure BlockIndex enum and BlockSprites array are the same size");
