# Embedded System Design with UML State Machines (PlatformIO Projects) - Arduino Uno Rev3

This repository contains all projects from the "[First Embedded System Design using UML State Machines](https://www.udemy.com/course/embedded-system-design-using-uml-state-machines)" Udemy course, implemented on an **Arduino Uno Rev3** using PlatformIO.The `007ClockAlarm` project includes specific enhancements.

## Key Features & Enhancements (007ClockAlarm)

*   **Auto-Increment Button Handling:** Improved button interaction allows values to increment automatically when a button is held down.
*   **I2C LCD:** Uses the `LiquidCrystal_I2C` library for simplified wiring with an I2C-based LCD.
*   **Alarm Set Indicator:** A visual cue on the LCD indicates when the alarm is set.

## Important Notes

*   **Hardware:**  Ensure `platformio.ini` and pin definitions match your Arduino Uno Rev3 and connected components.
*   **Multiple Projects:** Select the active project in PlatformIO before building/uploading.