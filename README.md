# <img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk_logo.png" width="60px" align="center" alt="Spectron icon"> CPK Package Manager

<p align="center"><a href="https://github.com/DEgiTx/cpk"><img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk.png"></a></p>

<p align="center"><a href="https://cpkpkg.com">https://cpkpkg.com - official repository site (search, download, information about packages)</a></p>

## CPK Package Manager

Very light and easy and fast native package manager to install **C/C++ (priority), JS, Python, Rust** packages and compile sources when it needed. Same package can be in muitple form for different languages.

Main purpose of this package manager to have simple C/C++ manager with possibility to post packages like **npm**, and also provide functionality to support same algorithms and code base for different programming languages, so if someone wanna **quicksort** package in their project on Python - they will get it, and if need the same one for C++ he will get it with simillar command.

Cross-platform. Implemented on C/C++ and provided for Linux, Mac OS, Windows arch.

## Usage

### Installing packages
```sh
cpk install package
```
Install 2 packages package=1.0 version and package2 latest version
```sh
cpk install package@1.0 package2
```

### Publish own package
Publish your own package (inside directory of project):
```sh
cpk publish
```

### List of available packages
List of available packages for install:
```sh
cpk packages
```

### Update packages
Update all packages:
```sh
cpk update
```

## Publish own package

CPK package manager created to distribute any kind of open source packages, but we want to *guarantee that any package can be used in commertial software* in any form of use. So we recomend to use licenses like BSD or MIT if it possible.

to publish your package you can create *cpk.json* with following very basic config with package name and list of dependencies:
```json
{
	"package": "example",
	"dependencies": {
		"zlib": ""
	}
}
```

and use

```sh
cpk publish
```
command to publish you own package

[https://cpkpkg.com/YOUR_PACKAGE](https://cpkpkg.com/zlib) - After publishing the package, it will displayed on reposity site.

## Available commands

Create a new application with the following options:

* `install PACKAGE` -  Install package
* `publish` - Publish current package
* `update` - Update tree of packages
* `packages` - Update tree of packages
* `-h` - Help

## Build CPK Client by own

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j8
```

## License
[MIT](https://github.com/DEgiTx/cpk/blob/master/LICENSE)