cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 12)

if(BUILD_PSP2)
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else ()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif ()
endif ()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_DEBUG true CACHE BOOL "Build with psp2shell support")
endif()
endif(BUILD_PSP2)

if(BUILD_NX)
    set(CMAKE_SYSTEM_NAME "Generic")
    set(DEVKITPRO $ENV{DEVKITPRO})
    set(CMAKE_SYSTEM_PROCESSOR "armv8-a")
    set(CMAKE_C_COMPILER "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc")
    set(CMAKE_CXX_COMPILER "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-g++")
    set(CMAKE_ASM_COMPILER "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-as")
    set(CMAKE_AR "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc-ar" CACHE STRING "")
    set(CMAKE_RANLIB "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc-ranlib" CACHE STRING "")
    set(CMAKE_FIND_ROOT_PATH ${DEVKITPRO} ${DEVKITPRO}/devkitA64 ${DEVKITPRO}/libnx ${DEVKITPRO}/portlibs/switch)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available")
    link_directories(${DEVKITPRO}/libnx/lib ${DEVKITPRO}/portlibs/switch/lib)
endif(BUILD_NX)

execute_process(COMMAND "date" "+%Y.%m.%d"
        OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)

enable_language(ASM)

project(enigma)

if(BUILD_PSP2)
  enable_language(ASM)

  include("${VITASDK}/share/vita.cmake" REQUIRED)
endif(BUILD_PSP2)

set(TITLEID "ENIGMA000")

