# <img src="https://cloud.githubusercontent.com/assets/378023/15063284/cf544f2c-1383-11e6-9336-e13bd64b1694.png" width="60px" align="center" alt="Spectron icon"> CPK Package Manager

## CPK Package Manager

Very light and easy and fast native package manager to install **C/C++ (priority), JS, Python, Rust** packages and compile sources when it needed. Same package can be in muitple form for different languages.

Main purpose of this package manager to have simple C/C++ manager with posibility to post packages like npm, and also provide functionality to support same algorithms and code base on different languges, so if someone wanna **quicksort** package in their project on Python - they will get and if need same one for C++ he will get it with same command.

Implemented on C/C++ and provided for Linux, Mac OS, Windows arch.

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
