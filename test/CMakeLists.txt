# hex generator
separate_arguments(GENERATE_HEX)
macro(generate_hex TARGET)
	if(NOT "${GENERATE_HEX}" STREQUAL "" AND NOT "${GENERATE_HEX}" STREQUAL "None")
		#message("*** Generate Hex for ${TARGET}: ${GENERATE_HEX}")
		add_custom_command(TARGET ${TARGET}
			POST_BUILD
			COMMAND ${GENERATE_HEX} ${TARGET} ${TARGET}.hex
		)
	endif()
endmacro()

# iterate over boards and generate tests
foreach(COMPONENT ${coco-devboards_COMPONENT_NAMES})
	string(REGEX REPLACE ".*\\:" "" BOARD ${COMPONENT})
	message("*** Board: ${BOARD}")

	# SSD130xTest: check if board specific header exists
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}/SSD130xTest.hpp)
		set(NAME "SSD130xTest-${BOARD}")

		add_executable(${NAME}
			SSD130xTest.cpp
		)
		target_include_directories(${NAME}
			PRIVATE
				../
				${BOARD}
		)

		# find link.ld of board
		get_target_property(INCLUDES ${COMPONENT} INTERFACE_INCLUDE_DIRECTORIES)
		target_link_directories(${NAME}
			PRIVATE
				${INCLUDES}
		)

		target_link_libraries(${NAME}
			coco::coco
			coco-loop::coco-loop
			${PROJECT_NAME}
			${COMPONENT}
		)

		# generate hex file for flashing the target
		generate_hex(${NAME})
	endif()

	# MenuTest: check if board specific header exists
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}/MenuTest.hpp)
		set(NAME "MenuTest-${BOARD}")

		add_executable(${NAME}
			MenuTest.cpp
		)
		target_include_directories(${NAME}
			PRIVATE
				../
				${BOARD}
		)

		# find link.ld of board
		get_target_property(INCLUDES ${COMPONENT} INTERFACE_INCLUDE_DIRECTORIES)
		target_link_directories(${NAME}
			PRIVATE
				${INCLUDES}
		)

		target_link_libraries(${NAME}
			coco::coco
			coco-loop::coco-loop
			coco-qdec::coco-qdec
			${PROJECT_NAME}
			${COMPONENT}
		)

		# generate hex file for flashing the target
		generate_hex(${NAME})
	endif()
endforeach()


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
