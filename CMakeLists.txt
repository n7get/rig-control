
cmake_minimum_required(VERSION 3.30)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(rig_control)
idf_build_set_property(PARTITION_TABLE_FILE partitions.csv)
