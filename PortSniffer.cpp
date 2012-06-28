
#include "PortSniffer.h"
#include "XmlReader.h"
#include <iostream>
#include <fstream>

using namespace std;

//--------------------<ctor>---------------------------------------------------

PortSniffer::PortSniffer() : _pTcpRecord(0), _pUdpRecord(0), _pNewTcpPorts(0),
_pNewUdpPorts(0), _newTcpPortsCount(0), _newUdpPortsCount(0), _summaryXML(0) {
}

//-------------------<destructor>----------------------------------------------

PortSniffer::~PortSniffer() {
    if (_pTcpRecord != 0)
        delete _pTcpRecord;
    if (_pUdpRecord != 0)
        delete _pUdpRecord;
    if (_pNewTcpPorts != 0)
        delete _pNewTcpPorts;
    if (_pNewUdpPorts != 0)
        delete _pNewUdpPorts;
    if (_summaryXML != 0)
        delete _summaryXML;
}
//--------------------<write shell result into file>---------------------------

void PortSniffer::WriteShellResult(const std::string command,
        const std::string filename) {
    FILE *fp;
    int status;
    size_t maxSize = 100;

    char* path = new char[maxSize];

    fp = popen(command.c_str(), "r");
    if (fp == NULL)
        perror(" popen fails");

    ofstream tcpFile;
    tcpFile.open(filename.c_str());
    while (getline(&path, &maxSize, fp) != -1) {
     //   printf("%s", path);
        tcpFile << path;
    }
    tcpFile.close();
    status = pclose(fp);
    delete path;
    if (status == -1)
        perror("close fails");
    return;
}

//-----------------< Get ports from report >-----------------------------
std::vector<std::string> PortSniffer::GetPortsFromReport(const XmlString& report, const std::string& tagName)
{
    XmlReader xdr(report);
    std::vector<std::string> portsVec;
    while(xdr.next())
    {
        if (xdr.tag().compare(tagName) == 0) {
            portsVec.push_back(xdr.body());
        }
    }
    return portsVec;
}

//-----------------<Get all ports>----------------------------------------

void PortSniffer::GetPorts(string recordFileName,
        std::map<std::string, std::string>& ports) {
    ifstream openTcpfile(recordFileName.c_str());
    string line;
    int count = 1;
    while (count < 3) {
        getline(openTcpfile, line);
        count++;
    }
    // parse the string
    size_t pos, endPos;
    pos = line.find("L");
    while (getline(openTcpfile, line)) {
        endPos = line.find(' ', pos);
        ports[line.substr(pos, endPos - pos)] = "NULL";
    }
}

//-----------------<get record>--------------------------------------------

void PortSniffer::GetRecord() {
    string recXML = PreviousReport(complete);
    XmlReader xdr(recXML);
    if (_pTcpRecord != 0)
        delete _pTcpRecord;
    _pTcpRecord = new std::map<std::string, std::string > ();
    if (_pUdpRecord != 0)
        delete _pUdpRecord;
    _pUdpRecord = new std::map<std::string, std::string > ();
    while (xdr.next()) {
        if (xdr.tag().compare("Tcp_Port") == 0) {
            (*_pTcpRecord)[xdr.body()] = "NULL";
        } else if (xdr.tag().compare("Udp_Port") == 0) {
            (*_pUdpRecord)[xdr.body()] = "NULL";
        }
    }
    return;
}


//--------------------------<store the result >-----------------------------

void PortSniffer::StoreResult(const std::string filename,
        const std::string xmlReport) {
    ofstream out(filename.c_str());
    if (out.good()) {
        out << xmlReport;
        out.close();
    }
    return;
}

//------------------------< extract the result >----------------------------

PortSniffer::XmlString PortSniffer::ExtractResult(const std::string filename) {
    ifstream fp(filename.c_str());
    if (fp) {
        std::string xmlContainer, temp;
        if (fp.is_open()) {
            while (fp.good()) {
                getline(fp, temp);
                xmlContainer += temp;
            }
            fp.close();
        }
        return xmlContainer;
    } else {
        cout << "\n no file" << endl;
        return "";
    }
}

