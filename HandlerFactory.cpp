
#include "HandlerFactory.h"
#include "RequestHandlers.h"


IRequestHandler* RequestHandlerFactory::CreateRequestHandler(const Http_Request& h_request) {
    return new StaticResHandler(h_request);
}
