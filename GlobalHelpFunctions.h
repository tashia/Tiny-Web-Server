/* 
 * File:   GlobalHelpFunctions.h
 * Author: shilei
 *
 * Created on October 18, 2012, 9:50 PM
 */

#ifndef GLOBALHELPFUNCTIONS_H
#define	GLOBALHELPFUNCTIONS_H

#include <string>
#include <sstream>

std::string IntToString(int size) {

    std::ostringstream out;
  out << size;
  return out.str();
}

int StringToInt(std::string str) {
    int val;
  std::stringstream ss(std::stringstream::in | std::stringstream::out);
  ss << str;
  ss >> val;
  return val;
}
#endif	/* GLOBALHELPFUNCTIONS_H */

