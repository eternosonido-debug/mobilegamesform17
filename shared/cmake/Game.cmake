get_filename_component(FMG_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
if(NOT TARGET fmg_runtime)
 find_package(SDL2 REQUIRED CONFIG)
 add_library(fmg_runtime STATIC "${FMG_ROOT}/shared/fmg-runtime/src/runtime_entry.cpp")
 target_compile_features(fmg_runtime PUBLIC cxx_std_11)
 target_link_libraries(fmg_runtime PUBLIC SDL2::SDL2)
 target_include_directories(fmg_runtime PUBLIC "${FMG_ROOT}/shared/fmg-runtime/src"
  "${FMG_ROOT}/games/juices-sort/src"
  "${FMG_ROOT}/games/new-blocks/src"
  "${FMG_ROOT}/games/color-mix/src"
  "${FMG_ROOT}/games/sudoku/src"
  "${FMG_ROOT}/games/traffic-jam/src"
  "${FMG_ROOT}/games/mahjong/src"
  "${FMG_ROOT}/games/boca/src"
  "${FMG_ROOT}/games/color-loop/src"
 )
endif()
function(fmg_add_game slug)
 add_executable(${slug} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
 target_link_libraries(${slug} PRIVATE fmg_runtime)
 if(slug STREQUAL "boca")
  add_custom_command(TARGET ${slug} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${FMG_ROOT}/games/boca/assets" "$<TARGET_FILE_DIR:${slug}>/../assets")
 endif()
 install(TARGETS ${slug} RUNTIME DESTINATION bin)
endfunction()
