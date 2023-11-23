#include "core.h"


namespace io {

struct Error {
    enum Kind { PlaceHolder } kind;

    const char* to_string() const {
        switch (kind) {
            case PlaceHolder:
                return "placeholder";
        }
    }
};

template<typename T>
using Result = core::Result<T, Error>;


struct Stdin;


struct Stdin {
    [[nodiscard]] Result<Unit> read_line(core::String& buffer);
};

}

