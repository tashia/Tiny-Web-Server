/* 
 * File:   IRequestHandler.h
 * Author: shilei
 *
 * Created on April 16, 2012, 1:05 PM
 */

#ifndef IREQUESTHANDLER_H
#define	IREQUESTHANDLER_H

#include <string>



enum Request_Type { GetType, PostType };

typedef struct
{
    std::string _method;
    std::string _host;       //maybe don't need it
    std::string _protocal;
    Request_Type _request_Type;
    char* _requestContent;
    std::string _relativeURL;
    std::string _connectionStatus;
    std::string _encoding;
    std::string _content_Type;
    int _content_Length;
}Http_Request;

typedef struct
{
    std::string _protocal;
    std::string _status;
    int _content_Length;
    std::string _content_Type;
    std::string _last_Modified_Time;
    char* _reponse_Resource;
    std::string _connectionStatus;
}Http_Response;


class IRequestHandler
{
public:
    virtual ~IRequestHandler(){}
    virtual std::string getResponseHeaders()=0;
    virtual void getResponseBody(char** memoryBlock)=0; /*dyly allocate inside*/
    virtual unsigned int getResponseLength()=0;
};


typedef struct{
	std::string ext;
	std::string filetype;
} ext; 





#endif	/* IREQUESTHANDLER_H */
