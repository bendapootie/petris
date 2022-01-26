- [ ] Move appropriate data into PROGMEM ([documentation link](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/)) to free up dynamic memory
- [ ] Initialization of `RotationOffsets` seems to take a fair amount of code. Global variable usage seems as expected.
	- [ ] Data for RotationOffsets takes 32-bytes per piece.
		- More efficient encoding could reduce total memory usage from 192 bytes (32 bytes * 6 pieces) to 120 bytes (20 bytes * 6 pieces)
		- See notes in [Super Rotation System](../../docs/Super%20Rotation%20System.md)
	- [ ] 💡 Move this data to PROGMEM and only bring the bits into ram that are needed
- [ ] `RotationFormula` has only two values and could be represented with one bit.
- [ ] Make m_cwButtonWasDown and m_ccwButtonWasDown only take 1-bit
	- [ ] Look at anything that's a boolean and consider adding ` 1` to the end (ie. `bool isSet : 1;`)