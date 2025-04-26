# Arduino LED Strip Pong Game (2-Player)

This project is a **2-player Pong-style game** for Arduino, using a long WS2812B/NeoPixel LED strip and two physical push-buttons (one for each player).  
It is based on the code by FlyingAngel, with further modifications and improved effects.

## Features

- **WS2812B (NeoPixel) LED Strip**: Main display with up to 106+ individually-addressable LEDs.
- **2 Players, 2 Buttons**: Each player has a button and an indicator LED.
- **LED Endzones**: Color-coded areas at each end of the strip represent each player’s "goal".
- **Dynamic Ball**: A "ball" travels back and forth along the strip, bouncing between players.
- **Change Direction**: Players must hit their button as the ball reaches their endzone to send it back.
- **Variable Ball Speed**: Ball speeds up with fast returns and based on where it’s hit.
- **Score Display**: Each player's score is shown as colored bars near the center of the strip.
- **Rainbow Win Animation**: Fun effects for the winner.
- **Brightness Adjustment**: One button changes LED brightness in the menu.
- **Game Menu**: Select brightness or start game with the two buttons.
- **No LCD Required**: All status and effects are displayed with LEDs.
- **Fully Self-Contained**: Just an Arduino, the LED strip, and two buttons.

## Hardware Requirements

- **Arduino UNO, Nano, Mega** or compatible board
- **WS2812B/NeoPixel LED Strip** (tested with 106 LEDs; adjust `NUM_LEDS` in code as needed)
- **2 Push Buttons** (connected to pins 5 and 8 with pull-ups)
- **2 Indicator LEDs** (pins 6 and 9; built into illuminated arcade buttons or use standalone)
- **5V Power Supply** (for LED strip; external supply recommended for >30 LEDs)
- **Jumper wires, breadboard, or PCB**

## Pinout

| Function         | Arduino Pin |
|------------------|:-----------|
| NeoPixel Data    | 3          |
| Player 1 Button  | 5          |
| Player 2 Button  | 8          |
| Player 1 LED     | 6          |
| Player 2 LED     | 9          |

*Adjust `playerBtnPin[]` and `playerLedPin[]` in code for different wiring.*

## Libraries Needed

- [FastLED](https://github.com/FastLED/FastLED)

Install via the Arduino Library Manager.

## How to Play

1. **Power on:** Both button LEDs are ON, and the LED strip shows the playfield.
2. **Adjust brightness:** Player 1 button cycles LED brightness in the menu.
3. **Start game:** Player 2 button starts the game.
4. **Game loop:**
    - A white "ball" travels along the LED strip toward one player.
    - When it reaches your end, press your button as the ball enters your colored endzone to "return" it.
    - The earlier (farther from the end) you hit, the more the speed increases!
    - Miss or press too late, and your opponent scores a point.
    - The center LEDs show the score (left half = player 0, right half = player 1).
5. **Win the game:** First to reach the set number of points (default 10) wins and gets a rainbow animation!

## Game Parameters

- **Brightness**: Adjustable in menu (Player 1 button).
- **Endzone size/color**: Adjustable in code.
- **Ball speed**: Increases with quick/early returns.
- **Winning score**: Default is 10 (set with `winRounds`).

## Customization

- Change `NUM_LEDS` for your strip.
- Adjust colors, speeds, and win conditions in the code.
- Use illuminated arcade buttons or separate LEDs for player indicators.

## Credits

- Original code: FlyingAngel (April 2020)

## License

This project is open source. See the `LICENSE` file for details.

---

Enjoy competitive Pong with just an Arduino and an LED strip!
