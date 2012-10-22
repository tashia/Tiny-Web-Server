#ifndef REQUEST_H
#define REQUEST_H 
/* File: Request.h
 * Author:  Shilei Zhu
 * Date: 2012-10-18
 * Purpose: This Request Package provides
 *          a Request class which helps
 *          the SendRequest method in HttpServer
 *          to send http request.
 *
 *
 */
/*        Module Description
 *======================================
 * Request("POST","/","HTTP/1.1") constructor
 * appendHeader(header)   add new header
 * string getRequest()    get the request string without
 *                        the content body
 * string 
 * getHeaderContent("Host: ") get the info of the specific
 *                          header
 * int getLength()        get the content length
 * addContentBody()       add the content body and content
 *                        length
 *
 */

#include <string>

class Request {
 public:
  Request (const std::string& method,
           const std::string& url,
           const std::string& protocal);
  void appendHeader(const std::string& header);
  std::string getRequest();
  std::string getHeaderContent(const std::string& headerName) const;
  const char* getContentBody();
  void addContentBody(const char* contentBody,
                      const int contentLength);
  int getLength() { return _length;}
  virtual ~Request ();
 private:
  std::string _method;
  std::string _url;
  std::string _protocal;
  const char* _content;
  int _length;
  std::string _headers;
};
#endif /* REQUEST_H */
