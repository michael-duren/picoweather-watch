cmake_minimum_required(VERSION 3.23)

project(weather_exporter)

add_executable(WeatherExporter)

target_sources(WeatherExporter
    PRIVATE
        main.c
        logger.c
        logger.h
)
