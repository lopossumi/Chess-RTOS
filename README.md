# Chess-RTOS
> Why spend 20 euros on a game clock when you can build one yourself in slightly less than infinite amount of free time?
> 
> *-- Me, first days of 2024*

This project is an over-engineered chess clock running on Arduino Uno. It's built on top of FreeRTOS (real-time operating system).

It is by no means a tournament-worthy timepiece, but works nonetheless for casual gameplay.

![Photo of the finished chess clock](images/white_to_play.jpeg?raw=true "White to play.")

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

### Why FreeRTOS?
Real-time operating system (RTOS) allows for better task scheduling, synchronization, and resource management. In this project, separate threads are used for I/O, display and game loop.

Although using an RTOS made the development process easier, Arduino Uno's limited 2kB RAM presented challenges in terms of the number of threads and stack size.

## Parts list
To build this chess clock, you'll need the following hardware components:

| Component                                            | Quantity |
| -----------------------------------------------------| -------- |
| Arduino Uno                                          | 1        |
| 16x2 HD44780 liquid crystal display with I2C adapter | 1        |
| Bi-color common anode LED                            | 2        |
| Arcade microswitch                                   | 2        |
| Encoder with pushbutton                              | 1        |
| Piezo buzzer                                         | 1        |
|                                                      |          |
| **Optional components:**                             |          |
| *Arduino Grove shield*                               | 1        |
| *Grove 4 pin cables*                                 | n        |

## Usage
### Select mode
Choose from the following modes:

| Mode          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| Sudden Death  | Absolute mode: Game ends when either player runs out of time |
| Fischer       | Increment is added to remaining time at the end of turn      |
| Hourglass     | Opponent's time increases during turn                        |
| Simple Delay  | Delay for n seconds every turn before clock starts running   |

* Cycle between options by turning the encoder or pressing black and white buttons
* Press Select to continue to next screen.

### Set minutes
Set the amount of play time in minutes (1...90).
* Cycle between options by turning the encoder or pressing black and white buttons
* Press Select to continue to next screen.

### Set increment
Set the amount of time added at the end of turn (Fischer) or delay before clock starts running (Simple Delay).
This screen is not shown in Sudden Death or Hourglass modes.
* Cycle between options by turning the encoder or pressing black and white buttons
* Press Select to continue to next screen.

### Ready to start
* Press Select to store settings to EEPROM
* Opponent starts the clock.

### During the game
* Select pauses/continues the game
* Current player is shown by the green LEDs
* End turn by pressing your own key.

### Game over
* Press Select to return to minute settings.

## Installation
Open the project in [PlatformIO](https://platformio.org), compile and flash to Arduino Uno. You may need to install dependencies first.

# Hardware

## Wiring

Todo. It's not hard to figure out for yourself though.

## Enclosure

The enclosure was made from 3mm plywood using a laser cutter. Some glue was needed for assembly, as I did not account for material loss.
![Enclosure in vector format](images/chess_clock_enclosure.svg?raw=true "Enclosure in vector format for the laser cutter.")

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
