const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');

server.listen(9988);

app.get('/publish', function (req, res) {
    res.send({
        hello: 'yes'
    })
});
