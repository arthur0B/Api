<template>
  <div>
    <h2>ESP32 Sensor Dashboard</h2>
    <button @click="connectToApi">Conectar</button>
    <button @click="disconnectFromApi" :disabled="!isConnected">Desconectar</button>
    <button @click="toggleAutoUpdate" :disabled="!isConnected">
      {{ isAutoUpdating ? "Detener Actualización" : "Reanudar Actualización" }}
    </button>

    <div v-if="isConnected">
      <table border="1">
        <thead>
          <tr>
            <th>Configuración / Sensor</th>
            <th>Valor</th>
          </tr>
        </thead>
        <tbody>
          <!-- Muestra los valores de configuración -->
          <tr v-for="(value, key) in sensorData.CONFIGURACION" :key="'config_' + key">
            <td>{{ key }}</td>
            <td @dblclick="enableEditing(key, 'CONFIGURACION')">
              <span v-if="!editMode.config[key]">{{ value }}</span>
              <input
                v-else
                v-model="editedValues.config[key]"
                @keyup.enter="saveEdit(key, 'CONFIGURACION')"
              />
            </td>
          </tr>
          <!-- Muestra los valores de sensores -->
          <tr v-for="(value, key) in sensorData.VALORES" :key="'sensor_' + key">
            <td>{{ key }}</td>
            <td @dblclick="enableEditing(key, 'VALORES')">
              <span v-if="!editMode.sensor[key]">{{ value }}</span>
              <input
                v-else
                v-model="editedValues.sensor[key]"
                @keyup.enter="saveEdit(key, 'VALORES')"
              />
            </td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data() {
    return {
      sensorData: {
        CONFIGURACION: {},
        VALORES: {}
      },
      editMode: {
        config: {},
        sensor: {}
      },
      editedValues: {
        config: {},
        sensor: {}
      },
      isConnected: false,
      isAutoUpdating: false,
      updateInterval: null
    };
  },
  methods: {
    fetchSensorData() {
      axios.get('http://localhost:5000/receive_data')
        .then(response => {
          if (response.status === 200) {
            this.sensorData = response.data;
          } else {
            console.log("No data received.");
          }
        })
        .catch(error => {
          console.error("Error fetching data:", error);
        });
    },
    enableEditing(key, type) {
      if (this.sensorData[type] && key in this.sensorData[type]) {
        if (type === 'CONFIGURACION') {
          this.editMode.config[key] = true;
          this.editedValues.config[key] = this.sensorData.CONFIGURACION[key];
        } else if (type === 'VALORES') {
          this.editMode.sensor[key] = true;
          this.editedValues.sensor[key] = this.sensorData.VALORES[key];
        }
      } else {
        console.error(`La clave '${key}' no existe en '${type}'`);
      }
    },
    saveEdit(key, type) {
      if (type === 'CONFIGURACION') {
        this.sensorData.CONFIGURACION[key] = this.editedValues.config[key];
        this.editMode.config[key] = false;
      } else if (type === 'VALORES') {
        this.sensorData.VALORES[key] = this.editedValues.sensor[key];
        this.editMode.sensor[key] = false;
      }

      // Envía los datos actualizados al ESP32
      const updatedData = { CONFIGURACION: this.sensorData.CONFIGURACION, VALORES: this.sensorData.VALORES };
      axios.post('http://localhost:5000/send_data', updatedData)
        .then(response => {
          console.log("Datos enviados al ESP32:", response.data.status);
        })
        .catch(error => {
          console.error("Error al enviar datos al ESP32:", error);
        });
    },
    connectToApi() {
      axios.get('http://localhost:5000/connect')
        .then(response => {
          this.isConnected = true;
          this.startAutoUpdate();
          console.log(response.data.status);
        })
        .catch(error => {
          console.error("Error connecting to API:", error);
        });
    },
    disconnectFromApi() {
      axios.get('http://localhost:5000/disconnect')
        .then(response => {
          this.isConnected = false;
          this.stopAutoUpdate();
          this.sensorData = { CONFIGURACION: {}, VALORES: {} };  // Limpiar datos al desconectar
          console.log(response.data.status);
        })
        .catch(error => {
          console.error("Error disconnecting from API:", error);
        });
    },
    startAutoUpdate() {
      this.isAutoUpdating = true;
      this.updateInterval = setInterval(this.fetchSensorData, 5000);  // Actualizar cada 5 segundos
    },
    stopAutoUpdate() {
      this.isAutoUpdating = false;
      if (this.updateInterval) {
        clearInterval(this.updateInterval);
        this.updateInterval = null;
      }
    },
    toggleAutoUpdate() {
      if (this.isAutoUpdating) {
        this.stopAutoUpdate();
      } else {
        this.startAutoUpdate();
      }
    }
  },
  beforeUnmount() {
    this.stopAutoUpdate();
  }
};
</script>

<style scoped>
table {
  width: 100%;
  border-collapse: collapse;
}

th, td {
  padding: 8px;
  text-align: left;
}

input {
  width: 100%;
}
</style>
