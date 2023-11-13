#include "fs.h"
#include <fstream>

namespace fs {

Result<core::String> read_to_string(const core::String& filename)
{
    std::ifstream f;
    f.open(filename);
    if (!f.is_open())
    {
        return Result<core::String>::Err(Error::FileNotFound);
    }
    f.seekg(0, std::ios::end);
    const usize size = f.tellg();
    f.seekg(0, std::ios::beg);
    core::String buf = core::String(size + 1, '\0');
    f.read(buf.data(), size);
    return Result<core::String>::Ok(buf);
}

Result<Unit> write(const core::String& filename, const core::String& content)
{
    std::ofstream f;
    f.open(filename);
    if (!f.is_open())
    {
        return Result<Unit>::Err(Error::FileNotFound);
    }
    f << content;
    return Result<Unit>::Ok({});
}

}