PortSniffer::XmlString PortSniffer::CompeleteReport() {
    WriteShellResult("netstat -lt", "listenTcp.txt");
    sleep(1);
    GetPorts("listenTcp.txt", _pTcpPorts);
    sleep(1);
    WriteShellResult("netstat -lu", "listenUdp.txt");
    sleep(1);
    GetPorts("listenUdp.txt", _pUdpPorts);
    sleep(1);
    XmlWriter head;
    head.start("Listen_Ports");
    map<string, string>::iterator it, rit;
    GetRecord(); // fill the _pRecord processes
    if (_pNewTcpPorts != 0)
        delete _pNewTcpPorts;
    _pNewTcpPorts = new map<string, string > ();
    for (it = _pTcpPorts.begin(); it != _pTcpPorts.end(); it++) {
        rit = _pTcpRecord->find((*it).first);
        if (rit == _pTcpRecord->end()) {
            (*_pNewTcpPorts)[(*it).first] = (*it).second;
        }
        AddXmlBody(head, "Tcp_Port", (*it).first);
    }
    if (_pNewUdpPorts != 0)
        delete _pNewUdpPorts;
    _pNewUdpPorts = new map<string, string > ();
    for (it = _pUdpPorts.begin(); it != _pUdpPorts.end(); it++) {
        rit = _pUdpRecord->find((*it).first);
        if (rit == _pUdpRecord->end()) {
            (*_pNewUdpPorts)[(*it).first] = (*it).second;
        }
        AddXmlBody(head, "Udp_Port", (*it).first);
    }
    head.end();
    StoreResult("compPorts.xml", head.xml());
    return head.xml();
}

//--------------------< get whole xml report String>----------------------------

PortSniffer::XmlString PortSniffer::WholeReport() {
    XmlString report;
    report = CompeleteReport();
    SnifferReport();
    return report;
}
//---------------<get new result>------------------------

PortSniffer::XmlString PortSniffer::SnifferReport() {
    if (_pNewTcpPorts == 0 || _pNewUdpPorts == 0) {
        CompeleteReport();
    }
    XmlWriter head;
    head.start("New_Ports");
    map<string, string>::iterator it;
    for (it = _pNewTcpPorts->begin(); it != _pNewTcpPorts->end(); ++it) {
        AddXmlBody(head, "Tcp_Port", (*it).first);
    }
    for (it = _pNewUdpPorts->begin(); it != _pNewUdpPorts->end(); ++it) {
        AddXmlBody(head, "Udp_Port", (*it).first);
    }
    head.end();
    _newTcpPortsCount = _pNewTcpPorts->size();
    _newUdpPortsCount = _pNewUdpPorts->size();
    StoreResult("newPorts.xml", head.xml());
    if (_summaryXML != 0)
        delete _summaryXML;
    _summaryXML = new string(GetSummaryReport());
    return head.xml();
}

//---------------<get summary xml string>---------------------------------------
PortSniffer::XmlString PortSniffer::SummaryReport()
{
    if(_summaryXML == 0)
        SnifferReport();
    return (*_summaryXML);
}

//---------------< generate summary report to private data >--------------------
PortSniffer::XmlString PortSniffer::GetSummaryReport() {
    XmlWriter head;
    head.start("New_Ports_Summary");
    char buffer[33];
    sprintf(buffer, "%d", _newTcpPortsCount);
    AddXmlBody(head, "New_tcp_ports_No", buffer);
    sprintf(buffer, "%d", _newUdpPortsCount);
    AddXmlBody(head, "New_udp_ports_No", buffer);
    head.end();
    StoreResult("portsSummary.xml", head.xml());
    return head.xml();
}

//---------------<get previous result>-----------------------------------

PortSniffer::XmlString PortSniffer::PreviousReport(ReportType type) {
    string xmlResult;
    switch (type) {
        case summary:
            xmlResult = ExtractResult("portsSummary.xml");
            break;
        case complete:
            xmlResult = ExtractResult("compPorts.xml");
            break;
        case new_only:
            xmlResult = ExtractResult("newPorts.xml");
            break;
    }
    return xmlResult;
}


//---------------<add element helper>-------------------

void PortSniffer::AddXmlBody(XmlWriter& xwr, string tag, string body) {
    XmlWriter ele;
    ele.start(tag);
    ele.addBody(body);
    ele.end();
    xwr.addBody(ele.xml());
}