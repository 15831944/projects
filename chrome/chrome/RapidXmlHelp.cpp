#include "RapidXmlHelp.h"

CRepidXmlHelp::CRepidXmlHelp(std::string strXmlFilePath)
: m_memFile(strXmlFilePath.c_str())
, m_strXmlPath(strXmlFilePath)
{
}


CRepidXmlHelp::~CRepidXmlHelp()
{

}


bool CRepidXmlHelp::ParseXmlAndCreateDOM()
{
	bool bResult = false;
	do 
	{
		if(m_memFile.is_open())
		{
			m_xmlDOM.parse<0>(m_memFile.data());
			m_nodeRoot = m_xmlDOM.first_node();
			m_nodeCurrent = m_nodeRoot;

			//rapidxml::xml_node<>* node1 = m_nodeRoot->first_node("resourcemonitor");  
			//rapidxml::xml_node<>* node2 = node1->first_node();  
			////std::cout<<node1->name()<<std::endl;  

			//rapidxml::xml_attribute<char>* attr = node2->first_attribute("rmt");

			bResult = true; 
		}

	} while (false);

	return bResult;
}
