# Chess-RTOS
> Why spend 20 euros on a game clock when you can build one yourself in slightly less than infinite amount of free time?
> 
> *-- Me, first days of 2024*

This project is an over-engineered chess clock running on Arduino Uno. It's built on top of FreeRTOS (real-time operating system) and requires a 16x2 liquid crystal display.

It is by no means a tournament-worthy timepiece, but works nonetheless.

** (in progress as of 2024-01-11) **

## Table of contents
- [Why?](#why)
- [Hardware](#hardware)
- [Usage](#usage)
    - [Select mode](#select-mode)
    - [Set minutes](#set-minutes)
    - [Set increment](#set-increment)
    - [Ready to start](#ready-to-start)
    - [During the game](#during-the-game)
    - [Game over](#game-over)
- [Installation](#installation)
- [Pictures](#pictures)
- [License](#license)

## Why?
I wanted a game clock, and had a bunch of Arduinos lying around. It was also a good opportunity to try out embedded development with GitHub copilot.

## Parts list
To build this chess clock, you'll need the following hardware components:

| Component                                      | Quantity |
| -----------------------------------------------| -------- |
| Arduino Uno                                    | 1        |
| 16x2 HD44780 liquid crystal display (LCD)      | 1        |
| I2C adapter for LCD                            | 1        |
| Bi-color common anode LED                      | 2        |
| Arcade microswitch                             | 2        |
| Pushbutton                                     | 1        |
| -----------------------------------------------|----------|
| Optional components                            |          |
| -----------------------------------------------|----------|
| Arduino Grove shield                           | 1        |
| Grove 4 pin cables                             | 7        |
| Encoder with pushbutton                        | 1        |

## Usage
### Select mode
Choose from the following modes:

| Mode          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| Sudden Death  | Absolute mode: Game ends when either player runs out of time |
| Fischer       | Increment is added to remaining time at the end of turn      |
| Hourglass     | Opponent's time increases during turn                        |
| Simple Delay  | Delay for n seconds every turn before clock starts running   |

### Set minutes
Set the amount of play time in minutes (1...90).
* Cycle between options with Left and Right buttons
* Press Select to continue to next screen.

### Set increment
Set the amount of time added at the end of turn (Fischer) or delay before clock starts running (Simple Delay).
This screen is not shown in Sudden Death or Hourglass modes.
* Cycle between options with Left and Right buttons
* Press Select to continue to next screen.

### Ready to start
* Press Select to store settings to EEPROM
* Opponent starts the clock.

### During the game
* Select pauses/continues the game
* End turn by pressing your own key.

### Game over
* Press select to return to minute settings.

## Installation
Open the project in [PlatformIO](https://platformio.org), compile and flash to Arduino Uno. You may need to install dependencies first.

## Pictures
TODO

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
