
## Building

See below for notes on building the project for both Windows and Unix.


First, install [conan](https://conan.io/).

Finally, run `conan install`
```bash
conan install . -s build_type=Debug --install-folder=cmake-build-debug --build=missing
conan install . -s build_type=Release --install-folder=cmake-build-release --build=missing
```

On Linux you should add `-s compiler.libcxx=libstdc++11` to the command.


### FMI4cpp

```fmi4cpp``` is bundled as a git sub-module, so you'll need to ensure that you are also keeping the FMI4cpp sub-folder up-to-date.
To initialize the sub-module you can run:
```bash
git submodule update --init --recursive
```
To update it, run:
```bash
git submodule update --recursive
```