set(SRC_FILES
    src/psp2/psp2_input.c
        src/AttributeDescriptor.cc
        src/DOMErrorReporter.cc
        src/DOMSchemaResolver.cc
        src/GridObject.cc
        src/Inventory.cc
        src/KindDescriptor.cc
        src/LocalToXML.cc
        src/MessageDescriptor.cc
        src/MusicManager.cc
        src/Object.cc
        src/ObjectValidator.cc
        src/PreferenceManager.cc
        src/PropertyManager.cc
        src/SoundEffectManager.cc
        src/SoundEngine.cc
        src/StateManager.cc
        src/StateObject.cc
        src/Utf8ToXML.cc
        src/Value.cc
        src/WorldProxy.cc
        src/XMLtoLocal.cc
        src/XMLtoUtf8.cc
        src/actors.cc
        src/client.cc
        src/d_follower.cc
        src/d_models.cc
        src/display.cc
        src/enigma.cc
        src/file.cc
        src/file_zip.cc
        src/floors.cc
        src/game.cc
        src/items.cc
        src/laser.cc
        src/lua-display.cc
        src/lua-ecl.cc
        src/lua-enigma.cc
        src/lua-global.cc
        src/lua.cc
        src/main.cc
        src/nls.cc
        src/options.cc
        src/ox_extra.cc
        src/ox_magnum.cc
        src/ox_oxyd1.cc
        src/ox_peroxyd.cc
        src/oxyd.cc
        src/player.cc
        src/resource_cache.cc
        src/server.cc
        src/stones.cc
        src/timer.cc
        src/utilXML.cc
        src/video.cc
        src/world.cc
        src/actors/Balls.cc
        src/actors/BugActor.cc
        src/actors/HorseActor.cc
        src/actors/KillerActor.cc
        src/actors/Rotors.cc
        src/others/CannonBall.cc
        src/others/CounterGadget.cc
        src/others/Other.cc
        src/others/Rubberband.cc
        src/others/TimerGadget.cc
        src/others/Wire.cc
        src/floors/BridgeFloor.cc
        src/floors/FloodStream.cc
        src/floors/ScalesFloor.cc
        src/floors/SimpleFloors.cc
        src/floors/SlopeFloor.cc
        src/floors/StandardFloors.cc
        src/floors/ThiefFloor.cc
        src/stones/ActorImpulseStone.cc
        src/stones/BarrierStone.cc
        src/stones/BlockerStone.cc
        src/stones/BoulderStone.cc
        src/stones/BoxStone.cc
        src/stones/BrakeStone.cc
        src/stones/BreakStone.cc
        src/stones/ChameleonStone.cc
        src/stones/ChessStone.cc
        src/stones/ClusterStone.cc
        src/stones/CoinSlot.cc
        src/stones/CompatibilityStones.cc
        src/stones/DeathStone.cc
        src/stones/DiscoStone.cc
        src/stones/DispenserStone.cc
        src/stones/Door.cc
        src/stones/FakeStone.cc
        src/stones/FlatStone.cc
        src/stones/FloppySwitch.cc
        src/stones/FourSwitch.cc
        src/stones/GhostStone.cc
        src/stones/GlassStones.cc
        src/stones/GreenBrownStone.cc
        src/stones/IceBlock.cc
        src/stones/JambStone.cc
        src/stones/KeySwitch.cc
        src/stones/KnightStone.cc
        src/stones/LaserStone.cc
        src/stones/LaserSwitch.cc
        src/stones/LightPassengerStone.cc
        src/stones/MagicStone.cc
        src/stones/MailStone.cc
        src/stones/MirrorStone.cc
        src/stones/MonoFlopStone.cc
        src/stones/OneWayStone.cc
        src/stones/OxydStone.cc
        src/stones/PassageStone.cc
        src/stones/PlasterStone.cc
        src/stones/PolarSwitchStone.cc
        src/stones/PortalStone.cc
        src/stones/PullStone.cc
        src/stones/PuzzleStone.cc
        src/stones/QuakeStone.cc
        src/stones/RotatorStone.cc
        src/stones/RubberbandStone.cc
        src/stones/ScissorsStone.cc
        src/stones/ShogunStone.cc
        src/stones/SimpleStones.cc
        src/stones/SpitterStone.cc
        src/stones/StandardStones.cc
        src/stones/StoneImpulse.cc
        src/stones/SurpriseStone.cc
        src/stones/SwapStone.cc
        src/stones/Switch.cc
        src/stones/ThiefStone.cc
        src/stones/TimerStone.cc
        src/stones/Turnstile.cc
        src/stones/VolcanoStone.cc
        src/stones/WindowStone.cc
        src/stones/YieldingStone.cc
        src/gui/ErrorMenu.cc
        src/gui/GameMenu.cc
        src/gui/HelpMenu.cc
        src/gui/InfoMenu.cc
        src/gui/LPGroupConfig.cc
        src/gui/LevelInspector.cc
        src/gui/LevelMenu.cc
        src/gui/LevelPackComposer.cc
        src/gui/LevelPackConfig.cc
        src/gui/LevelPackMenu.cc
        src/gui/LevelPreviewCache.cc
        src/gui/LevelWidget.cc
        src/gui/MainMenu.cc
        src/gui/Menu.cc
        src/gui/MonospacedLabel.cc
        src/gui/OptionsMenu.cc
        src/gui/ScreenshotViewer.cc
        src/gui/SearchMenu.cc
        src/gui/TextField.cc
        src/gui/widgets.cc
        src/items/BagItem.cc
        src/items/BlockerItem.cc
        src/items/Bomb.cc
        src/items/Bottle.cc
        src/items/BrakeItem.cc
        src/items/BurnableItem.cc
        src/items/Coin.cc
        src/items/CompatibilityItems.cc
        src/items/Crack.cc
        src/items/CrossItem.cc
        src/items/DocumentItem.cc
        src/items/Drop.cc
        src/items/Dynamite.cc
        src/items/Extinguisher.cc
        src/items/ExtraLife.cc
        src/items/GlassesItem.cc
        src/items/Hammer.cc
        src/items/Landmine.cc
        src/items/Magnet.cc
        src/items/Meditation.cc
        src/items/PipeItem.cc
        src/items/PullerItem.cc
        src/items/RubberbandItem.cc
        src/items/SeedItem.cc
        src/items/Sensor.cc
        src/items/ShogunDot.cc
        src/items/SimpleItems.cc
        src/items/StripItem.cc
        src/items/SurpriseItem.cc
        src/items/Sword.cc
        src/items/TrapItem.cc
        src/items/Trigger.cc
        src/items/Umbrella.cc
        src/items/Vortex.cc
        src/items/WormHole.cc
        src/lev/Index.cc
        src/lev/PersistentIndex.cc
        src/lev/Proxy.cc
        src/lev/RatingManager.cc
        src/lev/ScoreManager.cc
        src/lev/SubProxy.cc
        src/lev/VolatileIndex.cc
        lib-src/lua/lapi.c
        lib-src/lua/lauxlib.c
        lib-src/lua/lbaselib.c
        lib-src/lua/lcode.c
        lib-src/lua/ldblib.c
        lib-src/lua/ldebug.c
        lib-src/lua/ldo.c
        lib-src/lua/ldump.c
        lib-src/lua/lfunc.c
        lib-src/lua/lgc.c
        lib-src/lua/linit.c
        lib-src/lua/liolib.c
        lib-src/lua/llex.c
        lib-src/lua/lmathlib.c
        lib-src/lua/lmem.c
        lib-src/lua/loadlib.c
        lib-src/lua/lobject.c
        lib-src/lua/lopcodes.c
        lib-src/lua/loslib.c
        lib-src/lua/lparser.c
        lib-src/lua/lstate.c
        lib-src/lua/lstring.c
        lib-src/lua/lstrlib.c
        lib-src/lua/ltable.c
        lib-src/lua/ltablib.c
        lib-src/lua/ltm.c
        lib-src/lua/lundump.c
        lib-src/lua/lvm.c
        lib-src/lua/lzio.c
        lib-src/lua/tolua_event.c
        lib-src/lua/tolua_is.c
        lib-src/lua/tolua_map.c
        lib-src/lua/tolua_push.c
        lib-src/lua/tolua_to.c
        lib-src/zipios++/src/basicentry.cpp
        lib-src/zipios++/src/collcoll.cpp
        lib-src/zipios++/src/dircoll.cpp
        lib-src/zipios++/src/directory.cpp
        lib-src/zipios++/src/fcoll.cpp
        lib-src/zipios++/src/fcollexceptions.cpp
        lib-src/zipios++/src/fileentry.cpp
        lib-src/zipios++/src/filepath.cpp
        lib-src/zipios++/src/filterinputstreambuf.cpp
        lib-src/zipios++/src/filteroutputstreambuf.cpp
        lib-src/zipios++/src/deflateoutputstreambuf.cpp
        lib-src/zipios++/src/gzipoutputstream.cpp
        lib-src/zipios++/src/gzipoutputstreambuf.cpp
        lib-src/zipios++/src/inflateinputstreambuf.cpp
        lib-src/zipios++/src/zipfile.cpp
        lib-src/zipios++/src/ziphead.cpp
        lib-src/zipios++/src/zipheadio.cpp
        lib-src/zipios++/src/zipinputstream.cpp
        lib-src/zipios++/src/zipinputstreambuf.cpp
        lib-src/zipios++/src/zipoutputstream.cpp
        lib-src/zipios++/src/zipoutputstreambuf.cpp
        lib-src/enigma-core/IMG_SavePNG.c
        lib-src/enigma-core/SDL_gfxPrimitives.c
        lib-src/enigma-core/SDL_rotozoom.c
        lib-src/enigma-core/ecl_argp.cc
        lib-src/enigma-core/ecl_buffer.cc
        lib-src/enigma-core/ecl_dict.cc
        lib-src/enigma-core/ecl_font.cc
        lib-src/enigma-core/ecl_geom.cc
        lib-src/enigma-core/ecl_math.cc
        lib-src/enigma-core/ecl_sdl.cc
        lib-src/enigma-core/ecl_sys_localename.cc
        lib-src/enigma-core/ecl_system.cc
        lib-src/enigma-core/ecl_utf.cc
        lib-src/enigma-core/ecl_util.cc
        lib-src/enigma-core/ecl_video.cc
        lib-src/oxydlib/Bitmap.cpp
        lib-src/oxydlib/DatFile.cpp
        lib-src/oxydlib/FileUtils.cpp
        lib-src/oxydlib/Level.cpp
        lib-src/oxydlib/VecUtils.cpp
)

