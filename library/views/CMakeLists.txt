

set(CURRENT_TARGET dooc.lib.views)

add_library(${CURRENT_TARGET} INTERFACE)
add_library(dooc::${CURRENT_TARGET} ALIAS ${CURRENT_TARGET})

target_include_directories(${CURRENT_TARGET}
		INTERFACE
		public_include
)
