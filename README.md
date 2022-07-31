# <img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk_logo.png" width="60px" align="center" alt="Spectron icon"> CPK Package Manager

<p align="center"><a href="https://github.com/DEgiTx/cpk"><img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk.png"></a></p>

## CPK Package Manager

Very light and easy and fast native package manager to install **C/C++ (priority), JS, Python, Rust** packages and compile sources when it needed. Same package can be in muitple form for different languages.

Main purpose of this package manager to have simple C/C++ manager with possibility to post packages like **npm**, and also provide functionality to support same algorithms and code base for different programming languages, so if someone wanna **quicksort** package in their project on Python - they will get it, and if need the same one for C++ he will get it with simillar command.

Cross-platform. Implemented on C/C++ and provided for Linux, Mac OS, Windows arch.

## Package philosophy / license

CPK package manager created to distribute any kind of open source packages, but we want to *guarantee that any package can be used in commertial software* in any form of use. So if anyone who wanna install package will be ensure that he allowed to use it and modify without any restrictions even for commertial products. So we recommend to stick to MIT, BSD or other commertial free licenses. 

*Everyone can publish their own package.*

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