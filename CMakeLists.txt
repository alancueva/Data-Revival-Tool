cmake_minimum_required(VERSION 3.10)
project(MetadataRecovery)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Añadir directorios de cabeceras
include_directories(
    ${GTK3_INCLUDE_DIRS}
    include           
)

add_definitions(${GTK3_CFLAGS_OTHER})

# Recopila todos los .cpp
set(SOURCES
    src/main.cpp
    src/metadata/AudioMetadata.cpp
    src/metadata/DocumentMetadata.cpp
    src/metadata/FileMetadata.cpp
    src/metadata/ImageMetadata.cpp
    src/metadata/MetadataFactory.cpp
    src/core/MetadataRecoveryEngine.cpp
    # src/core/MetadataRecovery.cpp
    src/ui/MetadataRecoveryGUI.cpp
)


# Crear ejecutable
add_executable(metadata_recovery ${SOURCES})

# Enlazar con GTK
target_link_libraries(metadata_recovery ${GTK3_LIBRARIES})

# Especificar la carpeta de salida del ejecutable
set_target_properties(metadata_recovery PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Opcional: Para ver información útil
message(STATUS "GTK3_INCLUDE_DIRS: ${GTK3_INCLUDE_DIRS}")
message(STATUS "GTK3_LIBRARIES: ${GTK3_LIBRARIES}")