include_directories(
        include
        src
        src/psp2
        src/switch
        src/actors
        src/floors
        src/stones
        src/gui
        src/items
        src/lev
        lib-src
        lib-src/lua
        lib-src/zipios++
        lib-src/zipios++/zipios++
        lib-src/zipios++/src
        lib-src/enigma-core
        lib-src/oxydlib
)

if(BUILD_PSP2)
  set(PSP2_SRC_FILES
    src/psp2/psp2-dirent.c
    src/psp2/psp2_touch.c
    src/psp2/psp2_kbdvita.c
    src/net_enet.cc
    src/netgame.cc
    src/network.cc
    lib-src/vita-enet/callbacks.c
    lib-src/vita-enet/host.c
    lib-src/vita-enet/list.c
    lib-src/vita-enet/packet.c
    lib-src/vita-enet/peer.c
    lib-src/vita-enet/protocol.c
    lib-src/vita-enet/vita.c
  )  
  set(SRC_FILES ${PSP2_SRC_FILES} ${SRC_FILES})

  include_directories(
    ${VITASDK}/arm-vita-eabi/include/
    ${VITASDK}/arm-vita-eabi/include/SDL
    ${VITASDK}/arm-vita-eabi/include/xercesc
    lib-src/vita-enet/include
  )

set(FLAGS
        -std=c++11
        -std=c11
        -Wno-write-strings
        -DSYSTEM_DATA_DIR="ux0:/data/enigma/data"
        -DDOCDIR="ux0:/data/enigma/doc"
        -DLOCALEDIR="ux0:/data/enigma/locale"
#        -DCXXLUA
        -DHAVE_STDINT_H
        -DHAVE_STDLIB_H
        -DHAVE_STRING_H
        -DHAVE_LIBXERCES_C
        -DHAVE_LIBSDL_TTF
        -DHAVE_DIRENT_H
        -DHAVE_SYS_TYPES_H
        -DPACKAGE="enigma"
        -DHAS_SOCKLEN_T
        -DPACKAGE_NAME="enigma"
        -DPACKAGE_STRING="enigma 1.21"
        -DPACKAGE_TARNAME="enigma"
        -DPACKAGE_URL=""
        -DPACKAGE_VERSION="1.21"
        -O3
        -marm -mfpu=neon -mcpu=cortex-a9 -march=armv7-a -mfloat-abi=hard -ffast-math
        -fno-asynchronous-unwind-tables -funroll-loops
        -mword-relocations -fno-unwind-tables -fno-optimize-sibling-calls
        -mvectorize-with-neon-quad -funsafe-math-optimizations
        -mlittle-endian -munaligned-access
        )

