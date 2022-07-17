const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');
const path = require('path');
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
const OUT_DIR = path.resolve(__dirname + "/../public") + "/";
const PACKAGES_DIR = OUT_DIR + 'packages/'
logT("core", `public path: ${OUT_DIR}`);
app.use(express.static(OUT_DIR));

app.post('/publish', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 

    if (req.body instanceof Buffer) {
        if (!packages[ip]) {
            logW('package', 'no ip for this zip package');
            res.send({
                error: true,
                errorCode: 1,
                errorDesc: `Not found package data from this zip`,
            });
            return;
        }
        const package = packages[ip];
        delete packages[ip];
        if (!package.package) {
            logW('package', 'no package name');
            res.send({
                error: true,
                errorCode: 2,
                errorDesc: `No package name`,
            });
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
        success: true
    });
});

app.post('/install', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
    const request = req.body;
    if (!request) {
        res.send({
            error: true,
            errorCode: 1,
            errorDesc: "No body",
        });
        return;
    }
    if(!request.packages || !Array.isArray(request.packages) || request.packages.length == 0) {
        res.send({
            error: true,
            errorCode: 2,
            errorDesc: "No packages for install",
        });
        return;
    }

    const packagesMap = {}
    const recursiveInstall = async (packageNames) => {
        packageNames = packageNames.filter(name => !packagesMap[name]);
        if (packageNames.length == 0)
            return;
        logT("deps", "add deps", packageNames);
        await Promise.all(packageNames.map(async (packageName) => { 
            const package = await (await redis.DB.packages)[packageName];
            if (!package) {
                logTW("deps", "no deps found", packageName);
                res.send({
                    error: true,
                    errorCode: 3,
                    errorDesc: `One of the deps for ${packageName} is not founded`,
                });
                return;
            }
            packagesMap[package.package] = package;
            if (package.dependencies && package.dependencies.length > 0) {
                await recursiveInstall(package.dependencies);
            }
            return package;
        }));
    }
    await recursiveInstall(request.packages);
    const packages = Object.values(packagesMap);
    if (!packages || packages.length == 0)
        return;

    logT("install", packages);

    const packagesToInstall = []
    for(const package of packages)
    {
        packagesToInstall.push({
            package: package.package,
            url: "http://localhost:9988/packages/" + package.package + "/" + 'package.zip',
            version: '0.0.1'
        });
    }

    res.send({
        packages: packagesToInstall,
    })
});