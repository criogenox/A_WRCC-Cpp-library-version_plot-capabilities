# Helper function to enable sanitizers on a target
function(wrcc_enable_sanitizers TARGET_NAME)
    option(ENABLE_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)
    option(ENABLE_SANITIZER_UNDEFINED "Enable UndefinedBehaviorSanitizer" OFF)

    set(SANITIZERS "")

    if (ENABLE_SANITIZER_ADDRESS)
        list(APPEND SANITIZERS "address")
    endif ()

    if (ENABLE_SANITIZER_UNDEFINED)
        list(APPEND SANITIZERS "undefined")
    endif ()

    list(JOIN SANITIZERS "," SANITIZER_LIST)

    if (SANITIZER_LIST AND NOT MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=${SANITIZER_LIST} -fno-omit-frame-pointer)
        target_link_options(${TARGET_NAME} PRIVATE -fsanitize=${SANITIZER_LIST})
    endif ()
endfunction()
