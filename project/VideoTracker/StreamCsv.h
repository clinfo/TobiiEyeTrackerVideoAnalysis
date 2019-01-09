#pragma once
#ifndef STREAMCSV_H_
#define STREAMCSV_H_
#include <fstream>
class StreamCsv {
    // マニピュレータ endl の定義
    friend StreamCsv& endl(StreamCsv& riscsv);
    friend StreamCsv& first_endl(StreamCsv& riscsv);
private:
	std::istream& m_ris;     // 入力に使う istream の参照
public:
	int skips;
	StreamCsv(std::istream& ris);
	~StreamCsv(void);
	StreamCsv& operator >>(std::string& ri);
	StreamCsv& operator >>(int& ri);
	StreamCsv& operator >>(double& ri);
	// マニピュレータを受け入れるための挿入演算子
	StreamCsv& operator >>(StreamCsv& (*pmanipulatorfunction)(StreamCsv&));
};


StreamCsv& endl(StreamCsv& riscsv) ;
StreamCsv& first_endl(StreamCsv& riscsv) ;


#endif