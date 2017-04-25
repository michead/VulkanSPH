#pragma once

// Magma display name
#define MAGMA_DISPLAY_NAME  "Magma"

// Magma version number
#define MAGMA_VERSION       1

// Fatal error code
#define MAGMA_FATAL         1

// Macro expansion
#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

// Config file
#define MAGMA_CONFIG_FILE EXPAND(MAGMA_ROOT_DIR)"magma.ini"
