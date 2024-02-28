cmake_minimum_required(VERSION 3.27)
project(fall2023-csc3380-project3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#set(CMAKE_PREFIX_PATH "~/Qt/6.6.0/macos") # Set path to Qt download
set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.6.0")


#find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)
find_package(Qt6 6.6.0 REQUIRED COMPONENTS Widgets WebEngineWidgets)
qt_standard_project_setup()
find_package(Python 3.10 COMPONENTS Interpreter Development REQUIRED)




add_executable(project 
    code/project.cpp
    code/DataFrame.cpp
    code/TechnicalIndicators.cpp
    code/Gui.cpp
    code/getPath.cpp
    code/IndicatorBacktestCpp.cpp
)

# Combine all the libraries in a single target_link_libraries call
target_link_libraries(project 
    PRIVATE 
    Qt6::Widgets 
    Qt6::WebEngineWidgets
    ${Python_LIBRARIES}
)

target_include_directories(project PRIVATE ${Python_INCLUDE_DIRS})

set_target_properties(project PROPERTIES INSTALL_RPATH "${Python_LIBRARIES}")
set_target_properties(project PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# uncomment the lines below to enable the Catch2 testing infrastructure
# enable_testing()
# include(CTest)
#
# target_compile_definitions(project PRIVATE CATCH_CONFIG_MAIN)
# add_test(NAME project COMMAND project)