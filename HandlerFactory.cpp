
#include "HandlerFactory.h"
#include "RequestHandlers.h"


IRequestHandler* RequestHandlerFactory::CreateRequestHandler(Request_Type r_Type) {
    switch (r_Type) {
        case PortRequest:
            return new PortHandler();
            break;
        case ProcRequest:
            return new ProcHandler();
            break;
        case OtherRequest:
            return new ResourceHandler();
        default:
            return new PortHandler();     
    }
}
