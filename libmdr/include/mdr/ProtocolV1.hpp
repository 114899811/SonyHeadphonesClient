#pragma once
#include "Protocol.hpp"

namespace mdr::v1
{
    enum class Command : UInt8
    {
        VPT_SET_PARAM = 72,
        NCASM_SET_PARAM = 104
    };

    enum class NcAsmInquiredType : UInt8
    {
        NO_USE = 0,
        NOISE_CANCELLING = 1,
        NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE = 2,
        AMBIENT_SOUND_MODE = 3
    };

    enum class NcAsmEffect : UInt8
    {
        OFF = 0,
        ON = 1,
        ADJUSTMENT_IN_PROGRESS = 16,
        ADJUSTMENT_COMPLETION = 17
    };

    enum class NcAsmSettingType : UInt8
    {
        ON_OFF = 0,
        LEVEL_ADJUSTMENT = 1,
        DUAL_SINGLE_OFF = 2
    };

    enum class AsmSettingType : UInt8
    {
        ON_OFF = 0,
        LEVEL_ADJUSTMENT = 1
    };

    enum class AsmId : UInt8
    {
        NORMAL = 0,
        VOICE = 1
    };

    enum class NcDualSingleValue : UInt8
    {
        OFF = 0,
        SINGLE = 1,
        DUAL = 2
    };

    enum class VptInquiredType : UInt8
    {
        NO_USE = 0,
        VPT = 1,
        SOUND_POSITION = 2
    };

    enum class VptPresetId : UInt8
    {
        OFF = 0,
        OUTDOOR_FESTIVAL = 1,
        ARENA = 2,
        CONCERT_HALL = 3,
        CLUB = 4
    };

    enum class SoundPositionPreset : UInt8
    {
        OFF = 0,
        FRONT_LEFT = 1,
        FRONT_RIGHT = 2,
        FRONT = 3,
        REAR_LEFT = 17,
        REAR_RIGHT = 18,
        OUT_OF_RANGE = 255
    };

#pragma pack(push, 1)
    struct NcAsmSetParam
    {
        Command command{Command::NCASM_SET_PARAM};
        NcAsmInquiredType inquiredType{NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE};
        NcAsmEffect ncAsmEffect{NcAsmEffect::OFF};
        NcAsmSettingType ncAsmSettingType{NcAsmSettingType::LEVEL_ADJUSTMENT};
        NcDualSingleValue dualSingleValue{NcDualSingleValue::OFF};
        AsmSettingType asmSettingType{AsmSettingType::LEVEL_ADJUSTMENT};
        AsmId asmId{AsmId::NORMAL};
        UInt8 asmLevel{};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(NcAsmSetParam);
    };

    struct VptSetParam
    {
        Command command{Command::VPT_SET_PARAM};
        VptInquiredType inquiredType{VptInquiredType::NO_USE};
        UInt8 preset{};

        MDR_DEFINE_TRIVIAL_SERIALIZATION(VptSetParam);
    };
#pragma pack(pop)
}

namespace mdr
{
    template <>
    struct MDRTraits<v1::NcAsmSetParam>
    {
        static constexpr MDRDataType kDataType = MDRDataType::DATA_MDR;
    };

    template <>
    struct MDRTraits<v1::VptSetParam>
    {
        static constexpr MDRDataType kDataType = MDRDataType::DATA_MDR;
    };
}
