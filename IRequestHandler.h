/* 
 * File:   IRequestHandler.h
 * Author: shilei
 *
 * Created on April 16, 2012, 1:05 PM
 */

#ifndef IREQUESTHANDLER_H
#define	IREQUESTHANDLER_H

#include <string>



enum Request_Type{PortRequest, ProcRequest, FileRequest,OtherRequest};

typedef struct
{
    std::string _method;
    std::string _host;       //maybe don't need it
    std::string _protocal;
    Request_Type _request_Type;
    std::string _requestData;
    std::string _relativeURL;
    int _content_Length;
}Http_Request;

typedef struct
{
    std::string _protocal;
    std::string _status;
    int _content_Length;
    std::string _content_Type;
    char* _last_Modified_Time;
    char* _reponse_Resource;
}Http_Response;


class IRequestHandler
{
public:
    virtual ~IRequestHandler(){}
    virtual char* Response(const Http_Request& h_request)=0;
    virtual unsigned int getResponseLength()=0;
};


struct {
	std::string ext;
	std::string filetype;
} extensions [] = {
	{"gif", "image/gif" },  
	{"jpg", "image/jpeg"}, 
	{"jpeg","image/jpeg"},
	{"png", "image/png" },  
	{"zip", "application/zip" },  
	{"gz",  "image/gz"  },  
	{"tar", "image/tar" },  
	{"htm", "text/html" },  
	{"html","text/html" }, 
        {"txt", "text/plain"},
	{"0","0"} }; 



#endif	/* IREQUESTHANDLER_H */

