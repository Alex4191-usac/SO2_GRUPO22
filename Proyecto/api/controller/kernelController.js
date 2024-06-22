const db = require('../utils/db');
const bodyParser = require('body-parser');


async function getAllData(req, res) {
    try {
        const query = 'SELECT * FROM Data';
        const result = await db.query(query);
        console.log(result[0]);    
        res.json(result[0]);
    } catch (error) {
        console.log(error);
    }
}

async function getByProceso(req, res) {
    try {
        const query = `select pid, nombre, SUM(tamano) as total_tamanio
                        from Data
                        GROUP BY pid, nombre;`;
        const result = await db.query(query);
        console.log(result[0]);    
        res.json(result[0]);
    } catch (error) {
        console.log(error);
    }
}

async function hello(req, res) {
    try {
        const result = 'holaa'
        console.log(result);    
        res.json(result);
    } catch (error) {
        console.log(error);
    }
}

module.exports = {
    getAllData, hello, getByProceso
};