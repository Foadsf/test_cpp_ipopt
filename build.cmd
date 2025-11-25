@echo off
setlocal enabledelayedexpansion

REM === IPOPT Build Script ===
REM Uses conda-forge IPOPT with MUMPS

set "CONDA_ROOT=%USERPROFILE%\AppData\Local\miniconda3"
set "ENV_NAME=ipopt-dev"
set "BUILD_DIR=build"

REM Check conda installation
if not exist "%CONDA_ROOT%\Scripts\activate.bat" (
    echo ERROR: Miniconda not found at %CONDA_ROOT%
    exit /b 1
)

REM Initialize conda for this shell
call "%CONDA_ROOT%\Scripts\activate.bat" "%CONDA_ROOT%"

REM Check if environment exists
conda info --envs | findstr /C:"%ENV_NAME%" >nul 2>&1
if errorlevel 1 (
    echo.
    echo === Creating conda environment: %ENV_NAME% ===
    conda create -n %ENV_NAME% -c conda-forge ipopt -y
    if errorlevel 1 (
        echo ERROR: Failed to create conda environment
        exit /b 1
    )
)

REM Check if correct environment is active
if not "%CONDA_DEFAULT_ENV%"=="%ENV_NAME%" (
    echo.
    echo === Activating conda environment: %ENV_NAME% ===
    call conda activate %ENV_NAME%
    if errorlevel 1 (
        echo ERROR: Failed to activate conda environment
        exit /b 1
    )
)

echo Using conda environment: %CONDA_PREFIX%

REM Setup Visual Studio environment
where cl >nul 2>&1
if errorlevel 1 (
    echo.
    echo === Setting up Visual Studio environment ===
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    if errorlevel 1 (
        echo ERROR: Failed to setup Visual Studio environment
        exit /b 1
    )
)

REM Configure
echo.
echo === Configuring ===
cmake -B "%BUILD_DIR%" -S .
if errorlevel 1 (
    echo ERROR: CMake configure failed
    exit /b 1
)

REM Build
echo.
echo === Building ===
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

REM Run
echo.
echo === Running ===
"%BUILD_DIR%\Release\test_ipopt.exe"

endlocal