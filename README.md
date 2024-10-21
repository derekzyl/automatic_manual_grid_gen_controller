# Automatic, Manual, and Semi-Automatic Grid Switch Control System

## Project Overview

This project is a smart control system designed to manage power sources (grid and generator) and loads efficiently in an automated, semi-automated, and manual mode. The system prioritizes using grid power when available, monitors the state of the generator and loads, and ensures proper switching between grid and generator supply while safeguarding against load failure.

## Key Features

1. **Automatic Mode**:  
   - Automatically checks the availability of grid power.
   - If grid power is available, the generator is turned off.
   - If the generator is on, the system checks if the load is properly connected.
   - In case the load fails (e.g., disconnection or fault), an alarm is triggered, and the generator is turned off to prevent damage or unnecessary fuel consumption.
   - If grid power is unavailable, the system switches to generator mode, ensuring continuous supply.

2. **Manual Mode**:  
   - The user manually controls the switching between the grid and generator.
   - No automatic monitoring occurs in this mode, allowing the user to have complete control over power management.

3. **Semi-Automatic Mode**:  
   - Combines features from both automatic and manual modes.
   - The system assists with switching but allows user intervention where necessary.

## System Workflow

### Automatic Mode

1. **Grid Power Available**:
   - The system detects the presence of grid power.
   - The generator is turned off if it was previously running.
   - The load operates on grid power.

2. **Grid Power Unavailable**:
   - The system checks if the generator is running.
   - If the generator is off, it starts the generator to provide power.
   - The load is monitored. If the load is connected and operating, the system continues running.
   - If the load fails, the system triggers an alarm and shuts off the generator.

### Manual Mode

- The user manually switches between grid and generator as per their preference.
- No automation occurs—full control is given to the user to decide when to use grid or generator power.

### Semi-Automatic Mode

- The system will prompt the user for input if it detects certain conditions, like switching between grid and generator.
- The user can then confirm or deny the system’s suggestions.

## Components

- **Microcontroller (e.g., ESP32, Arduino)**: For managing the logic of the system.
- **Relays**: For switching between the grid, generator, and load.
- **Current/Voltage Sensors**: To detect grid status, generator state, and load conditions.
- **Alarms/Buzzer**: To signal load failure or other issues.
- **User Interface**: Button/switch for manual control and status indicators for monitoring.

## Installation

1. Set up the microcontroller, sensors, and relays according to the wiring diagram provided in the `schematics/` folder.
2. Upload the control logic code to the microcontroller.
3. Ensure the grid, generator, and load connections are correctly wired to the relays.
4. Test the system in **manual mode** first to verify the relays switch between grid and generator.
5. Enable **automatic mode** to test the system's ability to switch between grid and generator automatically.

## Usage Instructions

1. **Automatic Mode**:  
   - Switch the system to automatic mode using the mode selector switch.
   - The system will automatically handle power switching based on grid availability and load status.
  
2. **Manual Mode**:  
   - Switch the system to manual mode.
   - Manually turn on or off the generator or switch to grid power as required using the provided buttons/switches.

3. **Semi-Automatic Mode**:  
   - Switch to semi-automatic mode.
   - The system will assist with switching and prompt for manual confirmation when necessary.

## Troubleshooting

- **System not switching to generator when grid is off**:  
  Ensure the generator is functional and connected correctly.
  
- **Alarm triggered even though load is connected**:  
  Check the wiring between the load and the monitoring circuit to ensure proper connections.

- **Generator not turning off when grid is restored**:  
  Ensure that the voltage sensor is properly detecting grid power.


4. Open a Pull Request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contact

For any inquiries, suggestions, or support, feel free to reach out via [email](derekzyl@gmail.com).

