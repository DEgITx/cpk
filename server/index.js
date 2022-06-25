const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');
require('tagslog')();

server.listen(9988);
app.use(express.json());

app.post('/publish', function (req, res) {
    logT('publish', 'publish', req.body);

    res.send({
        hello: 'yes'
    })
});
