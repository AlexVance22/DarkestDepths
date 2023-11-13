#include "rusttypes.h"

namespace fs
{

enum class Error {
    FileNotFound,
};

template<typename T>
using Result = core::Result<T, Error>;

[[nodiscard]] Result<core::String> read_to_string(const core::String& filename);
[[nodiscard]] Result<Unit> write(const core::String& filename, const core::String& content);

}

