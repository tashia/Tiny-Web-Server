/* 
 * File:   RequestParser.h
 * Author: shilei
 *
 * Created on May 31, 2012, 7:58 PM
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
    int StringToInt(std::string& str);
protected:
    std::string getHeaderContent(std::string header);
private:
    std::string _request; //must be const
    int _startPos;
};


#endif	/* REQUESTPARSER_H */

