
#include "RequestHandlers.h"
#include "PortSniffer.h"
#include "ProcSniffer.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>   //strlen
#include <time.h>

char* PortHandler::Response(const Http_Request& h_request) {
    PortSniffer* pPS = new PortSniffer();
    std::string report = pPS->WholeReport();
     std::vector<std::string> portsVec = pPS->GetPortsFromReport(report,"Tcp_Port");
     std::string msgBody("<html><body><h1>Tcp Ports</h1>");
     for(size_t i=0; i<portsVec.size(); ++i)
         msgBody.append("<p>"+portsVec[i]+"</p>");
     msgBody.append("</body></html>");  
 //   std::string msgBody = "<html><body><h2>Norwegian Mountain Trip</h2><img border=\"0\" src=\"/images/nigel.jpg\" alt=\"Pulpit rock\" width=\"304\" height=\"228\" /></body>";
    _http_Response._protocal = h_request._protocal;
    _http_Response._content_Type = "text/html";
    char* pC = new char[msgBody.size()];
    for (int i = 0; i < msgBody.size(); ++i)
        *(pC + i) = (char) msgBody[i];
    _http_Response._reponse_Resource = pC;
    _http_Response._content_Length = msgBody.size();
    _http_Response._status = "200 OK";
    std::string resp = CreateResponseHeaders(_http_Response);
    char * pData = new char[resp.size()];
    for (int i = 0; i < resp.size(); ++i)
        *(pData + i) = (char) resp[i];
    delete pPS;
    return pData;
}

char* ProcHandler::Response(const Http_Request& h_request) {
    ProcSniffer* pPS = new ProcSniffer();
    std::string report = pPS->WholeReport();
    std::vector<std::string> procsVec = pPS->GetProcsFromReport(report, "process");
    std::string msgBody("<html><body><h1>Processes</h1>");
    for (size_t i = 0; i < procsVec.size(); ++i)
        msgBody.append("<p>" + procsVec[i] + "</p>");
    msgBody.append("</body></html>");
    _http_Response._protocal = h_request._protocal;
    _http_Response._content_Type = "text/html";
    char* pC = new char[msgBody.size()];
    for (int i = 0; i < msgBody.size(); ++i)
        *(pC + i) = (char) msgBody[i];
    _http_Response._reponse_Resource = pC;
    _http_Response._content_Length = msgBody.size();
    _http_Response._status = "200 OK";
    std::string resp = CreateResponseHeaders(_http_Response);
    char * pData = new char[resp.size()];
    for (int i = 0; i < resp.size(); ++i)
        *(pData + i) = (char) resp[i];
    delete pPS;
    return pData;
}


//---------------------< resouce handler >--------------------------------------

char* ResourceHandler::Response(const Http_Request& h_request) {
    // check request
    //   if (!IsGood(h_request))
    //     return _http_Response._status="400 bad";
    _http_Response._protocal = h_request._protocal;
    ParseMessageBody(h_request._requestData);
    getContentType(h_request._relativeURL);
    FindResource(h_request._relativeURL);
    _http_Response._status = "200 OK";
    char* resp = CreateResponseHeaders(_http_Response);
    return resp;
}


//-----------------< the source need format >-----------------------------------

bool ResourceHandler::NeedFormat(const std::string& contentType) {
    if (contentType.compare("image/gif") == 0) {
        return true;
    }
    return false;
}

void ResourceHandler::FindResource(const std::string& relativeURL) {
    std::ifstream::pos_type size;
    char* memoryblock;
    /* ios::ate set initial position at the end of the file, so tellg() give the size of the file*/
    std::ifstream resource(relativeURL.substr(1).c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (resource.is_open()) {
        size = resource.tellg(); /* get current read position*/
        memoryblock = new char[size];
        resource.seekg(0, std::ios::beg);
        resource.read(memoryblock, size);
        resource.close();
        _http_Response._content_Length = size;
        _http_Response._reponse_Resource = memoryblock;
    } else {
        _http_Response._status = "404 Not Found";
    }
}

void ResourceHandler::getContentType(const std::string& relativeURL) {
    std::string suffix;
    size_t begin;
    bool isTypeSupport = false;
    begin = relativeURL.find_last_of('.');
    suffix = relativeURL.substr(begin + 1);
    for (int i = 0; extensions[i].ext != "0"; i++) {
        if (suffix.compare(extensions[i].ext) == 0) {
            _http_Response._content_Type = extensions[i].filetype;
            isTypeSupport = true;
            break;
        }
    }
    if (!isTypeSupport)
        _http_Response._status = "401 Type does not support";
}

bool BaseHandler::IsGood(const Http_Request& h_request) {
    //check method
    if (h_request._method.compare("GET") != 0 && h_request._method.compare("POST") != 0)
        return false;
    return true;
}

char* BaseHandler::CreateResponseHeaders(const Http_Response& h_response) {
    std::string r_Header(h_response._protocal);
    r_Header.append(" " + h_response._status + "\r\n");
  //  r_Header.append("Date: " + getCurrentTime() + "\r\n");
    r_Header.append("Content-Type: " + h_response._content_Type + "\r\n");
    r_Header.append("Content-Length: " + sizeToString(h_response._content_Length));
    r_Header.append("\r\n\r\n");
    std::cout<<r_Header<<std::endl;
    _responseLen = r_Header.size() + h_response._content_Length;
    char* pC = new char[_responseLen];
    for (int i = 0; i < r_Header.size(); ++i)
        *(pC + i) = (char) r_Header[i];
    for (int j = 0; j < h_response._content_Length; j++)
        *(pC + r_Header.size() + j) = *(h_response._reponse_Resource+j);
    delete [] h_response._reponse_Resource;
    return pC;
}


//---------------------< parse message body >----------------------------------

void BaseHandler::ParseMessageBody(const std::string& msg) {
    if (msg.compare("") == 0)
        return;
    ValuePair::iterator it;
    std::string name, value;
    size_t end, begin = 0;
    bool notDone = true;
    while (notDone) {
        end = msg.find("=", begin);
        name = msg.substr(begin, end - begin);
        begin = end + 1;
        end = msg.find("&", begin);
        if (end == std::string::npos) {
            value = msg.substr(begin, msg.size() - begin);
            notDone = false;
        }else{
        value = msg.substr(begin, end - begin);}
        std::cout<<"The name is "<<name<<"--- The value is "<<value<<std::endl;
        begin = end+1;
        it = _pValuePair->find(name);
        if(it == _pValuePair->end())
        {
            std::vector<std::string> values;
            values.push_back(value);
            (*_pValuePair)[name] = values;
        }else{
            (*it).second.push_back(value);
        }
    }
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
    struct tm *tms = localtime(&rawtime);
    // tms = gmtime(&rawtime);
    char bufTime[1024];
    strftime(bufTime, sizeof bufTime, "%a, %d %b %Y %H:%M:%S %Z", tms);
    return bufTime;
}
