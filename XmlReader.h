#ifndef XMLREADER_H
#define XMLREADER_H
///////////////////////////////////////////////////////////////////////
// XmlReader.cpp - Parse XML strings                                 //
// ver 2.2                                                           //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// platform:    Sony Viao L series, Windows 7, Home Premium, SP1     //
// Application: CSE775 - Distributed Objects Demo                    //
// Author:      Shilei Zhu                                           //
// Original Author: Jim Fawcett, Syracuse University, CST 4-187      //
//              www.lcs.syr.edu/faculty/fawcett, (315) 443-3948      //
///////////////////////////////////////////////////////////////////////
/*
 * Note: XmlReader and XmlWriter supercede both XmlTran and XmlParser.
 * Modification: modified the Attribute method which makes the
 *               method can read "/" by Shilei Zhu
 * Package Operations:
 * ===================
 * This package contains a single class, XmlReader.  It provides a
 * simple interface to step from XML element to element and extract
 * constituent parts, e.g., tag, attributes, and body.  The package
 * does not support editing XML strings.  For that, you should use
 * the XmlDocument package.
 *
 * Parsing honors element scope, e.g., if there are two elements with
 * the same tag names at different levels of the document scope, the 
 * XmlReader will not get confused and prematurely close an element
 * because it found a matching closing tag for a contained element.
 *
 * Exceptions will be thrown by element(), body(), and attributes()
 * if malformed XML is detected.
 *
 * Required Files:
 * ===============
 * XmlReader.h, XmlReader.cpp
 *
 * Build Process:
 * ==============
 * cl /Eha XmlReader.cpp
 *
 * Maintenance History:
 * ====================
 * ver 2.2 : 22 Jan 12
 * - ported to Linux with changes to template brackets and
 *   exceptions
 * ver 2.1 : 25 Jul 11
 * - added comments to manual page, above, and implementations in
 *   XmlReader.cpp
 * ver 2.0 : 27 Jun 11
 * - moved #include<stack> to XmlReader.cpp
 * - moved #include<iostream> to test stub in XmlReader.cpp
 * ver 1.0 : 26 Jun 11
 * - first release
 */
#include <vector>
#include <string>  // added

class XmlReader
{
public:
  typedef std::vector<std::pair<std::string,std::string> > attribElems;   // change >> to > >
  XmlReader(const std::string& xml);
  bool next();
  std::string tag();
  attribElems attributes();
  std::string body();
  std::string element();
  std::string extractIdentifier(size_t& pos);
  void reset();
private:
  // source string
  std::string _xml;
  size_t position;
  // current element parts
  std::string _tag;
  attribElems _attributes;
  size_t localposition;
};

inline void XmlReader::reset() { position = 0; }

#endif
