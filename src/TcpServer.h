/**
  ******************************************************************************
  * @file           : TCPserver.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/20
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>
#include "Address.h"
#include "IOManager.h"
#include "Socket.h"
#include "iniFile.h"

namespace hyn {

class TcpServer : public std::enable_shared_from_this<TcpServer> {

};

} // hyn

