#pragma once
#ifndef STREAMCSV_H_
#define STREAMCSV_H_
#include <fstream>
class StreamCsv {
    // �}�j�s�����[�^ endl �̒�`
    friend StreamCsv& endl(StreamCsv& riscsv);
    friend StreamCsv& first_endl(StreamCsv& riscsv);
private:
	std::istream& m_ris;     // ���͂Ɏg�� istream �̎Q��
public:
	int skips;
	StreamCsv(std::istream& ris);
	~StreamCsv(void);
	StreamCsv& operator >>(std::string& ri);
	StreamCsv& operator >>(int& ri);
	StreamCsv& operator >>(double& ri);
	// �}�j�s�����[�^���󂯓���邽�߂̑}�����Z�q
	StreamCsv& operator >>(StreamCsv& (*pmanipulatorfunction)(StreamCsv&));
};


StreamCsv& endl(StreamCsv& riscsv) ;
StreamCsv& first_endl(StreamCsv& riscsv) ;


#endif