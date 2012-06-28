/* 
 * File:   ProcSniffer.h
 * Author: shilei
 *
 * Created on April 1, 2012, 11:48 AM
 */

#ifndef PROCSNIFFER_H
#define	PROCSNIFFER_H

#include <map>
#include <string>
#include <vector>

class ProcSniffer
{
public:
    typedef std::string XmlString;
    enum ReportType {summary, complete, new_only};
    ProcSniffer();
    ~ProcSniffer();
    void WriteShellResult(const std::string command, const std::string filename);
    void Print();
    std::string SnifferReport();
    std::string PreviousReport(ReportType type = new_only);
    XmlString WholeReport();
    std::string SummaryReport();
    std::vector<std::string> GetProcsFromReport(const XmlString& report, const std::string& tagName);
private:
    std::string CompeleteReport();
    XmlString GetSummaryReport();
    void GetProcesses();
    void GetRecord();
    void StoreResult(const std::string filename, const std::string xmlReport);
    std::string ExtractResult(const std::string filename);
    std::map<std::string,std::string> _pIDs_Names;
    std::map<std::string,std::string>* _pRecord;
    std::map<std::string,std::string>* _pNewProcs;
    std::string _recordFileName;
    XmlString *_summaryXML;
    int _newProcCount;
};

#endif	/* PROCSNIFFER_H */

