import { Outlet } from 'react-router-dom'
import Dashboard from './components/Dashboard'

function App() {

  return (
    <>
      <nav className="sticky top-0 bg-green-500 p-4 flex justify-between items-center">
        <div className="container mx-auto">
          <h1 className="text-white text-3xl font-bold">Proyecto Final - Sistemas Operativos 2 </h1>
        </div>
      </nav>
      <Outlet />
      <Dashboard />
    </>
  )
}

export default App
