/* 
 * File:   HandlerFactory.h
 * Author: shilei
 *
 * Created on April 16, 2012, 1:11 PM
 * 
 * This RequestHandlerFactory class creates an interface of a request
 * handler
 */

#ifndef HANDLERFACTORY_H
#define	HANDLERFACTORY_H

#include "IRequestHandler.h"

class RequestHandlerFactory
{
public:
   static IRequestHandler* CreateRequestHandler(const Http_Request& h_request);
};


#endif	/* HANDLERFACTORY_H */

