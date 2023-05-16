#pragma once
#include <vector>
#include <algorithm>
using namespace std;

class StreamBuffer : public vector<char> {
public:
    StreamBuffer(){ m_curpos = 0; }
    StreamBuffer(const char* in, size_t len) {
        m_curpos = 0;
        insert(begin(), in, in + len);//use function of vector<char>
    }
    ~StreamBuffer(){ }; 

    void reset() { m_curpos = 0; }
    const char* data() { }
    const char* current() {}
    void offset(int k) { m_curpos += k;}
    bool is_eof() { return (m_curpos >= size()); }
    void input(char* in, size_t len) { insert(end(), in, in + len); }
    int findc(char c) {
        iterator it = find(begin() + m_curpos, end(), c);
        if (it != end()) {
            return it - (begin() + m_curpos);
        }
        return -1;
    }

private:
    unsigned int m_curpos;
};

class Serializer {
public:
    enum ByteOrder { BigEndian = 0, LittleEndian };
    Serializer() { m_byteorder = LittleEndian; };
	~Serializer() { };

private:
    int  m_byteorder;
};