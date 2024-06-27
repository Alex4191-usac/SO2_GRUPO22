import React, { useEffect, useState } from 'react'
import "chart.js/auto"
import axios from 'axios'
import { Chart } from "react-chartjs-2"


export default function GraficaPie() {

  const [procesosMemoria, setProcesosMemoria] = useState([]);

  useEffect(() => {

    const fetchData = () => {
      axios.get(`http://127.0.0.1:3001/kernel/getByProceso`)
        .then(response => {
          const datosProcesos = response.data;
          // Resto del código para procesar los datos y actualizar el estado

          //const memoriaTotalOcupada = datosProcesos.reduce((total, proceso) => total + parseFloat(proceso.total_tamanio), 0);

          const Top10Process = datosProcesos.slice(0, 10);
          const Top10ProcessesSize = Top10Process.reduce((totalMemory, currentProcess) => totalMemory + parseFloat(currentProcess.total_tamanio), 0);
          Top10Process.forEach(currentProcess => {
            currentProcess.total_tamanio = (currentProcess.total_tamanio / 4096) * 100;
          });

          const OtherProcesses = datosProcesos.slice(10);
          const OtherProcessesSize = OtherProcesses.reduce((totalMemory, currentProcess) => totalMemory + parseFloat(currentProcess.total_tamanio), 0);
          const OtherMemory = {
            pid: -1,
            nombre: "Memoria Otros",
            total_tamanio: OtherProcessesSize
          }

          const memoriaTotalOcupada = Top10ProcessesSize + OtherProcessesSize;

          console.log("Memoria Total: ", memoriaTotalOcupada, " Memoria Top 10: ", Top10ProcessesSize, " Memoria Otros Procesos: ", OtherProcessesSize);

          const memoriaLibreObj = {
            pid: -1,
            nombre: "Memoria Libre",
            total_tamanio: ((4096 - memoriaTotalOcupada) / 4096) * 100
          };

          setProcesosMemoria([...Top10Process, OtherMemory, memoriaLibreObj]);
        })
        .catch(error => {
          console.log(error)
        })
    };

    // Ejecutar la función fetchData() inicialmente
    fetchData();

    // Establecer un intervalo para ejecutar fetchData() cada 3 segundos
    const intervalId = setInterval(fetchData, 3000);

    // Limpiar el intervalo cuando el componente se desmonte para evitar fugas de memoria
    return () => clearInterval(intervalId);

  }, [])

  const data = {
    labels: procesosMemoria.map(item => item.nombre),
    datasets: [
      {
        label: '%',
        backgroundColor: [
          'rgba(255, 99, 132, 0.2)',
          'rgba(54, 162, 235, 0.2)',
          'rgba(255, 206, 86, 0.2)',
          'rgba(75, 192, 192, 0.2)',
          'rgba(153, 102, 255, 0.2)',
          'rgba(255, 159, 64, 0.2)',
          'rgba(255, 0, 0, 0.2)',
          'rgba(0, 255, 0, 0.2)',
          'rgba(0, 0, 255, 0.2)',
          'rgba(255, 0, 255, 0.2)',
          'rgba(255, 255, 0, 0.2)',
          'rgba(0, 255, 255, 0.2)',
          'rgba(128, 0, 128, 0.2)',
          'rgba(128, 128, 0, 0.2)',
          'rgba(0, 128, 128, 0.2)',
          'rgba(128, 0, 0, 0.2)',
          'rgba(0, 128, 0, 0.2)',
          'rgba(0, 0, 128, 0.2)',
          'rgba(192, 192, 192, 0.2)',
          'rgba(128, 128, 128, 0.2)',
          'rgba(64, 64, 64, 0.2)',
          'rgba(255, 215, 0, 0.2)',
          'rgba(128, 0, 128, 0.2)',
          'rgba(0, 255, 127, 0.2)',
          'rgba(255, 215, 0, 0.2)',
          'rgba(255, 69, 0, 0.2)',
          'rgba(218, 165, 32, 0.2)',
          'rgba(75, 0, 130, 0.2)',
          'rgba(139, 69, 19, 0.2)',
          'rgba(0, 250, 154, 0.2)'
        ],
        borderColor: [
          'rgba(255, 99, 132, 1)',
          'rgba(54, 162, 235, 1)',
          'rgba(255, 206, 86, 1)',
          'rgba(75, 192, 192, 1)',
          'rgba(153, 102, 255, 1)',
          'rgba(255, 159, 64, 1)',
          'rgba(255, 0, 0, 1)',
          'rgba(0, 255, 0, 1)',
          'rgba(0, 0, 255, 1)',
          'rgba(255, 0, 255, 1)',
          'rgba(255, 255, 0, 1)',
          'rgba(0, 255, 255, 1)',
          'rgba(128, 0, 128, 1)',
          'rgba(128, 128, 0, 1)',
          'rgba(0, 128, 128, 1)',
          'rgba(128, 0, 0, 1)',
          'rgba(0, 128, 0, 1)',
          'rgba(0, 0, 128, 1)',
          'rgba(192, 192, 192, 1)',
          'rgba(128, 128, 128, 1)',
          'rgba(64, 64, 64, 1)',
          'rgba(255, 215, 0, 1)',
          'rgba(128, 0, 128, 1)',
          'rgba(0, 255, 127, 1)',
          'rgba(255, 215, 0, 1)',
          'rgba(255, 69, 0, 1)',
          'rgba(218, 165, 32, 1)',
          'rgba(75, 0, 130, 1)',
          'rgba(139, 69, 19, 1)',
          'rgba(0, 250, 154, 1)'
        ],
        borderWidth: 2,
        data: procesosMemoria.map(item => item.total_tamanio)
      },

    ],
  }


  return (
    <div>
      <Chart type="pie" data={data} />
    </div>
  )
}
