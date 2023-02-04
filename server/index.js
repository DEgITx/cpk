const express = require('express');
const app = express();
const server = require('http').Server(app);
const fs = require('fs');
const {readFile} = require('fs/promises');
const path = require('path');
const template = require('lodash.template');
const AdmZip = require("adm-zip");
const moment = require('moment');
const createDOMPurify = require('dompurify');
const { JSDOM } = require('jsdom');
const marked = require('marked');
require('tagslog')();
let redis;
global.PRODUCTION = (process.env.NODE_ENV == 'production');

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

const parsePackages = (packages) => {
    if (!packages)
        return;
    
    return packages.map (p => {
        p.publishDate = moment(p.publishDate).format('MMMM Do YYYY, h:mm a');
        return p;
    })
}

app.get('/', async function (req, res) {
    const packages = await redis.values("cpk:packages:*");
    res.send(render('index', {
        packages : parsePackages(packages) || []
    }))
});

const packages = {};
const ROOT_DIRECTORY = path.resolve(__dirname + '/..') + '/';
const OUT_DIR = path.resolve(__dirname + "/../public") + "/";
const PACKAGES_DIR = OUT_DIR + 'packages/'
logT("core", `public path: ${OUT_DIR}`);
app.use(express.static(OUT_DIR));
app.use('/resources', express.static(ROOT_DIRECTORY + 'resources'));

function render(view, ctx = {}) {
    return template(fs.readFileSync(OUT_DIR + `/${view}.html`))(ctx)
}

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
        if (!package.package || package.package.includes('/') || package.package.includes('\\')) {
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
        package.installed = pkgInfo?.installed || 0;

        logT('zip', 'archive', package);
        if (!fs.existsSync(PACKAGES_DIR + package.package)){
            logT('zip', 'create new dir for package', PACKAGES_DIR + package.package);
            fs.mkdirSync(PACKAGES_DIR + package.package);
        }
        fs.writeFileSync(PACKAGES_DIR + package.package + "/" + 'package.zip', req.body);
        logT('zip', 'archive', package.package, 'saved');
        fs.writeFileSync(PACKAGES_DIR + package.package + "/" + `package_${package.version}.zip`, req.body);
        logT('zip', 'archive', `package_${package.version}.zip`, 'saved for version', package.version);

        const {size: fileSize} = fs.statSync(PACKAGES_DIR + package.package + "/" + 'package.zip');

        const zip = new AdmZip(PACKAGES_DIR + package.package + "/" + 'package.zip');
        const zipEntries = zip.getEntries();
        try {
            zip.extractEntryTo(/*entry name*/ "README.md", /*target path*/ PACKAGES_DIR + package.package, /*maintainEntryPath*/ false, /*overwrite*/ true);
            logT('package', 'README.md exists, extract');
        } catch(err) {
            logT('package', 'no README.md');
        }
        package.entries = zipEntries.length;
        package.isLastVersion = true;
        package.archiveSize = fileSize;
        package.publishDate = Date.now();
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
    const packagesDepsCount = {}
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
                for (const dep in package.dependencies)
                {
                    if (!packagesDepsCount[dep]) {
                        packagesDepsCount[dep] = 0;
                    }
                    packagesDepsCount[dep]++;
                }
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
    packages.sort((a, b) => {
        const valA = packagesDepsCount[a.package] || -1;
        const valB = packagesDepsCount[b.package] || -1;
        console.log(valA, valB)
        return valB - valA;
    })
    logT("install", packages);

    const packagesToInstall = []
    for(const package of packages)
    {
        packagesToInstall.push(Object.assign(package, {
            url: `http://${PRODUCTION ? '143.244.189.114' : 'localhost'}:9988/packages/` + package.package + "/" + 'package.zip',
        }));
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

app.post('/installed', async function (req, res) {
    const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress 
    const request = req.body;

    if (request?.package && request.success) {
        const package = await redis.get(`cpk:packages:${request.package}`);
        if (typeof package?.installed != 'undefined') {
            package.installed++;
            logT('installed', 'installed', package.package, 'times', package.installed);
            await redis.set(`cpk:packages:${package.package}`, package);
        }
    }

    res.send({success: true})
});

const wind = new JSDOM('').window;
const DOMPurify = createDOMPurify(wind);
app.get('/:package', async (req, res) => {
    // get the link from the request parameters
    const packageName = req.params.package;
    const package = await redis.get(`cpk:packages:${packageName}`);
    if (package) {
        let readmeFile = await readFile(PACKAGES_DIR + package.package + "/README.md", 'utf8');
        if (readmeFile) {
            readmeFile = DOMPurify.sanitize(marked.parse(readmeFile.toString()));
        }
        res.send(render('index', {
            package : package,
            readme: readmeFile,
        }));
        return;
    }
    res.status(404).send('Link not found');
  });
  