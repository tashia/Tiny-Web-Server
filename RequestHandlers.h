/* 
 * File:   RequestHandlers.h
 * Author: shilei
 *
 * Created on May 31, 2012, 9:20 PM
 */
/*        Module Description
 *======================================
 * This package provides two classes.
 * One is BaseHandler which is the base
 * class of all handlers. It contains several
 * common methods needed by almost all derived
 * classes. The other class is Static Handler
 * which is able to handler simple get and post
 * request so far. Need to be modified in the
 * future.
 * 
 * 
 * Interface
 * ======================================
 * IRequestHandler* iPH = new GetHandler(h_request);
 * iPH->DoProcess ()         // process the request
 * iPH->getResponseHeaders()  // get response information
 * char* block;
 * iPH->getResponseBody(&block)  //get response body
 * iPH->getResponseLength();     //get the response body length
 * iPH->getCurrentTime();        //get GMT time string
 *
 */
#ifndef REQUESTHANDLERS_H
#define	REQUESTHANDLERS_H

#include "IRequestHandler.h"
#include <map>
#include <vector>

/* Global functions for scanning files in server */

std::string ScanFiles(std::string dir);

class BaseHandler : public IRequestHandler {
public:
    BaseHandler(const Http_Request& h_request);
    virtual ~BaseHandler();
    virtual void DoProcess() = 0;
    virtual std::string getResponseHeaders();
    virtual void getResponseBody(const char** memoryBlock);
    unsigned int getResponseLength();
    std::string sizeToString(size_t size);
    std::string getCurrentTime();
protected:
    void setProtocal(std::string protocal);
    void setStatus(std::string stat);
    void setContentLength(int len);
    void setContentType(std::string type);
    void setConnectionStatus(std::string stat);
    void setMsgBody(const char* res);
    bool _hasProcessed;
    const Http_Request& _http_Request;
private:
    Http_Response* _http_Response;

};

class GetHandler : public BaseHandler {
public:
    GetHandler(const Http_Request& h_request);
    void DoProcess();
private:
    void getContentType(const std::string& relativeURL);
    void ResourceRetrieve(const std::string& URI);
    bool _continue;
    std::string _fileSuffix;
};

class PostHandler : public BaseHandler {
public:
    PostHandler(const Http_Request& h_request);
    void DoProcess();
    virtual ~PostHandler() {
    } // if lack of this build error
private:
    void ResourceSave(const std::string&URI, char* memBlock, int len);

};



#endif	/* REQUESTHANDLERS_H */

