const express = require('express');
const mysql = require('mysql');

const bodyParser = require('body-parser');

const PORT = process.env.PORT || 3050;

const app = express();

var jsonParser = bodyParser.json()
 
// create application/x-www-form-urlencoded parser
var urlencodedParser = bodyParser.urlencoded({ extended: false })

//Mysql
const connection = mysql.createConnection({
    host: 'localhost',
    user:'root',
    password:'hola123',
    database:'node20_mysql'
});

//Route
app.get('/',(req,res)=>{
    res.sendFile('index.html', { root: __dirname })
})

//All customers
app.get('/GET',(req, res) =>{
    
    const sql = 'SELECT * FROM lecturas ORDER BY lec_id DESC LIMIT 1';
    connection.query(sql,(err,results )=> {
        if (err) throw err;
        if(results.length > 0){
            res.json(results);
        }else{
            res.send('No results');
        }
    });
     
})



app.post('/add', urlencodedParser , (req,res)=>{

    const sql = 'INSERT INTO lecturas SET ?';

    const lecturaObj = {
        humedad: req.body.humedad,
        temperatura: req.body.temperatura,
        ppm: req.body.ppm,
        fecha: new Date()
    };

    

    connection.query(sql, lecturaObj, error => {
        if(error) throw error;
        res.send('Lectura creada + ' + lecturaObj.fecha);
    });
});








//Check connect
connection.connect(error =>{
    if(error) throw error;
    console.log('Database server running!');
});

app.listen(PORT, "192.168.100.48")