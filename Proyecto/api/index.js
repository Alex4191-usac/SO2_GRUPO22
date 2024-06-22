const express = require('express');
const http = require('http');
const cors = require('cors');
const bodyParser = require('body-parser');
const kernelRouter = require('./routes/kernelRoutes');

require('dotenv').config();

const app = express();

const PORT = process.env.PORT || 3001;

app.use(bodyParser.json());
app.use(cors());
app.use(express.json());


app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header('Access-Control-Allow-Headers', "Origin, X-Requested-With, Content-Type, Accept");
    next();
  });

  app.use('/kernel', kernelRouter);

  const listener = app.listen(PORT, () => {
    console.log(`Servidor corriendo en el puerto ${listener.address().port}`);
});

