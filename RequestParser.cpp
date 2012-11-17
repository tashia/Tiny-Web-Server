#include "RequestParser.h"
#include "GlobalHelpFunctions.h"
#include <sstream>
#include <ctype.h>

std::string Methods[] = {"GET ", "POST ", "PUT ", "DELETE ", "HEAD ", ""};

//--------------< constructor >----------------------------------

RequestParser::RequestParser(std::string& request) : _request(request),
    _badRequest(false),
    _startPos(0)
{
}

//--------------< get Http method>--------------------------------

std::string RequestParser::getRequestMethod() {
  int i=0;
  for (i = 0; !Methods[i].empty(); i++) {
    _startPos = _request.find(Methods[i]);
    if (_startPos != std::string::npos)
      break;
  }
  if (Methods[i].empty()) {
    _badRequest = true;
    return "";
  }
  size_t end;
  end = _request.find(' ', _startPos);
  if (end == std::string::npos || end == 0){
    _badRequest = true;
    return "";
  }
  return _request.substr(_startPos, end - _startPos);
}

//--------------< get relative URL>------------------------------

std::string RequestParser::getRelativeURL() {
  if(_badRequest) return "";
  size_t begin, end;
  begin = _request.find('/', _startPos);
  if (begin == std::string::npos){
    _badRequest = true;
    return "";}
    end = _request.find(' ', begin);
    if (begin == std::string::npos){
      _badRequest = true;
      return "";
    }
    return _request.substr(begin, end - begin);
}

//--------------< get request protocal>----------------------------

std::string RequestParser::getRequestProtocal() {
  size_t begin, end;
  begin = _request.find("HTTP/", _startPos);
  if (begin == std::string::npos)
    return "";
  if ((end = _request.find("\n", begin)) != std::string::npos) {
    std::string p( _request.substr(begin, end - begin));
    if ((end = p.find('\r')) != std::string::npos) { /*delete carriage return*/
      p.erase(end, 1);
    }
    return p;
  } else
    return "";
}

//--------------< get content length >-------------------------

int RequestParser::getContentLength() {
  std::string len = getHeaderContent("Content-Length: ");
  if (len.compare("") == 0)
    len = "0";
  return StringToInt(len);
}

//--------------< get content type >---------------------------

std::string RequestParser::getContentType() {
  return getHeaderContent("Content-Type: ");
}

//--------------< get connection status >----------------------

std::string RequestParser::getConnectionStatus() {
  return getHeaderContent("Connection: ");
}

//--------------< get accept encoding >------------------------

std::string RequestParser::getAcceptEncoding() {
  return getHeaderContent("Accept-Encoding: ");
}

//--------------< get host >-----------------------------------

std::string RequestParser::getHost() {
  return getHeaderContent("Host: ");
}

//--------------< return trimed headers >-------------------------

std::string RequestParser::TrimedHeaders() {
  return _request.substr(_startPos);
}

//--------------< string to int >---------------------------------


//--------------< get header content >--------------------------

std::string RequestParser::getHeaderContent(std::string header) {
  int start, end;
  start = _request.find(header, _startPos);
  if (start == std::string::npos)
    return "";
  end = _request.find('\n', start);
  if (end == std::string::npos)
    return "";
  std::string content = _request.substr(start + header.size(), end - start - header.size());
  if ((end = content.find('\r')) != std::string::npos) { /*delete carriage return*/
    content.erase(end, 1);
  }
  return content;
}

//--------------< test stub >------------------------------------

#ifdef TEST_REQUESTPARSER

#include <iostream>
int main(int argc, const char *argv[])
{

  std::string request =  "POST /files/ HTTP/1.1\r\n"
      "Host: 127.0.0.1:3490\r\n"
      "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
      "Accept-Language: en-US,en;q=0.5\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Connection: keep-alive\r\n"
      "Referer: http://127.0.0.1:3490/index.htm\r\n"
      "Content-Length: 481\r\n"
      "Content-Type: multipart/form-data \r\n"
      "Pragma: no-cache\r\n" 
      "Cache-Control: no-cache\r\n\r\n";
  RequestParser *rp = new RequestParser(request);
  std::cout<<"\n"<<rp->getRequestMethod();
  std::cout<<"\n"<<rp->getRelativeURL();
  std::cout<<"\n"<<rp->getRequestProtocal();
  std::cout<<"\n"<<rp->getContentLength();
  std::cout<<"\n"<<rp->getContentType();
  std::cout<<"\n"<<rp->getHost();
  return 0;
}

#endif

