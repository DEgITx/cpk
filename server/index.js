const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');
require('tagslog')();

server.listen(9988);
app.use(express.json());
app.use(express.raw({
    inflate: true,
    limit: '5mb',
    type: 'application/zip'
}));

const packages = {};

app.post('/publish', function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 

    if (req.body instanceof Buffer) {
        if (!packages[ip]) {
            logW('package', 'no ip for this zip package');
            return;
        }
        const package = packages[ip];
        delete packages[ip];
        logT('zip', 'archive', package);
    } else {
        logT('package', 'publish', 'ip', ip, 'info', req.body);
        packages[ip] = req.body;
    }

    res.send({
        hello: 'yes'
    })
});
