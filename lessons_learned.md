# Lessons Learned: IPOPT on Windows with C++

This document captures the challenges encountered and solutions found while setting up a minimal IPOPT example on Windows.

## 1. Package Manager Selection

### Problem

IPOPT requires a sparse linear solver. The available options are:

- **MUMPS** - Open source, included in official releases
- **HSL (MA27, MA57, etc.)** - Requires license/registration
- **Pardiso** - Commercial or via Intel MKL
- **SPRAL** - Open source, primarily for Linux

### Findings

| Package Manager | IPOPT Available | MUMPS Included | Windows Support |
| --------------- | --------------- | -------------- | --------------- |
| vcpkg           | ✅ Yes          | ❌ No          | ✅ Yes          |
| Conan           | ❌ No           | -              | -               |
| conda-forge     | ✅ Yes          | ✅ Yes         | ✅ Yes          |

### Solution

**conda-forge** is the only C++ package manager that provides IPOPT with MUMPS pre-built for Windows.

```cmd
conda create -n ipopt-dev -c conda-forge ipopt
```

## 2. vcpkg's coin-or-ipopt Limitations

### Problem

vcpkg's `coin-or-ipopt` package builds without MUMPS. It only supports:

- HSL solvers (loaded at runtime via `libhsl.dll`)
- Pardiso (loaded at runtime via `libpardiso.dll`)

### Error Encountered

```
Exception of type: DYNAMIC_LIBRARY_FAILURE
Error 126 while loading DLL libhsl.dll: The specified module could not be found.
```

### Root Cause

vcpkg's portfile doesn't include MUMPS as a dependency because:

1. MUMPS requires Fortran compilation
2. MUMPS is not available as a vcpkg port
3. The port relies on runtime-loadable solvers instead

### Lesson

Always check what linear solvers a pre-built IPOPT package includes before committing to a package manager.

## 3. CMake Integration Without pkg-config

### Problem

vcpkg's IPOPT only provides pkg-config files (`.pc`), not CMake config files. Windows MSVC environments don't have `pkg-config` by default.

### Error Encountered

```
Could NOT find PkgConfig (missing: PKG_CONFIG_EXECUTABLE)
```

### Solution

Use CMake's `find_path` and `find_library` directly:

```cmake
find_path(IPOPT_INCLUDE_DIR
    NAMES IpIpoptApplication.hpp
    PATH_SUFFIXES coin-or
)

find_library(IPOPT_LIBRARY NAMES ipopt)
```

## 4. Conda Environment Integration with CMake

### Problem

CMake needs to know where conda installed IPOPT.

### Solution

Use the `CONDA_PREFIX` environment variable:

```cmake
if(DEFINED ENV{CONDA_PREFIX})
    set(CONDA_PREFIX "$ENV{CONDA_PREFIX}")
    set(IPOPT_ROOT "${CONDA_PREFIX}/Library")
endif()
```

### Important

The conda environment must be activated before running CMake, otherwise `CONDA_PREFIX` won't be set.

## 5. Runtime Linear Solver Selection

### Problem

Even with MUMPS available, IPOPT may default to trying Pardiso first.

### Error Encountered

```
Error 126 while loading DLL libpardiso.dll: The specified module could not be found.
```

### Solutions

**Option A: Set in code (recommended)**

```cpp
app->Options()->SetStringValue("linear_solver", "mumps");
```

**Option B: Options file**

Create `ipopt.opt` in the working directory:

```
linear_solver mumps
```

### Lesson

Always explicitly set the linear solver to avoid runtime surprises.

## 6. DLL Dependencies

### Problem

The executable runs but crashes because IPOPT DLLs aren't in the path.

### Solution

Copy DLLs to the output directory via CMake:

```cmake
file(GLOB IPOPT_DLLS "${IPOPT_ROOT}/bin/*ipopt*.dll" "${IPOPT_ROOT}/bin/*mumps*.dll")
add_custom_command(TARGET test_ipopt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${IPOPT_DLLS}
        $<TARGET_FILE_DIR:test_ipopt>
)
```

## 7. TNLP Interface Requirements

### Key Methods

Every IPOPT problem must implement these `TNLP` virtual methods:

| Method               | Purpose                        |
| -------------------- | ------------------------------ |
| `get_nlp_info`       | Problem dimensions             |
| `get_bounds_info`    | Variable and constraint bounds |
| `get_starting_point` | Initial guess                  |
| `eval_f`             | Objective function value       |
| `eval_grad_f`        | Objective gradient             |
| `eval_g`             | Constraint values              |
| `eval_jac_g`         | Constraint Jacobian            |
| `eval_h`             | Hessian of Lagrangian          |
| `finalize_solution`  | Called with final solution     |

### Hessian Structure

The `eval_h` method is called twice:

1. First with `values == nullptr` to get the sparsity structure
2. Then with `values != nullptr` to get actual values

```cpp
bool eval_h(..., Number* values) override {
    if (values == nullptr) {
        // Return structure (row/column indices)
        iRow[0] = 0;
        jCol[0] = 0;
    } else {
        // Return values
        values[0] = obj_factor * 2.0;
    }
    return true;
}
```

## 8. Build System Recommendations

### For Windows + IPOPT + MUMPS

1. **Use conda-forge** - Only reliable source of pre-built IPOPT with MUMPS
2. **Single build script** - Handle conda activation, VS setup, and build in one script
3. **Check environment** - Verify conda environment is active before CMake
4. **Copy DLLs** - Automate DLL copying in CMake

### Script Pattern

```batch
REM Initialize conda
call "%CONDA_ROOT%\Scripts\activate.bat" "%CONDA_ROOT%"

REM Create environment if needed
conda info --envs | findstr /C:"env-name" >nul 2>&1
if errorlevel 1 conda create -n env-name -c conda-forge package -y

REM Activate environment
call conda activate env-name

REM Setup Visual Studio
call vcvars64.bat

REM Build
cmake -B build -S .
cmake --build build --config Release
```

## Summary

Getting IPOPT working on Windows with C++ requires:

1. **conda-forge** for package management (vcpkg lacks MUMPS)
2. **Explicit linear solver selection** (`linear_solver mumps`)
3. **Proper conda/CMake integration** via `CONDA_PREFIX`
4. **DLL management** for runtime dependencies
5. **Unified build script** for reproducible builds
