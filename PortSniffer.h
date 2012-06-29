/* 
 * File:   ProtSniffer.h
 * Author: shilei
 *
 * Created on April 2, 2012, 12:14 PM
 */

#ifndef PROTSNIFFER_H
#define	PROTSNIFFER_H

#include <map>
#include <string>
#include "XmlWriter.h"
#include <vector>

class PortSniffer {
    public:
        typedef std::string XmlString;

        enum ReportType {
            summary, complete, new_only
        };
        PortSniffer();
        ~PortSniffer();
        XmlString SnifferReport();
        XmlString PreviousReport(ReportType type = new_only);
        XmlString WholeReport();
        XmlString SummaryReport();
        std::vector<std::string> GetPortsFromReport(const XmlString& report,
                const std::string& tagName);
    private:
        XmlString CompeleteReport();
        XmlString GetSummaryReport();
        void WriteShellResult(const std::string command, const std::string filename);
        void GetPorts(std::string filename, std::map<std::string,std::string>&);
        void GetRecord();
        void StoreResult(const std::string filename, const std::string xmlReport);
        void AddXmlBody(XmlWriter& xwr, std::string tag, std::string content);
        XmlString ExtractResult(const std::string filename);
        std::map<std::string,std::string> _pTcpPorts;
        std::map<std::string,std::string> _pUdpPorts;
        std::map<std::string,std::string>* _pTcpRecord;
        std::map<std::string,std::string>* _pUdpRecord;
        std::map<std::string,std::string>* _pNewTcpPorts;
        std::map<std::string,std::string>* _pNewUdpPorts;
        XmlString  *_summaryXML;
        int _newTcpPortsCount;
        int _newUdpPortsCount;
};


#endif	/* PROTSNIFFER_H */

