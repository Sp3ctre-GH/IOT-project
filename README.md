<div style="text-align: left;">

# ESP and Flask Buzzer Control Project

This project consists of two main components: an ESP-based firmware for controlling an active buzzer and a Flask backend server to manage the buzzer remotely via API.

## Features
- **ESP Firmware**: Controls an active buzzer (e.g., KY-012 module) using an ESP microcontroller.
- **Flask Backend**: Provides RESTful API endpoints to interact with the buzzer and retrieve data.
- **API Endpoints**:
  - `/data`: Retrieve data in JSON format.
  - `/buzzer`: Turn the buzzer on or off.
  - `/history`: View the history of buzzer actions.
- **Database Support**: Uses SQLite for local testing and PostgreSQL for production.

## Requirements
- **ESP Firmware**:
  - ESP32 or ESP8266 microcontroller
  - Active buzzer (e.g., KY-012)
  - Arduino IDE or PlatformIO for uploading the code
- **Flask Backend**:
  - Python 3.8+
  - Flask, Flask-SQLAlchemy, and other dependencies (see `requirements.txt`)
  - SQLite or PostgreSQL database

## Installation
1. **ESP Firmware**:
   - Clone this repository.
   - Open the `esp` folder in your IDE.
   - Upload the code to your ESP device.
2. **Flask Backend**:
   - Clone this repository.
   - Navigate to the `server` folder.
   - Install dependencies: `pip install -r requirements.txt`.
   - Set up the database (SQLite or PostgreSQL) and update the connection string in the configuration.
   - Run the server: `python main.py`.

## Usage
- **ESP**: After uploading the firmware, the ESP will connect to the specified server and listen for buzzer commands.
- **Flask Server**:
  - Start the server and access the API endpoints.
  - Example: Send a POST request to `/buzzer` to activate the buzzer.

## Project Structure
- `/esp`: Contains the ESP firmware code.
- `/server`: Contains the Flask backend code.
  - `main.py`: Main Flask application.
  - `/routes`: API route definitions.

## Future Improvements
- Add authentication for API endpoints.
- Implement scalability features using Gunicorn and Nginx.
- Expand the history endpoint with pagination.

## License
This project is licensed under the MIT License.

</div>
