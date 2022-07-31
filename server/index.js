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

function isNumeric(str) {
    if (typeof str != "string") return false // we only process strings!  
    return !isNaN(str) && // use type coercion to parse the _entirety_ of the string (`parseFloat` alone does not do this)...
           !isNaN(parseFloat(str)) // ...and ensure strings of whitespace fail
  }
function changeVersion(version) {
    const versionArr = version.trim().split('.');
    for(let i = versionArr.length - 1; i > 0; i--) {
        if (isNumeric(versionArr[i])) {
            versionArr[i]++;
            break;
        }
    }
    return versionArr.join('.');
}

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

        if(!package.language || package.language.length == 0) {
            logW('package', 'no language for package');
            res.send({
                error: true,
                errorCode: 6,
                errorDesc: `No language for package`,
            });
            return;
        }

        if(!package.buildType || package.buildType.length == 0) {
            logW('package', 'no buildType');
            res.send({
                error: true,
                errorCode: 7,
                errorDesc: `No buildType`,
            });
            return;
        }

        const pkgInfo = await redis.get(`cpk:packages:${package.package}`);
        if (package.dependencies) {
            if (typeof package.dependencies != 'object' || Object.keys(package.dependencies).length == 0) {
                res.send({
                    error: true,
                    errorCode: 3,
                    errorDesc: `Not array deps`,
                });
                return;
            }

            for (let dep in package.dependencies) {
                const depVersion = package.dependencies[dep];
                const pkg = await redis.get((depVersion && depVersion.length != 0) ? `cpk:archive:${dep}:${depVersion}` : `cpk:packages:${dep}`);
                if (!pkg) {
                    res.send({
                        error: true,
                        errorCode: 4,
                        errorDesc: `Package from deps not founded ${dep} with version ${depVersion}`,
                    });
                    return;
                }
            }
        }

        let oldVersion;
        if (!pkgInfo?.version) {
            package.version = '0.1';
            logT('version', 'set basic version', package.version);
        } else {
            oldVersion = pkgInfo.version;
            package.version = changeVersion(pkgInfo.version);
            if (package.version == oldVersion) {
                res.send({
                    error: true,
                    errorCode: 5,
                    errorDesc: `Can't change version for ${oldVersion} to ${package.version}`,
                });
                return;
            }
            logT('version', 'change version from', oldVersion, 'to', package.version);
        }

        logT('zip', 'archive', package);
        if (!fs.existsSync(PACKAGES_DIR + package.package)){
            logT('zip', 'create new dir for package', PACKAGES_DIR + package.package);
            fs.mkdirSync(PACKAGES_DIR + package.package);
        }
        fs.writeFileSync(PACKAGES_DIR + package.package + "/" + 'package.zip', req.body);
        logT('zip', 'archive', package.package, 'saved');
        fs.writeFileSync(PACKAGES_DIR + package.package + "/" + `package_${package.version}.zip`, req.body);
        logT('zip', 'archive', `package_${package.version}.zip`, 'saved for version', package.version);

        package.isLastVersion = true;
        if (pkgInfo) {
            pkgInfo.isLastVersion = false;
            await redis.set(`cpk:archive:${package.package}:${oldVersion}`, pkgInfo);
        }
        await redis.set(`cpk:packages:${package.package}`, package);
        await redis.set(`cpk:archive:${package.package}:${package.version}`, package);
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
    if(!request.packages || typeof request.packages != 'object' || Object.keys(request.packages).length == 0) {
        res.send({
            error: true,
            errorCode: 2,
            errorDesc: "No packages for install",
        });
        return;
    }

    const packagesMap = {}
    const recursiveInstall = async (packagesObject) => {
        let packageNames = Object.keys(packagesObject);
        packageNames = packageNames.filter(name => {
            if (packagesObject[name]?.length > 0) {
                return !packagesMap[`${name}:${packagesObject[name]}`]
            } else {
                return !packagesMap[name];
            }
        });
        if (packageNames.length == 0)
            return;
        logT("deps", "add deps", packageNames);
        await Promise.all(packageNames.map(async (packageName) => { 
            let package;
            if (packagesObject[packageName]?.length > 0) {
                package = await (await redis.DB.archive)[`${packageName}:${packagesObject[packageName]}`];
            } else {
                package = await (await redis.DB.packages)[packageName];
            }
            if (!package) {
                logTW("deps", "no deps found", packageName);
                throw new Error("Not found deps for package " + packageName);
            }
            if (!(packagesObject[packageName]?.length > 0) || package.isLastVersion) {
                packagesMap[package.package] = package;
            }
            packagesMap[`${package.package}:${package.version}`] = package;
            if (package.dependencies && Object.keys(package.dependencies).length > 0) {
                await recursiveInstall(package.dependencies);
            }
            return package;
        }));
    }
    try {
        await recursiveInstall(request.packages);
    } catch(e) {
        logTW("deps", "one of the deps missing");
        res.send({
            error: true,
            errorCode: 3,
            errorDesc: e.message,
        });
        return;
    }
    
    let packages = Object.keys(packagesMap);
    if (!packages || packages.length == 0)
        return;
    packages = packages.filter(name => name.includes(':')).map(name => packagesMap[name]);

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

app.post('/packages', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
    const request = req.body;

    const packages = await redis.values("cpk:packages:*");

    res.send({
        packages,
    })
});