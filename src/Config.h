/**
  ******************************************************************************
  * @file           : Config.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */
#pragma once

#include "iniFile.h"
#include "Logger.h"

struct Config {
    Config() {
        hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->open("/home/hyn/test_log.log");
        hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->load("/home/hyn/test_ini.ini");
    }

    ~Config() {
        hyn::singleton::Singleton<hyn::logger::Logger>::get_instance()->close();
        hyn::singleton::Singleton<hyn::ini::IniFile>::get_instance()->save("/home/hyn/test_ini.ini");
    }
};