set(LDFLAGS
        SDL_image SDL_ttf SDL_mixer SDL
#        ${CMAKE_SOURCE_DIR}/src/psp2/vita2d_fbo/lib/libvita2d.a
#        ${CMAKE_SOURCE_DIR}/src/psp2/vita-shader-collection/lib/libvitashaders.a
        vita2d
        xerces-c
        curl
        SceSysmodule_stub SceDisplay_stub SceGxm_stub SceAppUtil_stub
        SceCtrl_stub ScePgf_stub ScePower_stub SceCommonDialog_stub
        SceAudio_stub SceShellSvc_stub 
        SceHid_stub
        SceTouch_stub
        png jpeg z m c
        vorbisfile
        vorbis
        ogg
        freetype
        ssl
        crypto
        mikmod
        )

if (BUILD_DEBUG)
    list(APPEND FLAGS -DDEBUG)
    list(APPEND LDFLAGS psp2shell SceSysmodule_stub SceNetCtl_stub
            SceNet_stub SceAppUtil_stub SceAppMgr_stub
            )
    set(UNSAFE_FLAG UNSAFE)
else ()
    list(APPEND FLAGS -O3)
endif (BUILD_DEBUG)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME} ${LDFLAGS})

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} ${UNSAFE_FLAG})
add_custom_target(${PROJECT_NAME}.vpk
        DEPENDS ${PROJECT_NAME}.self
        COMMAND rm -rf vpk && mkdir -p vpk/sce_sys
        COMMAND ${VITASDK}/bin/vita-mksfoex -s TITLE_ID=${TITLEID} "${PROJECT_NAME}" vpk/sce_sys/param.sfo
        COMMAND cp ${PROJECT_NAME}.self vpk/eboot.bin
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/psp2data/* vpk
#        COMMAND cp -r ${CMAKE_SOURCE_DIR}/data vpk/data
        COMMAND cd vpk && zip -r ../enigma.vpk . && cd ..
        )
endif(BUILD_PSP2)

if(BUILD_NX)
  set(NX_SRC_FILES
    src/switch/switch_touch.c
    src/switch/switch_kbd.c
    src/switch/sdl2_to_sdl1.cpp
  )
  set(SRC_FILES ${NX_SRC_FILES} ${SRC_FILES})

  include_directories(
    ${DEVKITPRO}/portlibs/switch/include/SDL2
    ${DEVKITPRO}/libnx/include
    ${DEVKITPRO}/portlibs/switch/include
  )

  add_definitions(
    -D__SWITCH__
    -DUSE_SDL2
    -DSYSTEM_DATA_DIR="/switch/enigma/data"
    -DDOCDIR="/switch/enigma/doc"
    -DLOCALEDIR="/switch/enigma/locale"
  #        -DCXXLUA
    -DHAVE_STDINT_H
    -DHAVE_STDLIB_H
    -DHAVE_STRING_H
    -DHAVE_LIBXERCES_C
    -DHAVE_LIBSDL_TTF
    -DHAVE_DIRENT_H
    -DHAVE_SYS_TYPES_H
    -DPACKAGE="enigma"
    -DHAS_SOCKLEN_T
    -DPACKAGE_NAME="enigma"
    -DPACKAGE_STRING="enigma 1.21"
    -DPACKAGE_TARNAME="enigma"
    -DPACKAGE_URL=""
    -DPACKAGE_VERSION="1.21"
  )

  set(FLAGS
    -Wno-write-strings
    -march=armv8-a
    -mtune=cortex-a57
    -mtp=soft
#-fPIE is neccessary to prevent crash on startup
    -fPIE
    -O3
    -ffast-math
    -fno-asynchronous-unwind-tables
    -funroll-loops
    -fno-unwind-tables
    -fno-optimize-sibling-calls
    -funsafe-math-optimizations
    -mlittle-endian
    -ffunction-sections
  )

  set(LIBS
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    SDL2 
    EGL
    GLESv2
    glapi
    drm_nouveau
    stdc++
    xerces-c
    curl
    png
    jpeg
    z
    m
    c
    vorbisidec
    ogg
    modplug
    mpg123
    freetype
    ssl
    crypto
    bz2
    nx
  )

  foreach(flag ${FLAGS})
    set(FLAGS_GENERAL "${FLAGS_GENERAL} ${flag}")
  endforeach(flag ${FLAGS})

  #set(CMAKE_C_FLAGS "${FLAGS_GENERAL}" CACHE STRING "C Flags" FORCE)
  #set(CMAKE_CXX_FLAGS "${FLAGS_GENERAL}" CACHE STRING "C++ Flags" FORCE)

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS_GENERAL} -specs=${DEVKITPRO}/libnx/switch.specs --verbose")
  
  add_executable(${PROJECT_NAME}.elf ${SRC_FILES})
  target_compile_options(${PROJECT_NAME}.elf PRIVATE ${FLAGS})
  target_link_libraries(${PROJECT_NAME}.elf PRIVATE ${LIBS})
  add_custom_target(${PROJECT_NAME}.nro
    DEPENDS ${PROJECT_NAME}.elf
    COMMAND ${DEVKITPRO}/tools/bin/nacptool --create "${PROJECT_NAME}" "rsn8887" "${VERSION_MAJOR}.${VERSION_MINOR}" ${PROJECT_NAME}.nacp
    COMMAND ${DEVKITPRO}/tools/bin/elf2nro ${PROJECT_NAME}.elf ${PROJECT_NAME}.nro --icon=${CMAKE_SOURCE_DIR}/switchdata/icon0.jpg --nacp=${PROJECT_NAME}.nacp
  )
  add_custom_target(${PROJECT_NAME}_switch.zip
    DEPENDS ${PROJECT_NAME}.nro
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/release/${PROJECT_NAME}
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/release/${PROJECT_NAME}
    COMMAND cp -f ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.nro ${CMAKE_BINARY_DIR}/release/${PROJECT_NAME}/${PROJECT_NAME}.nro
    COMMAND unzip ${CMAKE_SOURCE_DIR}/switchdata/data.zip -d${CMAKE_BINARY_DIR}/release/
    COMMAND cd ${CMAKE_BINARY_DIR}/release && zip -r ../${PROJECT_NAME}_${VERSION_MAJOR}.${VERSION_MINOR}_switch.zip ${PROJECT_NAME} && cd ${CMAKE_BINARY_DIR}
  )
endif(BUILD_NX)

