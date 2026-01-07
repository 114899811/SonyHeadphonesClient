#include <mdr/ProtocolV1.hpp>

namespace mdr::v1
{
    bool NcAsmSetParam::Validate(const NcAsmSetParam&)
    {
        return true;
    }

    bool VptSetParam::Validate(const VptSetParam&)
    {
        return true;
    }
}
