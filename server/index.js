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

app.post('/install', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
    const request = req.body;
    if (!request) {
        return;
    }
    if(!request.packages || !Array.isArray(request.packages) || request.packages.length == 0) {
        return;
    }

    const recursiveInstall = async (packageNames) => {
        const packages = await Promise.all(packageNames.map(async (packageName) => (await redis.DB.packages)[packageName]));
        return packages;
    }
    const packages = await recursiveInstall(request.packages);

    logT("install", packages);

    let pkg;
    for(const package of packages)
    {
        pkg = {
            package: package.package,
            url: PACKAGES_DIR + package.package + "/" + 'package.zip',
            version: '0.0.1'
        };
    }

    res.send(pkg)
});