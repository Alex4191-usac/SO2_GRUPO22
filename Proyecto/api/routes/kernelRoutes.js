const express = require('express');
const router = express.Router();
const kernelController = require('../controller/kernelController');


router.get('/allData', kernelController.getAllData);
router.get('/getByProceso', kernelController.getByProceso);
router.get('/hello', kernelController.hello);

module.exports = router;
