add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PUBLIC FILE_SET headers TYPE HEADERS FILES
		Bitmap.hpp
		DrawMode.hpp
		Font.hpp
		Menu.hpp
		SSD130x.hpp
		font/tahoma_8pt.hpp
	PRIVATE
		Bitmap.cpp
		Font.cpp
		Menu.cpp
		SSD130x.cpp
		font/tahoma_8pt.cpp
)

if(${PLATFORM} STREQUAL "native")
	# native platform (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/native FILES
			native/coco/platform/SSD130x_cout.hpp
		PRIVATE
			native/coco/platform/SSD130x_cout.cpp
	)
elseif(${PLATFORM} STREQUAL "emu")
	# emulator platform with graphical user interface (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/emu FILES
			emu/coco/platform/Display.hpp
			emu/coco/platform/SSD130x_emu.hpp
		PRIVATE
			emu/coco/platform/Display.cpp
			emu/coco/platform/SSD130x_emu.cpp
	)
endif()

target_link_libraries(${PROJECT_NAME}
	coco::coco
	coco-loop::coco-loop
	coco-buffer::coco-buffer
	coco-io::coco-io
	coco-qdec::coco-qdec
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		..
)

# install the library
install(TARGETS ${PROJECT_NAME}
	FILE_SET headers DESTINATION include/coco
	FILE_SET platform_headers DESTINATION include
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
