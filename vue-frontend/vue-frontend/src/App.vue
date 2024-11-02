<!-- src/App.vue -->
<template>
  <div id="app">
    <SensorTable />
  </div>
</template>

<script>
import SensorTable from "./components/SensorTable.vue";

export default {
  components: {
    SensorTable,
  },
};
</script>
