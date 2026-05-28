# Juice Dispenser Project

An Arduino-based automated juice dispensing system with a keypad interface and LCD display. Users can input the desired amount and the system dispenses the exact quantity of juice.

## Features

- **Keypad Input**: 4x4 matrix keypad for user input (0-99 Rs)
- **LCD Display**: 16x2 I2C LCD display for user interface
- **Flow Sensor**: Precise volume measurement using a flow rate sensor
- **Relay Control**: Automatic pump control via relay module
- **Calibration**: Adjustable calibration factor for accurate dispensing
- **Safety**: Input validation and range checking (1-100 Rs)

## Hardware Requirements

- Arduino Microcontroller
- 16x2 I2C LCD Display (Address: 0x27)
- 4x4 Matrix Keypad
- Flow Sensor (attached to pin 2)
- Relay Module (connected to pin 10)
- Pump/Motor with appropriate power supply
- Connecting wires and breadboard

## Pin Configuration

| Component | Pin |
|-----------|-----|
| Relay | 10 |
| Flow Sensor | 2 (Interrupt) |
| LCD SDA | A4 |
| LCD SCL | A5 |
| Keypad Row 1 | 9 |
| Keypad Row 2 | 8 |
| Keypad Row 3 | 7 |
| Keypad Row 4 | 6 |
| Keypad Col 1 | 5 |
| Keypad Col 2 | 4 |
| Keypad Col 3 | 3 |
| Keypad Col 4 | 12 |

## Dependencies

- `Wire.h` - I2C communication
- `LiquidCrystal_I2C.h` - LCD control
- `Keypad.h` - Keypad library

## Installation

1. Install the required libraries in Arduino IDE:
   - LiquidCrystal_I2C
   - Keypad

2. Upload `main.ino` to your Arduino board

3. Adjust the `calibrationFactor` variable if needed (default: 4.5)

## Usage

1. Power on the system
2. LCD displays "Enter Amount:"
3. Use the keypad to enter the desired amount (1-100 Rs)
4. Press `#` to dispense
5. Press `*` to clear and start over

### Pricing Logic

- 1 Rs = 50 mL (minimum order)
- 20 Rs = 1000 mL (maximum order)
- Proportional scaling for amounts in between

## Code Structure

- **Setup**: Initializes pins, LCD, keypad, and interrupt handler
- **Loop**: Continuously monitors keypad input
- **Keypad Handling**:
  - `0-9`: Add digits to input (max 3 digits)
  - `#`: Validate and dispense
  - `*`: Clear input
- **Flow Measurement**: Uses interrupt-based pulse counting for accurate volume measurement
- **Calibration**: Adjustable factor to fine-tune liquid dispensed

## Calibration

If the dispensed amount is incorrect:
1. Open `main.ino`
2. Locate: `float calibrationFactor = 4.5;`
3. Adjust:
   - Increase value → Less liquid dispensed
   - Decrease value → More liquid dispensed
4. Test and re-upload

## License

This project is open source and available under the MIT License.

## Author

AI Expert Academy

## Support

For issues or questions, please open an issue on the GitHub repository.
