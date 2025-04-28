# Instalación de MSYS2 y GTK3 para C++
Para compilar y ejecutar aplicaciones C++ con GTK3 en Windows, sigue estos pasos:

1. **Descarga e instala MSYS2**  
   Ve a [https://www.msys2.org](https://www.msys2.org) y descarga el instalador adecuado para tu sistema operativo. Sigue las instrucciones de la página para completar la instalación.

2. **Actualiza los paquetes base**  
   Abre la terminal de MSYS2 (puedes buscar "MSYS2 MSYS" en el menú de inicio) y ejecuta el siguiente comando para actualizar el sistema:
   ```
   pacman -Syu
   ```

3. **Instala GTK3 para C++ y pkg-config**  
   Una vez actualizado el sistema, instala los paquetes necesarios ejecutando:
   ```
   pacman -S mingw-w64-x86_64-gtk3
   pacman -S pkg-config
   pacman -S mingw-w64-x86_64-gcc
   ```

Esto instalará las bibliotecas necesarias para desarrollar aplicaciones gráficas en C++ usando GTK3 y la herramienta `pkg-config` en el entorno MSYS2.
