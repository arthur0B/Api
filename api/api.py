from flask import Flask, jsonify, request
from flask_cors import CORS  # Importa CORS
from serial_communication import SerialCommunication
import json

app = Flask(__name__)
CORS(app)  # Habilita CORS para todos los endpoints

# Inicializa la comunicación serial con el ESP32
esp32 = SerialCommunication(port='COM5', baudrate=115200)

@app.route('/connect', methods=['GET'])
def connect():
    esp32.connect()
    return jsonify({"status": "Conexión establecida con el ESP32"})

@app.route('/disconnect', methods=['GET'])
def disconnect():
    esp32.disconnect()
    return jsonify({"status": "Conexión cerrada con el ESP32"})

@app.route('/send_data', methods=['POST'])
def send_data():
    try:
        data = request.get_json()  # Expecting JSON input for sensor data
        esp32.send_data(json.dumps(data))  # Serializes data to JSON string
        return jsonify({"status": "Datos enviados al ESP32"}), 200
    except Exception as e:
        return jsonify({"status": "Error al enviar datos", "error": str(e)}), 500

@app.route('/receive_data', methods=['GET'])
def receive_data():
    try:
        data = esp32.receive_data()
        if data:
            return jsonify(data), 200  # Directly jsonify without further JSON decoding
        else:
            return jsonify({"status": "No data received"}), 204
    except Exception as e:
        return jsonify({"status": "Error al recibir datos", "error": str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
