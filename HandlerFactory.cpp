
#include "HandlerFactory.h"
#include "RequestHandlers.h"

IRequestHandler* RequestHandlerFactory::CreateRequestHandler(const Http_Request& h_request) {
    if (h_request._method.compare("POST") == 0)
        return new PostHandler(h_request);
    if (h_request._method.compare("GET") == 0)
        return new GetHandler(h_request);
    else
        return 0;
}
