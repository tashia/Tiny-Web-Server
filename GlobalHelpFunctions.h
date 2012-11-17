/* 
 * File:   GlobalHelpFunctions.h
 * Author: shilei
 *
 * Created on October 18, 2012, 9:50 PM
 * 
 * This header file creates two global functions for
 * conversions between int and string.
 */

#ifndef GLOBALHELPFUNCTIONS_H
#define	GLOBALHELPFUNCTIONS_H

#include <string>
#include <sstream>

/*must use inline*/

inline std::string IntToString(int size) {

    std::ostringstream out;
  out << size;
  return out.str();
}

inline int StringToInt(std::string str) {
    int val;
  std::stringstream ss(std::stringstream::in | std::stringstream::out);
  ss << str;
  ss >> val;
  return val;
}
#endif	/* GLOBALHELPFUNCTIONS_H */

