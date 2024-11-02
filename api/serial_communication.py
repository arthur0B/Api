import serial
import json
import re

class SerialCommunication:
    def __init__(self, port='COM5', baudrate=115200, timeout=1):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.connection = None

    def connect(self):
        try:
            self.connection = serial.Serial(self.port, self.baudrate, timeout=self.timeout)
            if self.connection.is_open:
                print(f"Conectado al ESP32 en {self.port} a {self.baudrate} baudios")
        except Exception as e:
            print(f"Error al conectar: {e}")

    def disconnect(self):
        if self.connection and self.connection.is_open:
            self.connection.close()
            print("Conexión cerrada")

    def send_data(self, data):
        if self.connection and self.connection.is_open:
            # Encode JSON string and send via serial
            self.connection.write(data.encode('utf-8'))
            print("Datos enviados:", data)
        else:
            print("Conexión no establecida. No se pueden enviar datos.")

    def receive_data(self):
        if self.connection and self.connection.is_open:
            if self.connection.in_waiting > 0:
                raw_data = self.connection.readline().decode('utf-8').strip()
                print("Datos recibidos:", raw_data)
                
                # Use regex to find JSON part of the received message
                json_match = re.search(r'(\{.*\})', raw_data)
                
                if json_match:
                    json_data = json_match.group(1)  # Extract JSON string
                    try:
                        data = json.loads(json_data)
                        return data  # Returns dictionary
                    except json.JSONDecodeError:
                        print("Error al decodificar los datos JSON")
                else:
                    print("No se encontró JSON en los datos recibidos")
        else:
            print("Conexión no establecida. No se pueden recibir datos.")
        return None
