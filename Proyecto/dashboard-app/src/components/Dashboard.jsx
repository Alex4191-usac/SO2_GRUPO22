import React, {useEffect, useState} from "react";
import axios from 'axios'
import GraficaPie from "./GraficaPie";
export default function Dashboard() {

  const [procesos, setProcesos] = useState([]);
  const [procesosMemoria, setProcesosMemoria] = useState([]);

  useEffect(() => {

    const fetchData = () => {
        axios.get(`http://127.0.0.1:3001/kernel/allData`)
          .then(response => {
            setProcesos(response.data)
          })
          .catch(error => {
            console.log(error)
          })

      axios.get(`http://127.0.0.1:3001/kernel/getByProceso`)
          .then(response => {
            setProcesosMemoria(response.data)
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

    return (

      <>

        <div class="container mx-auto">
          <div className="flex items-center justify-center h-full mt-12">
            <h1 className="text-4xl font-bold mb-4">DASHBOARD</h1>
          </div>
          <div class="grid grid-cols-2 gap-4 mt-12">
            
            <div class="bg-gray-100 p-4">
              <div className="flex items-center justify-center mt-1">
                <h1 className="text-4xl font-bold mb-4">Grafica Memoria utilizada</h1>
              </div>
              <GraficaPie/>
            </div>

            <div class="bg-gray-100 p-4">
              <div className="flex items-center justify-center mt-1">
                <h1 className="text-4xl font-bold mb-4">Procesos y Memoria</h1>
              </div>
              <div class="mt-4" >
                <div className="max-h-40">
                  <table className="table-auto w-full">
                    <thead className="bg-black text-white">
                      <tr>
                        <th className="p-4 w-1/4 text-center">PID</th>
                        <th className="p-4 w-1/4 text-center">Nombre</th>
                        <th className="p-4 w-2/4 text-center">Memoria</th>
                      </tr>
                    </thead>
                  </table>
                  <div className="overflow-y-scroll max-h-64">
                    <table className="table-auto w-full">
                      <tbody>
                        {procesosMemoria.map(proceso => (
                          <tr key={proceso.pid}>
                            <td className="p-4 w-1/4 text-center">{proceso.pid}</td>
                            <td className="p-4 w-1/4 text-center">{proceso.nombre}</td>
                            <td className="p-4 w-1/4 text-center">{proceso.total_tamanio} MB</td>
                            <td className="p-4 w-1/4 text-center">{(proceso.total_tamanio * 100) / 4000 } % </td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>
              
            </div>
          </div>

          <div class="mt-12" >
            <h1 className="text-4xl font-bold mb-4">Solicitudes</h1>
            <div className="max-h-40">
              <table className="table-auto w-full">
                <thead className="bg-black text-white">
                  <tr>
                    <th className="p-4 w-1/4 text-center">PID</th>
                    <th className="p-4 w-1/4 text-center">Llamada</th>
                    <th className="p-4 w-1/4 text-center">Tamaño</th>
                    <th className="p-4 w-1/4 text-center">Fecha</th>
                  </tr>
                </thead>
              </table>
              <div className="overflow-y-scroll max-h-64">
                <table className="table-auto w-full">
                  <tbody>
                    {procesos.map(proceso => (
                      <tr key={proceso.pid}>
                        <td className="p-4 w-1/4 text-center">{proceso.pid}</td>
                        <td className="p-4 w-1/4 text-center">{proceso.llamada}</td>
                        <td className="p-4 w-1/4 text-center">{proceso.tamano}</td>
                        <td className="p-4 w-1/4 text-center">{proceso.fecha}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          </div>
          
        </div>
       
      </>
    );
  }
  