#ifndef RAPID_XML_HELP_H
#define RAPID_XML_HELP_H
#include <iostream>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

class CRepidXmlHelp
{
public:
	CRepidXmlHelp(std::string strXmlFilePath);
	~CRepidXmlHelp();

	bool ParseXmlAndCreateDOM();
	bool FindNode(std::string strNodeName);
	bool EnterCurrentNode();
	bool ExitCurrentNode();
	
	
	char* GetAttrValue(std::string strAttrName);


private:
	bool           m_bHasXmlPath;
	std::string    m_strXmlPath;
	rapidxml::file<>         m_memFile;
	rapidxml::xml_document<> m_xmlDOM;  // xml document object model
	rapidxml::xml_node<>*    m_nodeRoot;
	rapidxml::xml_node<>*    m_nodeCurrent;

private:
};
#endif