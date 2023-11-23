#include "core.h"

namespace fs
{

struct Error {
    enum Kind { FileNotFound } kind;

    const char* to_string() const {
        switch (kind) {
            case FileNotFound:
                return "file not found";
        }

        return "";
    }
};

template<typename T>
using Result = core::Result<T, Error>;

[[nodiscard]] Result<core::String> read_to_string(const core::String& filename);
[[nodiscard]] Result<Unit> write(const core::String& filename, const core::String& content);

}

