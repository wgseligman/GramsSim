/// \file EventID.cc
/// \brief Implementation of the longer EventID-related methods.
// 05-Jun-2024 WGS

#include "EventID.h"
#include "iostream"

// How to write EventID.
std::ostream& operator<< (std::ostream& out, const grams::EventID& e)
{
  out << "run=" << e.Run()
      << " event=" << e.Event();
  return out;
}
