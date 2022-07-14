const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');
require('tagslog')();
let redis;

async function f() {
    redis = await require('./redis')();
    server.listen(9988);
}
f();
app.use(express.json());
app.use(express.raw({
    inflate: true,
    limit: '5mb',
    type: 'application/zip'
}));

const packages = {};
const PACKAGES_DIR = 'e:/Projects/cpk/build/packages/'

app.post('/publish', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 

    if (req.body instanceof Buffer) {
        if (!packages[ip]) {
            logW('package', 'no ip for this zip package');
            return;
        }
        const package = packages[ip];
        delete packages[ip];
        if (!package.package) {
            logW('package', 'no package name');
            return;
        }

        logT('zip', 'archive', package);
        if (!fs.existsSync(PACKAGES_DIR + package.package)){
            logT('zip', 'create new dir for package', PACKAGES_DIR + package.package);
            fs.mkdirSync(PACKAGES_DIR + package.package);
        }
        fs.writeFileSync(PACKAGES_DIR + package.package + "/" + 'package.zip', req.body);
        logT('zip', 'archive', package.package, 'saved');

        await redis.set(`cpk:packages:${package.package}`, package);
    } else {
        logT('package', 'publish', 'ip', ip, 'info', req.body);
        packages[ip] = req.body;
    }

    res.send({
        hello: 'yes'
    })
});

app.post('/get', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
    const request = req.body;
    if (!request) {
        return;
    }
    if(!request.package) {
        return;
    }

    const dbPackage = await (await redis.DB.packages)[request.package];
    if (!dbPackage) {
        logT('get', 'no such package', request.package);
    }

    const package = {
        package: request.package,
        url: PACKAGES_DIR + package.package + "/" + 'package.zip',
        version: '0.0.1'
    }

    res.send(package)
});