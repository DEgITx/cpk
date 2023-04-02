# <img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk_logo.png" width="60px" align="center" alt="Cpk Package manager"> CPK Package Manager

<p align="center"><a href="https://github.com/DEgiTx/cpk"><img src="https://raw.githubusercontent.com/DEgITx/cpk/master/resources/cpk.png"></a></p>

<p align="center">
<a href="https://github.com/DEgITx/cpk/releases"><img src="https://github.com/DEgITx/cpk/actions/workflows/build.yml/badge.svg" /></a>
<a href="https://github.com/DEgITx/cpk/releases"><img src="https://img.shields.io/github/release/DEgITx/cpk.svg" /></a>
<a href="https://github.com/DEgITx/cpk/README.md"><img src="https://img.shields.io/badge/docs-faq-brightgreen.svg" /></a>
</p>

<b><p align="center"><a href="https://cpkpkg.com">https://cpkpkg.com - official repository site (search, download, information about packages)</a></p></b>

## CPK Package Manager

<p align="center"><img src="https://github.com/DEgITx/share/blob/main/cpk/cpk_basic.gif?raw=true" /></p>

Very light and easy and fast native package manager to install **C/C++ (priority), JS, Python, Rust** packages and compile sources when it needed. Same package can be in muitple form for different languages.

Main purpose of this package manager to have simple C/C++ manager with possibility to post packages like **npm**, and also provide functionality to support same algorithms and code base for different programming languages, so if someone wanna **quicksort** package in their project on Python - they will get it, and if need the same one for C++ he will get it with simillar command.

CPK has possibility to search and install packages using **GPT** by given description, generate example code for installed libraries using `cpk nn` command. So you can not only install libraries, but find needed and use them immediately.  

Cross-platform. Implemented on C/C++ and provided for Linux, Mac OS, Windows arch.

## Features
* Lightweight, written on C/C++
* Cross-platform. Prebuilds packages for popular systems (like Windows, Linux, macOS), just download and ready to use. (Recommended add to path var)
* Publishing your own package immediately with "cpk publish" command inside project directory. Package ready to be used on other side right after with "cpk install". (no intermediate moderation etc...)
* Everyone can publish the own package library or binary with one simple command.
* Keep installation & popularity statistic to recommend most popular packages and libs (can be yours).
* Using external effective build managers: like cmake, mingw32.
* Easy to use configuration for packages: json based package config (very similar to npm).
* Use the builded packages and libraries in your project with simple `cpk command` alias. For example: `cpk cmake -G Ninja ../` will apply all needed libraries in your project.
* Use GPT to search packages by description and your needes without actual knowing of package name.
* Generate example code using GPT to any of the libraries right after library installation.

## Download/install

Download last available version on [CPK at download page](https://github.com/DEgITx/cpk/releases). CPK is console based utility, so extact archive and add CPK utility to PATH of the system.

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

After command execution the package must become published/updated on https://cpkpkg.com 

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

## Use of installed packages in your project

For example you installed zlib library that your project required
```sh
cpk install zlib
```
and you want to use it inside your project. You just need to execute cmake command for your project with cpk alias, like this:
```sh
# Insead of using:
# cmake ../
# use follow:
cpk cmake ../
```

The package will be founded in your project:

```cmake
cmake_minimum_required(VERSION 3.1)
project(zlib-test)

# The library will be founded with cpk
find_package(ZLIB REQUIRED)
```

It is also possible to execute with gcc, clang and other build tools (not only cmake types of project).

### How does it work ?

CPK will capture cmake command arguments and will add it own libraries and include pathes to cmake execution:

<p align="center"><img src="https://github.com/DEgITx/share/blob/main/cpk/cpk_args.png?raw=true" /></p>

In this example it's reaply arguments using sysroot with generated cmake -DCMAKE_PREFIX_PATH argument.

## Use ChatGPT as part of CPK

### Search packages by description
You can find any needed packages without knowing it names.

Use:
```
cpk nn search Library to extract zip archives
```

You will suggested to install libzip packages or other alternative.

## Available commands

Available commands and options:

* `install PACKAGE` -  Install package
* `publish` - Publish current package
* `update` - Update tree of packages
* `packages` - List of available packages
* `info PACKAGE` - Recive information about package (like descriptions, downloads, version and so on)
* `nn search [description]` - use to find any package/library accroding any description using ChatGPT. Like: *"library to open and extract zip files"*. It will suggess to install libzip package.
* `[build command]` - like `cpk cmake -G Ninja ../` to build project used installed libraries
* `-h` - Help
* `-v` - version of CPK

## Build CPK Client by own

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j8
```

## License
[MIT](https://github.com/DEgiTx/cpk/blob/master/LICENSE)

## Comparing with other package managers

### Conan
* No python and pip manager is required to start work with. The cpk in binary and source form on C/C++
* No python required to create package, configuration of packages is with simple json syntax declared in cpk.json
* Publishing package mechanism is easy
* Open-source backend as part of cpk
* Neural networks features (like search/install packages without knowing package real name or generate example library code)

### Vcpkg
* Don't need to build cpk like vcpkg, cpk binaries is also presented for most popular OS's. Just download archive with cpk binary and add it to path
* Easy to start work with from the box (easy installing and publishing)
* To start usage the package add "cpk" before cpk command is enought "cpk cmake ../" will take care for dependencies in your project.
* Open-source backend as part of cpk
* Neural networks features (like search/install packages without knowing package real name or generate example library code)
