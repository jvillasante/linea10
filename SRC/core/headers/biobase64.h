#ifndef BIOBASE64_H
#define BIOBASE64_H
#include <QString>
class biobase64
{
public:
    biobase64();
    QString  base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    QString  base64_decode(std::string const& encoded_string);
    static inline bool is_base64(unsigned char c);
};

#endif // BIOBASE64_H
