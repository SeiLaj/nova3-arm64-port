#include "cmemory_stream.h"
#include <cstring>

CMemoryStream::CMemoryStream(uint8_t *buf, size_t len, bool owner)
    : m_buf(buf), m_len(len), m_pos(0), m_owner(owner), m_reading(false) {}

CMemoryStream::~CMemoryStream() {
    if (m_owner && m_buf) delete[] m_buf;
}

bool CMemoryStream::BeginRead() {
    m_reading = true;
    m_pos = 0;
    return true;
}

void CMemoryStream::EndRead() {
    m_reading = false;
}

int16_t CMemoryStream::ReadShort() {
    if (!m_reading || m_pos + 2 > m_len) return 0;
    int16_t val = m_buf[m_pos] | (m_buf[m_pos+1] << 8);  // little-endian
    m_pos += 2;
    return val;
}

int32_t CMemoryStream::ReadInt() {
    if (!m_reading || m_pos + 4 > m_len) return 0;
    int32_t val = m_buf[m_pos] | (m_buf[m_pos+1] << 8)
                | (m_buf[m_pos+2] << 16) | (m_buf[m_pos+3] << 24);
    m_pos += 4;
    return val;
}
