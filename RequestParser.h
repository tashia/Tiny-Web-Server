/* 
 * File:   RequestParser.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:58 PM
 */
/*        Module Description
 *======================================
 * This package provides a RequestParser
 * class to parse the http request. It help
 * RequestHandler to fill the http request
 * struct.
 *
 *
 *
 */
#ifndef REQUESTPARSER_H
#define	REQUESTPARSER_H

#include <string>


class RequestParser {   
public:
    RequestParser(std::string& request) ;
    ~RequestParser() {
    }
    std::string getRequestMethod();
    std::string getRequestProtocal();
    std::string getContentType();
    std::string getConnectionStatus();
    std::string getRelativeURL(); 
    std::string getAcceptEncoding();
    std::string getHost();
    std::string TrimedHeaders();   //cut off redundant prefix if it has  
    int getContentLength();
protected:
    std::string getHeaderContent(std::string header);
private:
    std::string _request; 
    int _startPos;
    bool _badRequest;
};


#endif	/* REQUESTPARSER_H */

