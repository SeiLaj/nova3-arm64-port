#pragma once
#include <cstdint>
#include <cstddef>

class CMemoryStream {
public:
    CMemoryStream(uint8_t *buf, size_t len, bool owner);
    ~CMemoryStream();

    bool BeginRead();
    void EndRead();

    int16_t ReadShort();
    int32_t ReadInt();
    // TODO: ReadString, ReadFloat, ReadBytes, Seek, Tell

private:
    uint8_t *m_buf;
    size_t m_len;
    size_t m_pos;
    bool m_owner;
    bool m_reading;
};
