/* 
 * File:   RequestHandlers.h
 * Author: shilei
 *
 * Created on April 17, 2012, 7:46 AM
 */

#ifndef REQUESTHANDLERS_H
#define	REQUESTHANDLERS_H

#include "IRequestHandler.h"
#include <map>
#include <vector>

class BaseHandler : public IRequestHandler {
public:
    typedef std::map<std::string,std::vector<std::string> > ValuePair;
    BaseHandler(){ _pValuePair = new ValuePair();};
    virtual ~BaseHandler() {
        delete _pValuePair;
    }
    virtual char* Response(const Http_Request& h_request) = 0;
    std::string sizeToString(size_t size);
    std::string getCurrentTime();
    bool IsGood(const Http_Request& h_request);
    void ParseMessageBody(const std::string& msg);
    unsigned int getResponseLength(){ return _responseLen; }
protected:
    ValuePair* _pValuePair;
    unsigned int _responseLen;
    char* CreateResponseHeaders(const Http_Response& h_response);
};

class PortHandler : public BaseHandler {
public:
   char* Response(const Http_Request& h_request);
private:
    Http_Response _http_Response;
};

class ProcHandler : public BaseHandler {
public:
    char* Response(const Http_Request& h_request);
private:
    Http_Response _http_Response;
};


class ResourceHandler : public BaseHandler
{
public:
    char* Response(const Http_Request& h_request);
    void FindResource(const std::string& relativeURL);
    void getContentType(const std::string& relativeURL);
    bool NeedFormat(const std::string& contentType);
private:
    Http_Response _http_Response;
};


#endif	/* REQUESTHANDLERS_H */

