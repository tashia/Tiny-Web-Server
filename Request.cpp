
#include "Request.h"
#include "GlobalHelpFunctions.h"

//----------------< Cotor >--------------------------------
//
Request::Request(const std::string& method,
                 const std::string& url,
                 const std::string& protocal)
                :_method(method), _url(url), _protocal(protocal),
                 _content(0), _length(0), _headers("")
{
}

//----------------< append headers >--------------------------------
//
void Request::appendHeader(const std::string& header)
{
  _headers.append(header+"\r\n");
}

//----------------< add content body >------------------------------
//
void Request::addContentBody(const char* contentBody,
                             const int contentLength)
{
  _length = contentLength;
  _content = contentBody;
  // int to string
  std::string len = IntToString(_length);
  this->appendHeader("Content-Length: "+len);
}

//----------------< get request >--------------------------------
//
std::string Request::getRequest()
{
  std::string req(_method+" "+_url+" "+_protocal+"\r\n");
  req.append(_headers);
  req.append("\r\n");
  return req;
}

//----------------< get content body >--------------------------------
//
const char* Request::getContentBody()
{
   return _content;
}

//----------------< get the header content >--------------------------------
//
std::string Request::getHeaderContent(const std::string& headerName) const 
{
  size_t pos;
  pos = _headers.find(headerName);
  if (pos == std::string::npos) return "";
  size_t endPos;
  endPos = _headers.find_first_of('\n', pos);
  std::string headerContent( _headers.substr(pos+headerName.size(), 
                                             endPos-pos));
  if (headerContent[headerContent.size()-1] == '\r')
    headerContent.erase(headerContent.size(),1);
  return headerContent;
}

//----------------< destructor >--------------------------------
//
Request::~Request()
{
  if (_content != 0) delete [] _content;
}


#ifdef TEST_REQUEST
//----------------< Test Stub >--------------------------------
//

#include <iostream>

using namespace std;

int main(int argc, const char *argv[])
{
  Request *iR = new Request("GET","/index.htm", "HTTP/1.1");
  std::string h1("Host: localhost:80");
  iR->appendHeader(h1);
  h1.clear();
  h1.append("Content-Type: text/html");
  iR->appendHeader(h1);
  int i,len = 7;
  char *co = new char[len];
  for (i = 0; i < len; i++) {
    co[i] = char(i+'0');
  }
  iR->addContentBody(co, len);
  cout<< iR->getRequest()<<endl;
  cout<<iR->getContentBody()<<endl;
  delete iR;
  return 0;
}
#endif /*test_request*/
