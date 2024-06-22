CREATE DATABASE IF NOT EXISTS BaseUsoGeneral;

CREATE TABLE BaseUsoGeneral.Data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    llamada VARCHAR(255),
    pid INT,
    nombre VARCHAR(255),
    fecha VARCHAR(255),
    tamano INT
);