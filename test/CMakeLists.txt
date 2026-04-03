# Generate a test for a board
# TEST the test application, implemented in ${TEST}.cpp
# BOARD_LIB a library for a board containing SystemInit() and a linker script for embedded platforms
function(board_test TEST BOARD_LIB)
    # check if board library exists for the current platform
    if(TARGET ${BOARD_LIB})
        string(REGEX REPLACE ".*\\:" "" BOARD ${BOARD_LIB})
        set(NAME "${TEST}-${BOARD}")
        message("*** Board: ${BOARD}")

        add_executable(${NAME}
            ${TEST}.cpp
        )
        target_include_directories(${NAME}
            PRIVATE
                ../
                ${BOARD}
        )
        target_link_libraries(${NAME}
            coco-spi::coco-spi
            coco-i2c::coco-i2c
            ${BOARD_LIB}
            ${PROJECT_NAME}
        )

        # generate hex file for flashing the target
        if(${CMAKE_CROSSCOMPILING})
            #message("*** Generate Hex for ${NAME} using ${CMAKE_OBJCOPY}")
            add_custom_command(TARGET ${NAME}
                POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O ihex ${NAME} ${NAME}.hex
            )
        endif()
    endif()
endfunction()

# LedControl board
if(${PLATFORM} STREQUAL "stm32g431xx")
    add_library(LedControl-g431
        LedControl-g431/debug.cpp
        LedControl-g431/SystemInit.cpp
    )

    # inherit from nucleo-g431rb
    target_link_libraries(LedControl-g431
        coco::coco
        coco-loop::coco-loop
    )

    target_link_options(LedControl-g431
        PUBLIC
            "-T${CMAKE_CURRENT_SOURCE_DIR}/LedControl-g431/link.ld"
    )
endif()

board_test(SSD1306Test coco-devboards::native)
board_test(SSD1306Test coco-devboards::emu)
board_test(SSD1306Test coco-devboards::nucleo-g431rb)

board_test(SSD1309Test coco-devboards::native)
board_test(SSD1309Test coco-devboards::emu)
board_test(SSD1309Test coco-devboards::nucleo-g431rb)
board_test(SSD1309Test LedControl-g431)

board_test(MenuTest coco-devboards::emu)
board_test(MenuTest LedControl-g431)


# unit tests for "normal" operating systems
if(NOT ${CMAKE_CROSSCOMPILING})
    add_executable(gTest
        gTest.cpp
    )
    target_include_directories(gTest
        PRIVATE
        ..
    )
    target_link_libraries(gTest
        ${PROJECT_NAME}
        GTest::gtest
    )

    add_test(NAME gTest
        COMMAND gTest --gtest_output=xml:report.xml
        #WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../testdata
    )
endif()
