import serial
import serial.tools.list_ports

def check_esp32_connection(port='COM5', baudrate=115200):
    try:
        # Lista todos los puertos disponibles
        ports = [p.device for p in serial.tools.list_ports.comports()]
        if port not in ports:
            print(f"El puerto {port} no está disponible.")
            print("Puertos disponibles:", ports)
            return

        # Intenta abrir el puerto
        with serial.Serial(port, baudrate, timeout=1) as ser:
            if ser.is_open:
                print(f"Conexión exitosa al ESP32 en el puerto {port} a {baudrate} baudios.")
            else:
                print(f"No se pudo abrir el puerto {port}.")
                
    except serial.SerialException as e:
        print(f"Error al conectar al ESP32 en el puerto {port}: {e}")

# Llama a la función para validar la conexión
check_esp32_connection(port='COM5', baudrate=115200)
