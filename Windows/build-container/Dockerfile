# ArangoDB 2025, all rights reserved
#
# ArangoDB build environment for Windows
#
# This Dockerfile creates a Windows container with the necessary tools to build ArangoDB on Windows. Build tools include Visual Studio 2022, CMake 3.31, LLVM 19.1.5, Python 3.13, Ninja, ccache, Node.js, Yarn, and MSYS2. The Vcpkg package manager is also installed and bootstrapped. The container is based on the official Windows image and uses Chocolatey as a package manager.
#
# To build container from oskar's root folder use:
#    docker build -t arangodb-windows-clang -f containers/buildWindows.docker/Dockerfile .
#
# To run the container (and start into PowerShell) use:
#    docker run -h windows-clang-mingw-buildhost -it arangodb-windows-clang
#
# IMPORTANT- specifyng being "host-platform-mingw-name" is important for the (icu-legacy) build scripts to work correctly!
#
# To start (latest) ArangoDB build use:
#   ./start_build.ps
#
# --------------------------------------------------------------------------------
# NOTE: Source by default must be cloned in a path with short path name, e.g.: c:\src
# or icu-legacy build will hit windows command line maximum limit(8191 characters)!!!
# Also PATH environment must first be set to bin64uwp and only then to bin64, otherwise
# the icu-legacy build will fail with being unable to run built tools.
# --------------------------------------------------------------------------------
#
# Use the official Windows image as a base
FROM mcr.microsoft.com/windows/servercore:ltsc2025

# Install Chocolatey package manager
RUN powershell -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "Set-ExecutionPolicy Bypass -Scope Process; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"

# Install packages required
RUN choco feature enable -n allowGlobalConfirmation

RUN choco install git
RUN choco install python311 --params "/NoLockdown /InstallDir:C:\\Python311"
RUN choco install ninja
RUN choco install ccache
RUN choco install nodejs
RUN choco install yarn

# Install Visual Studio Build Tools with the Windows SDK
# from https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-history
# Current buildtools version 17.14.2
RUN powershell -Command \
    Invoke-WebRequest -Uri https://download.visualstudio.microsoft.com/download/pr/d99c7868-44a2-4254-9861-78e21c19bd34/fa923c074d0a62d965f4dac9020924afbfe452c6ee29b294e227acd1913c0452/vs_BuildTools.exe -OutFile vs_buildtools.exe; \
    Start-Process -Wait -FilePath .\vs_buildtools.exe -ArgumentList '--quiet --wait --norestart --nocache --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows11SDK.26100 --add Microsoft.VisualStudio.Component.VC.Redist.14.Latest --add Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset --add Microsoft.VisualStudio.ComponentGroup.UWP.VC.BuildTools --add Microsoft.VisualStudio.ComponentGroup.UWP.BuildTools --add Microsoft.VisualStudio.Component.VC.Llvm.Clang --add Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang --add Microsoft.VisualStudio.Component.VC.CMake.Project'; \
    Remove-Item -Force vs_buildtools.exe

# Clone the Vcpkg repository
RUN git clone --verbose --progress https://github.com/microsoft/vcpkg.git C:\vcpkg

# Set the Vcpkg root environment variable (see vcpkg documentation page: https://vcpkg.io/en/getting-started.html )
ENV VCPKG_ROOT=C:\vcpkg
# Bootstrap Vcpkg
RUN C:\vcpkg\bootstrap-vcpkg.bat

# Set the Vcpkg toolchain file environment variable
ENV CMAKE_TOOLCHAIN_FILE="C:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"

# Silence git warnings
RUN git config --global --add safe.directory C:/build/*

# Set environment variables
ENV PATH="C:\\msys64\\mingw64\\bin;c:\\Program Files\\nodejs;c:\\Program Files (x86)\\Yarn\\bin;c:\\ProgramData\\chocolatey\\bin;C:\\Windows\\SysWOW64\\WindowsPowerShell\\v1.0;C:\\msys64\\usr\\bin;C:\\Python311;C:\\Program Files\\Git\\bin;C:\\Windows\\SysWOW64;c:\Windows;C:\\Program Files\\Git\\usr\\bin;c:\src\3rdParty\icu-legacy\bin64uwp;c:\src\3rdParty\icu-legacy\bin64;${PATH}"

# Copy start_build.ps1 / setvar scripts to the container
COPY *.ps1 /
COPY *.bat /

SHELL ["C:\\Windows\\SysWOW64\\cmd.exe","/S","/C"]

# Set the working directory where the build will be done
WORKDIR /

ENTRYPOINT ["cmd", "/S", "/K", "c:\\setvar.bat"]
