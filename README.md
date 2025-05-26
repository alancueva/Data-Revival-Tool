# Data Revival Tool

---

## ⚙️ Instalación en Windows

### 1. Instalar MSYS2
- Descarga el instalador desde [msys2.org](https://www.msys2.org) y sigue las instrucciones de la página.

### 2. Actualizar Paquetes Base
Abre la terminal de MSYS2 y ejecuta:
```sh
pacman -Syu
```

### 3. Instalar Dependencias
Ejecuta los siguientes comandos en la terminal de MSYS2:
```sh
pacman -S mingw-w64-x86_64-gtk3 \
           pkg-config \
           mingw-w64-x86_64-gcc \
           mingw-w64-x86_64-cmake \
           mingw-w64-x86_64-file
```
Esto instalará las bibliotecas necesarias para desarrollar aplicaciones gráficas en C++ usando GTK3 y la herramienta `pkg-config`.

### 4. Compilar y Ejecutar el Programa
```sh
cd build
cmake ..
cmake --build .
cd ..
bin/metadata_recovery.exe
```
- Entra al directorio de compilación
- Compila el proyecto usando CMake
- Vuelve al directorio principal
- Ejecuta el programa compilado

---

## 🐧 Instalación en Linux (Fedora)

### 1. Instalar Dependencias
```sh
sudo dnf install gcc-c++ cmake pkgconf gtk3-devel
```

### 2. Compilar y Ejecutar
```sh
cd build
cmake ..
cmake --build .
cd ..
./bin/metadata_recovery
```

> **Nota:** Asegúrate de tener las dependencias instaladas antes de compilar el proyecto.
