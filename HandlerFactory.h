/* 
 * File:   HandlerFactory.h
 * Author: shilei
 *
 * Created on April 16, 2012, 1:11 PM
 */

#ifndef HANDLERFACTORY_H
#define	HANDLERFACTORY_H

#include "IRequestHandler.h"

class RequestHandlerFactory
{
public:
    IRequestHandler* CreateRequestHandler(Request_Type r_Type);
};


#endif	/* HANDLERFACTORY_H */

