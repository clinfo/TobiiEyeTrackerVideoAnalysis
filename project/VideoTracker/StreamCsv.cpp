#include "StreamCsv.h"
#include <fstream>
#include <stdlib.h>
using namespace std;

//
//Windows \r\n
//Linux \n
//MacOS9 \r:非対応

StreamCsv& endl(StreamCsv& riscsv) {
	int iChar = 0;
	while ((iChar = riscsv.m_ris.get()) != '\n'&& iChar != EOF) {}        // 行末まで読み飛ばす。
	iChar = riscsv.m_ris.peek();
	if (iChar == '\n'||iChar == '#') {//コメント行読み飛ばし
		riscsv.skips++;
		return endl(riscsv);
	}
	return riscsv;
}

StreamCsv& first_endl(StreamCsv& riscsv) {
	int iChar = 0;
	iChar = riscsv.m_ris.get();
	if (iChar == '\n'||iChar == '#') {//コメント行読み飛ばし
		riscsv.skips++;
		return endl(riscsv);
	}else{
		riscsv.m_ris.putback(iChar);
	}
	return riscsv;
}


StreamCsv::~StreamCsv(void)
{
}
StreamCsv::StreamCsv(istream& ris) : m_ris(ris) ,skips(0){}
StreamCsv& StreamCsv::operator >>(string& ri) {
	skips=0;
	ri = "";
	int iChar = 0;
	// 空白を読み飛ばす。
	//while (isspace(iChar = m_ris.get()) && iChar != '\n') {}
	while (' '==(iChar = m_ris.get()) && iChar != '\n') {}
	if (iChar == EOF ) { return *this; }
	string strValue;
	if (iChar == '\n'||iChar=='\t'||iChar=='\r') { m_ris.putback(iChar); iChar = 0; }
	else{ri += iChar;}
	// コンマまで読む。
	while ((iChar = m_ris.get()) != '\t' && iChar != EOF && iChar != '\n' && iChar!='\r') {
		ri+= iChar;
	}
	if (iChar == EOF ) { return *this;         }
	if (iChar == '\n'||iChar=='\r') { m_ris.putback(iChar); }
	return *this;
}
StreamCsv& StreamCsv::operator >>(double& ri) {
	skips=0;
	ri = 0;
	string strValue;
	this->operator>>(strValue);
	ri=strtod(strValue.c_str(),NULL);
	return *this;
}
StreamCsv& StreamCsv::operator >>(int& ri) {
	skips=0;
	ri = 0;
	string strValue;
	this->operator>>(strValue);
	ri=atoi(strValue.c_str());
	return *this;
}
// マニピュレータを受け入れるための挿入演算子
StreamCsv& StreamCsv::operator >>(StreamCsv& (*pmanipulatorfunction)(StreamCsv&)) {
	return (*pmanipulatorfunction)(*this);
}
