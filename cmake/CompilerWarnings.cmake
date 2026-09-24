# Helper function to enable recommended compiler warnings on a target
function(wrcc_set_target_warnings TARGET_NAME)
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
                /W4
                /permissive-
                /w14242
                /w14254
                /w14263
                /w14265
                /w14287
                /we4289
                /w14296
                /w14311
                /w14545
                /w14546
                /w14547
                /w14549
                /w14555
                /w14619
                /w14640
                /w14826
                /w14905
                /w14906
                /w14928
        )
    else ()
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wno-unused-parameter
                -Wconversion
                -Wsign-conversion
                -Wnull-dereference
                -Wdouble-promotion
                -Wformat=2
        )
        # Relax conversions to non-fatal warning to avoid stopping build on existing math conversions
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wno-sign-conversion
                -Wno-conversion
        )
    endif ()
endfunction()
