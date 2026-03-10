cmake_minimum_required(VERSION 3.16)
project(enlace-escuela-libreria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Qt ──────────────────────────────────────────────────────────────
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network)

# ── PostgreSQL (libpq) ───────────────────────────────────────────────
# Cambia "16" por tu version instalada si es diferente
set(PG_ROOT "C:/Program Files/PostgreSQL/18")

# ── Fuentes ─────────────────────────────────────────────────────────
set(TS_FILES enlace-escuela-libreria_es_MX.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        CsvUtils.cpp
        CsvUtils.h
        DatabaseManager.cpp
        DatabaseManager.h
        Escuela.cpp
        Escuela.h
        Inventario.cpp
        Inventario.h
        Libro.cpp
        Libro.h
        SistemaGestion.cpp
        SistemaGestion.h
        Vale.cpp
        Vale.h
        ${TS_FILES}
)

# ── Ejecutable ──────────────────────────────────────────────────────
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(enlace-escuela-libreria
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(enlace-escuela-libreria SHARED ${PROJECT_SOURCES})
    else()
        add_executable(enlace-escuela-libreria ${PROJECT_SOURCES})
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# ── Includes ────────────────────────────────────────────────────────
target_include_directories(enlace-escuela-libreria PRIVATE
    "${PG_ROOT}/include"
)

# ── Links ───────────────────────────────────────────────────────────
target_link_libraries(enlace-escuela-libreria PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    "${PG_ROOT}/lib/libpq.lib"
)

# ── Propiedades del ejecutable ──────────────────────────────────────
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.enlace-escuela-libreria)
endif()

set_target_properties(enlace-escuela-libreria PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ── Copiar libpq.dll al directorio de build automaticamente ─────────
add_custom_command(TARGET enlace-escuela-libreria POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PG_ROOT}/bin/libpq.dll"
        $<TARGET_FILE_DIR:enlace-escuela-libreria>
    COMMENT "Copiando libpq.dll al directorio de build"
)

# ── Instalar ────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS enlace-escuela-libreria
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(enlace-escuela-libreria)
endif()