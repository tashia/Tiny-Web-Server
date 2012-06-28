
#include "ProcSniffer.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "XmlWriter.h"
#include "XmlReader.h"

using namespace std;

//---------------<ctor>-----------------------------------

ProcSniffer::ProcSniffer() : _pRecord(0), _pNewProcs(0), _newProcCount(0),
                                        _summaryXML(0)
{
}

//---------------<destructor>-----------------------------

ProcSniffer::~ProcSniffer() {
    if (_pRecord != 0)
        delete _pRecord;
    if (_pNewProcs != 0)
        delete _pNewProcs;
    if (_summaryXML != 0)
        delete _summaryXML;
}

void ProcSniffer::WriteShellResult(const std::string command,
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
    _recordFileName = filename;
    delete path;
    if (status == -1)
        perror("close fails");
    return;
}


//-------------------<get the processes from report >--------------------
std::vector<std::string> ProcSniffer::GetProcsFromReport(const XmlString& report, const std::string& tagName)
{
     XmlReader xdr(report);
    std::vector<std::string> itemsVec;
    while(xdr.next())
    {
        if (xdr.tag().compare(tagName) == 0) {
            itemsVec.push_back(xdr.body());
        }
    }
    return itemsVec;
}

//-------------------< parse and get processes >-------------------------
void ProcSniffer::GetProcesses() {
    ifstream openProc(_recordFileName.c_str());
    string line;
    getline(openProc, line);
    size_t pos = line.find("CMD");
    string pid, procName;
    size_t pidfirst, pidLast, procLast;
    while (getline(openProc, line)) {
        pidfirst = line.find_first_not_of(' ');
        pidLast = line.find_first_of(' ', pidfirst);
        pid = line.substr(pidfirst, pidLast - pidfirst);
        procLast = line.find(' ', pos);
        procName = line.substr(pos, procLast - pos);
        _pIDs_Names[pid] = procName;
    }

}

//-------------------<Sniffer reproter>---------------------------------------

std::string ProcSniffer::CompeleteReport() {
    WriteShellResult("ps -au$USER","proc.txt");
    GetProcesses();
    XmlWriter head, body;
    head.start("USER_Processes");
    map<string, string>::iterator it, rit;
    GetRecord(); // fill the _pRecord processes
    if(_pNewProcs != 0)
        delete _pNewProcs;
    _pNewProcs = new map<string, string > ();
    for (it = _pIDs_Names.begin(); it != _pIDs_Names.end(); it++) {
        body.start("process");
        rit = _pRecord->find((*it).first);
        if (rit == _pRecord->end()) {
            (*_pNewProcs)[(*it).first] = (*it).second;
        }
        body.addAttribute("name", (*it).second);
        body.addBody((*it).first);
        body.end();
        head.addBody(body.xml());
        body.clear();
    }
    head.end();
    StoreResult("compProcesses.xml", head.xml());
    return head.xml();
}

//--------------------< get whole xml report String>----------------------------
ProcSniffer::XmlString ProcSniffer::WholeReport()
{
    string report;
    report = CompeleteReport();
    SnifferReport();
    return report;
}

//------------------< get new reports >----------------------------------------

std::string ProcSniffer::SnifferReport() {
    if (_pNewProcs == 0)
        CompeleteReport(); //? need renew?
    XmlWriter head, body;
    head.start("New_USER_Processes");
    map<string, string>::iterator it;
    for (it = _pNewProcs->begin(); it != _pNewProcs->end(); ++it) {
        body.start("process");
        body.addAttribute("name", (*it).second);
        body.addBody((*it).first);
        body.end();
        head.addBody(body.xml());
        body.clear();
    }
    _newProcCount = _pNewProcs->size();
    head.end();
    StoreResult("newProcesses.xml", head.xml());
      if (_summaryXML != 0)
        delete _summaryXML;
    _summaryXML = new string(GetSummaryReport());
    return head.xml();
}

//---------------<get summary xml string>---------------------------------------

ProcSniffer::XmlString ProcSniffer::SummaryReport()
{
    if(_summaryXML == 0)
        SnifferReport();
    return (*_summaryXML);
}

//----------------< get summary xml string>-----------------------------------

std::string ProcSniffer::GetSummaryReport() {
    XmlWriter head, body;
    head.start("Procs_Summary");
    body.start("new_processes");
    char buffer[33];
    sprintf(buffer, "%d", _newProcCount);
    body.addBody(buffer);
    body.end();
    head.addBody(body.xml());
    head.end();
    StoreResult("procSummary.xml", head.xml());
    return head.xml();
}

//-------------------< get previous result >-----------------------------------

std::string ProcSniffer::PreviousReport(ReportType type) {
    string xmlResult;
    switch (type) {
        case summary:
            xmlResult = ExtractResult("procSummary.xml");
            break;
        case complete:
            xmlResult = ExtractResult("compProcesses.xml");
            break;
        case new_only:
            xmlResult = ExtractResult("newProcesses.xml");
            break;
        default:
            return "No documentation for this type\n";
    }
    return xmlResult;
}

//-------------------<get record processes>-----------------------------------

void ProcSniffer::GetRecord() {
    string recXML = PreviousReport(complete);
    XmlReader xdr(recXML);
    if(_pRecord != 0)
        delete _pRecord;
    _pRecord = new std::map<std::string, std::string > ();
    while (xdr.next()) {
        if (xdr.tag().compare("process") == 0) {
            (*_pRecord)[xdr.body()] = xdr.attributes()[0].second;
        }
    }
    return;
}

//-------------------<store the processes>-------------------------------------

void ProcSniffer::StoreResult(const string filename, const string xmlReport) {
    ofstream out(filename.c_str());
    if (out.good()) {
        out << xmlReport;
        out.close();
    }
    return;
}

//------------------<extract the result>---------------------------------------

std::string ProcSniffer::ExtractResult(const std::string filename) {
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

void ProcSniffer::Print() {
    map<string, string>::iterator it;
    for (it = _pIDs_Names.begin(); it != _pIDs_Names.end(); it++) {
        cout << "PID is " << (*it).first << " The name is " << (*it).second << endl;
    }
    GetRecord();
    return;
}
