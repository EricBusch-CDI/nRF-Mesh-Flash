# nRF-Tool
The nRF-Tool is a c++ version of nrf.py

## Features

- Build, merge and flash projects using the corresponding commands
- Configure sdk_config.h through cmsis GUI
- Configurable through the build_flash_config.json


## Issues
- Tool is not very modular
    - should be split up into multiple files/classes
- std::setw() does not work with mingw? So the help if not formatted well