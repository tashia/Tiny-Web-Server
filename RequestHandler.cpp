
#include "RequestHandlers.h"
#include <sstream>
#include <fstream>
#include <time.h>
#include <iostream>

struct TypePair {
  std::string ext;
  std::string filetype;
};

TypePair extensions [] = {
  {"gif", "image/gif"},
  {"jpg", "image/jpeg"},
  {"jpeg", "image/jpeg"},
  {"png", "image/png"},
  {"zip", "image/zip"},
  {"gz", "image/gz"},
  {"tar", "image/tar"},
  {"htm", "text/html"},
  {"html", "text/html"},
  {"txt", "text/plain"},
  {"css", "text/css"},
  {"0", "0"}
};

//--------------< constructor >------------------

BaseHandler::BaseHandler(const Http_Request& h_request) : _http_Request(h_request),
    _hasProcessed(false) {
      _http_Response = new Http_Response();
    }

//--------------< destructor >-------------------
BaseHandler::~BaseHandler()
{
  if (_http_Response->_reponse_Resource != 0)
    delete [] _http_Response->_reponse_Resource;
  delete _http_Response;
}
//--------------< get msg body length >----------

unsigned int BaseHandler::getResponseLength() {
  if (!_hasProcessed)
    DoProcess();
  return _http_Response->_content_Length;
}

//--------------< get response body >------------
void BaseHandler::getResponseBody(const char** memoryBlock)
{
  if (!_hasProcessed)
    DoProcess();
  *memoryBlock = _http_Response->_reponse_Resource;
}

//--------------< set protocal >------------------

void BaseHandler::setProtocal(std::string protocal) {
  _http_Response->_protocal = protocal;
}

//--------------< set status >--------------------

void BaseHandler::setStatus(std::string stat) {
  _http_Response->_status = stat;
}

//--------------< set content length >------------

void BaseHandler::setContentLength(int len) {
  _http_Response->_content_Length = len;
}

//--------------< set content type >--------------

void BaseHandler::setContentType(std::string type) {
  _http_Response->_content_Type = type;
}

//--------------< set connection status >---------

void BaseHandler::setConnectionStatus(std::string stat) {
  _http_Response->_connectionStatus = stat;
}

//--------------< set message body >--------------
void BaseHandler::setMsgBody(const char* res)
{
  if (_http_Response->_reponse_Resource != 0)
    delete [] _http_Response->_reponse_Resource;
  _http_Response->_reponse_Resource = res;
}
//----------------------< int to string >---------------------------------------

std::string BaseHandler::sizeToString(size_t size) {
  std::ostringstream out;
  out << size;
  return out.str();
}

//---------------------< get the time information >-----------------------------

std::string BaseHandler::getCurrentTime() {
  time_t rawtime = time(0);
  struct tm *tms; //= localtime(&rawtime);
  tms = gmtime(&rawtime);
  char bufTime[1024];
  strftime(bufTime, sizeof bufTime, "%a, %d %b %Y %H:%M:%S %Z", tms);
  return bufTime;
}

//---------------------< get header >-------------------------

std::string BaseHandler::getResponseHeaders() {
  if(!_hasProcessed)
    DoProcess();
  std::string header(_http_Response->_protocal+" ");
  header.append(_http_Response->_status+"\r\n");
  header.append("Content-Type: "+_http_Response->_content_Type+"\r\n");
  header.append("Content-Length: ");
  header.append(sizeToString(_http_Response->_content_Length)+"\r\n");
  header.append("\r\n");
  return header;

}


//---------------------< constructor >------------------------

GetHandler::GetHandler(const Http_Request& h_request)
  : BaseHandler(h_request) {
    _continue = true;
  }


//---------------------< do process >--------------------------

void GetHandler::DoProcess() {
  setProtocal(this->_http_Request._protocal); // check
  getContentType(this->_http_Request._relativeURL);
  if (_continue) {
    if (this->_http_Request._method == "GET") {// retrieve
      ResourceRetrieve(this->_http_Request._relativeURL);
    } 
  }
  else{
    this->setContentLength(0);
  }
  if (_continue)
    this->setStatus("200 OK");
  _hasProcessed = true;
}

//---------------------< get content type >---------------------

void GetHandler::getContentType(const std::string& relativeURL) {
  std::string suffix;
  size_t begin;
  bool typeSupport = false;
  begin = relativeURL.find_last_of('.');
  suffix = relativeURL.substr(begin + 1);
  for (int i = 0; extensions[i].ext != "0"; i++) {
    if (suffix.compare(extensions[i].ext) == 0) {
      this->setContentType(extensions[i].filetype);
      typeSupport = true;
      break;
    }
  }
  if (!typeSupport) {
    this->setStatus("401 Type does not support");
    _continue = false;
  }
}

//-------------------< retrieve resource >-----------------------
void GetHandler::ResourceRetrieve(const std::string& URI)
{
  std::ifstream::pos_type size;
  /* ios::ate set initial position at the end of the file, so tellg() give the size of the file*/
  std::ifstream resource(URI.substr(1).c_str(), std::ios::in | std::ios::binary | std::ios::ate);
  if (resource.is_open()) {
    size = resource.tellg(); /* get current read position*/
    char* memblock = new char[size];
    resource.seekg(0, std::ios::beg);
    resource.read(memblock, size);
    resource.close();
    this->setContentLength(size);
    this->setMsgBody(memblock);
  } else {
    this->setStatus("404 Not Found");
    _continue = false;
  }
}


//----------------< Implementation of PostHandler >--------------------------------
//

PostHandler::PostHandler(const Http_Request& h_request) : BaseHandler(h_request)
{
}

static int count=0;
//----------------< Do Post Process >--------------------------------
//
void PostHandler::DoProcess()
{
  setProtocal(this->_http_Request._protocal); 
  //check the action
  if (this->_http_Request._relativeURL.find("message/") != std::string::npos) {
    //this is message action
    std::cout<<"the message is:\n"<<this->_http_Request._requestContent<<std::endl;
    std::cout<<count<<std::endl;
    count++;
  } else if (this->_http_Request._relativeURL.find("files/") != std::string::npos) {
    //this is a file action
    ResourceSave(this->_http_Request._relativeURL,
                 this->_http_Request._requestContent,
                 this->_http_Request._content_Length);
  } else {
    //handle the unknown action
      
    this->setContentType("text/html");
    std::string msgBody("<html><p>post failed</p></html>");
    this->setContentLength(msgBody.size());
    char *msg = new char[msgBody.size()];
    for(size_t i=0; i<msgBody.size(); i++)
        msg[i]=msgBody[i];
    this->setMsgBody(msg);
    setStatus("404 Bad");
    _hasProcessed = true;
    return;
  }
  this->setContentType("text/html");
  std::string msgBody("<html><p>post succeeded</p></html>");
  this->setContentLength(msgBody.size());
  char *msg = new char[msgBody.size()];
  for(size_t i=0; i<msgBody.size(); i++)
        msg[i]=msgBody[i];
  this->setMsgBody(msg);
  setStatus("200 OK");
  _hasProcessed = true;
  return;
}

//------------------< save resource >----------------------------
void PostHandler::ResourceSave(const std::string& URI, char* memBlock, int len)
{
    std::string URi(URI+"post.txt");
  std::ofstream saveFile(URi.substr(1).c_str(), std::ios::out | std::ios::binary);
  saveFile.write(memBlock,len);
}


