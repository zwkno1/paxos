#include "exception.h"

namespace detail
{

class Exception : public ::Exception
{
public:
    Exception(const char * where, const char * what)
        : where_(where)
        , what_(what)
    {
    }

    const char * where() const noexcept override
    {
        return where_;
    }

    const char * what() const noexcept override
    {
        return what_;
    }
private:
    const char * where_;

    const char * what_;
};

}

void throwException(const char *where, const char *what)
{
    throw detail::Exception(where, what);
}
