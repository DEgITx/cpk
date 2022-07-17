# <img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk_logo.png" width="60px" align="center" alt="Spectron icon"> CPK Package Manager

<p align="center"><a href="https://github.com/DEgiTx/cpk"><img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk.png"></a></p>

## CPK Package Manager

Very light and easy and fast native package manager to install **C/C++ (priority), JS, Python, Rust** packages and compile sources when it needed. Same package can be in muitple form for different languages.

Main purpose of this package manager to have simple C/C++ manager with posibility to post packages like **npm**, and also provide functionality to support same algorithms and code base for different programming languges, so if someone wanna **quicksort** package in their project on Python - they will get it, and if need the same one for C++ he will get it with simillar command.

Cross-platform. Implemented on C/C++ and provided for Linux, Mac OS, Windows arch.

## Usage

Install package:
```sh
cpk install package
```

Publish your own package:
```sh
cpk publish
```

Update all pacakges:
```sh
cpk update
```

### Available commands

Create a new application with the following options:

* `install PACKAGE` -  Install package
* `publish` - Publish current package
* `update` - Update tree of packages

### Build CPK client

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make
```

## License
[MIT](https://github.com/DEgiTx/cpk/blob/master/LICENSE)