# proxy-fmu

`proxy-fmu` is a tool-independent solution for running FMUs compatible with FMI for Co-simulation version 1.x & 2.x in distributed fashion.
`proxy-fmu` will automatically spawn a new process for each instance that is instantiated from an FMU.

By default, `proxy-fmu` targets `localhost`. For this behaviour zero configuration is required apart from the fact that the `proxyfmu` executable must be available.
It is also possible to make `proxy-fmu` run instances on a remote computer. For this to work, the `proxyfmu-booter` executable must be started on the remote computer and provided with a valid port.

In short, this solution allows you to:
* Import FMI 1.0 models in software that otherwise only supports FMI 2.0.
* Instantiate multiple instances of FMUs that only allows one instance per process.
* The ability to run the FMU on some remote resource
    * Which in turn allows FMUs to run on otherwise unsupported platforms.

The solution is written in C++ and the static library and bundled executables have no dependencies.

Currently, only Windows and Linux are supported, but there are no known obstacles for supporting the Darwin platform.

### Building

Building `proxy-fmu` is straightforward, however, conan is a hard requirement:
```cmd
pip install conan
```

For an example on how to build the project using the command line, refer to the [CI setup](https://github.com/open-simulation-platform/proxy-fmu/blob/master/.github/workflows/build.yml).


### Installing

`proxy-fmu` is hosted by the conan remote https://osp.jfrog.io/artifactory/api/conan/conan-local

Add the remote using the command:
```
conan remote add osp https://osp.jfrog.io/artifactory/api/conan/conan-local
```

Then add a dependency to: `proxyfmu/<version>@osp/stable`

The artifact contains both the library and required executables.

### proxy-fmu & libcosim

Adding `proxy_fmu` support for [libcosim](https://github.com/open-simulation-platform/libcosim/blob/master/conanfile.py) is as simple as adding `-o proxyfmu=True` to its conan command.


### How is this project related to `FMU-proxy`?

This library serves similar purpose as [FMU-proxy v.0.6.2](https://github.com/NTNU-IHB/FMU-proxy/releases/tag/v0.6.2). 
However, it requires no JVM and automatically spawns new processes.
The current development of FMU-proxy is about wrapping existing FMUs into new FMUs with network capabilities, so they now serve different purposes. `proxy-fmu` is a library that apps can integrate with and FMU-proxy produces FMI compatible wrapper models.
