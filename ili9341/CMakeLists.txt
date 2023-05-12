# Generated Cmake Pico project file




# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
## Uncomment and set custom path if needed
# set(PICO_SDK_PATH "$(set_path_here)/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)



# Initialise the Raspberry Pi Pico SDK


# Add executable. Default name is the project name, version 0.1
add_subdirectory(include/FatFs_SPI build)

add_executable(PocketGamer 
        mode0_demo.c
        ili9341.c
        # interface/ili9341_touchscreen.c
        gameshared.c
        mode0.c
        interface.c
        mainloop.c
        menu.c
        # textures.c
        games/game01.c
        # games/game02.c
        games/settings.c
        games/home.c
        games/HC05.c
        games/files.c
        hw_config.c
        #sdcard_console_v1.c
        # games/game03.c
        # games/game04.c
        # games/game05.c
        # games/game06.c
        # games/game07.c
        # games/game08.c
        # games/game09.c
        # games/game10.c
        # games/game11.c
        # games/game12.c
        # games/game13.c
        # games/game14.c
        )

pico_set_program_name(PocketGamer "PocketGamer")
pico_set_program_version(PocketGamer "0.1")

pico_enable_stdio_uart(PocketGamer 0)
pico_enable_stdio_usb(PocketGamer 1)

# Add the standard library to the build
target_link_libraries(PocketGamer pico_stdlib     FatFs_SPI )

# Add any user requested libraries
target_link_libraries(PocketGamer
        hardware_spi
        hardware_dma
        )

pico_add_extra_outputs(PocketGamer)





# add_library(ili9341
# 	font.c
# 	ili9341.c
# )
# target_link_libraries(ili9341 pico_stdlib hardware_spi)
# target_include_directories(ili9341 PUBLIC include)


# # MODE 1 (SPRITE GRAPHICS)
# add_library(mode1
# 	mode1.c
# )

# target_link_libraries(mode1 ili9341 pico_stdlib hardware_spi)
# target_include_directories(mode1 PUBLIC include)

# add_executable(mode1_demo
#     mode1_demo.c
# )

# pico_enable_stdio_usb(mode1_demo 1)
# pico_add_extra_outputs(mode1_demo)
# target_link_libraries(mode1_demo mode1 pico_stdlib hardware_spi)


# # MODE 2 (FRAMEBUFFER)
# add_library(mode2
# 	mode2.c
# )

# target_link_libraries(mode2 ili9341 pico_stdlib hardware_spi)
# target_include_directories(mode2 PUBLIC include)

# add_executable(mode2_demo
# 	mode2_demo.c
# )

# pico_enable_stdio_usb(mode2_demo 1)
# pico_add_extra_outputs(mode2_demo)
# target_link_libraries(mode2_demo mode2 pico_stdlib hardware_spi)


# # MODE 0 (TEXT)
# add_library(mode0
# 	mode0.c
# 	gameshared.c
# 	interface.c
# 	mainloop.c
# 	menu.c
# )
# target_link_libraries(mode0 ili9341 pico_stdlib hardware_spi)
# target_include_directories(mode0 PUBLIC include)

# add_executable(mode0_demo
#     mode0_demo.c
# 	games/game01.c
# )

# pico_enable_stdio_usb(mode0_demo 1)
# pico_add_extra_outputs(mode0_demo)
# target_link_libraries(mode0_demo mode0 pico_stdlib pico_mem_ops hardware_spi)
