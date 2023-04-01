//
// Created by hyn on 2023/3/18.
//

#pragma once

#include <stdexcept>
#include <cassert>


#define DEBUG(expr) \
        assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
        if((expr)) throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)


