# Minimal IPOPT C++ Example

A minimal working example demonstrating IPOPT (Interior Point OPTimizer) usage in C++ on Windows, using conda-forge as the package manager.

## Problem

This example solves the simplest possible nonlinear optimization problem:

```
Minimize: (x - 2)²
Subject to: x ≥ 0
Starting point: x = 5
```

Expected solution: `x = 2`, `f(x) = 0`

## Prerequisites

- Windows 10/11
- [Miniconda](https://docs.conda.io/en/latest/miniconda.html) installed at `%USERPROFILE%\AppData\Local\miniconda3`
- Visual Studio 2019 Build Tools (or full Visual Studio 2019)
- CMake 3.20 or higher

## Quick Start

```cmd
git clone <repository-url>
cd test_cpp_ipopt
build.cmd
```

The build script automatically:

1. Creates a conda environment `ipopt-dev` with IPOPT (if not exists)
2. Activates the environment
3. Sets up Visual Studio environment
4. Configures and builds the project
5. Runs the example

## Project Structure

```
test_cpp_ipopt/
├── CMakeLists.txt      # CMake configuration
├── main.cpp            # IPOPT example implementation
├── build.cmd           # Build and run script
├── README.md           # This file
├── lessons_learned.md  # Development notes
├── LICENSE             # WTFPL license
└── .gitignore
```

## Output

Successful execution produces:

```
This is Ipopt version 3.14.19, running with linear solver MUMPS 5.8.1.
...
EXIT: Optimal Solution Found.

=== Solution ===
x = 2
f(x) = 6.39853e-24
Expected: x = 2.0, f(x) = 0.0

Optimization succeeded!
```

## Customization

### Changing the Conda Path

Edit `build.cmd` and modify:

```batch
set "CONDA_ROOT=C:\Users\<your-username>\AppData\Local\miniconda3"
```

### Changing the Visual Studio Version

Edit `build.cmd` and modify the vcvars path:

```batch
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

For VS2022:

```batch
call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

## IPOPT Configuration

The linear solver is set programmatically in `main.cpp`:

```cpp
app->Options()->SetStringValue("linear_solver", "mumps");
```

Other useful options:

```cpp
app->Options()->SetIntegerValue("print_level", 5);        // Verbosity (0-12)
app->Options()->SetNumericValue("tol", 1e-8);             // Convergence tolerance
app->Options()->SetIntegerValue("max_iter", 1000);        // Maximum iterations
app->Options()->SetStringValue("mu_strategy", "adaptive"); // Barrier parameter strategy
```

## References

- [IPOPT Documentation](https://coin-or.github.io/Ipopt/)
- [IPOPT GitHub Repository](https://github.com/coin-or/Ipopt)
- [conda-forge IPOPT Package](https://anaconda.org/conda-forge/ipopt)
- [TNLP Interface Reference](https://coin-or.github.io/Ipopt/classIpopt_1_1TNLP.html)

## License

This project is licensed under the WTFPL - see the [LICENSE](LICENSE) file.
