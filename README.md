# ESP32-C3 Project

This project is designed for the ESP32-C3 microcontroller using the ESP-IDF framework. It includes a custom component and demonstrates how to set up and build an application for the ESP32-C3.

## Project Structure

```
esp32-c3-project
├── components
│   └── custom_component
│       ├── CMakeLists.txt
│       └── custom_component.c
├── main
│   ├── CMakeLists.txt
│   └── main.c
├── CMakeLists.txt
├── sdkconfig
├── sdkconfig.defaults
└── README.md
```

## Getting Started

### Prerequisites

- Install the ESP-IDF framework. Follow the official installation guide for your operating system.
- Ensure you have the necessary tools and dependencies installed.

### Building the Project

1. Navigate to the project directory:
   ```
   cd esp32-c3-project
   ```

2. Configure the project:
   ```
   idf.py menuconfig
   ```

3. Build the project:
   ```
   idf.py build
   ```

### Flashing the Project

To flash the project onto your ESP32-C3 device, connect the device via USB and run:
```
idf.py -p (PORT) flash
```
Replace `(PORT)` with the appropriate port for your device.

### Monitoring the Output

To monitor the output from the ESP32-C3, use:
```
idf.py -p (PORT) monitor
```

### Custom Component

The project includes a custom component located in the `components/custom_component` directory. This component can be modified to add specific functionality as needed.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.