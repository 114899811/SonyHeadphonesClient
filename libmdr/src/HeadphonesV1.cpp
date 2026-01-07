#include <algorithm>
#include <mdr/Headphones.hpp>

namespace mdr
{
    namespace
    {
        constexpr int kLegacyAsmMaxSteps = 21;
        constexpr int kLegacyAsmLevelDisabled = -1;
        v1::NcDualSingleValue GetDualSingleForAsmLevel(int asmLevel)
        {
            if (asmLevel > kLegacyAsmMaxSteps)
                MDR_CHECK_MSG(false, "Exceeded max ASM steps");
            if (asmLevel == 1)
                return v1::NcDualSingleValue::SINGLE;
            if (asmLevel == 0)
                return v1::NcDualSingleValue::DUAL;
            return v1::NcDualSingleValue::OFF;
        }
    }

    MDRTask MDRHeadphones::RequestInitV1()
    {
        mProtocolVersion = MDRProtocolVersion::V1;
        mProtocol = {};
        std::ranges::fill(mSupport.table1Functions, false);
        std::ranges::fill(mSupport.table2Functions, false);
        mModelName.clear();
        mFWVersion.clear();
        mUniqueId.clear();

        mLegacyAmbientSoundControl.overwrite(false);
        mLegacyFocusOnVoice.overwrite(false);
        mLegacyAsmLevel.overwrite(1);
        mLegacyVptType.overwrite(static_cast<int>(v1::VptPresetId::OFF));
        mLegacySurroundPosition.overwrite(v1::SoundPositionPreset::OFF);
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }

    MDRTask MDRHeadphones::RequestCommitV1()
    {
        if (mLegacyAmbientSoundControl.dirty() || mLegacyFocusOnVoice.dirty() || mLegacyAsmLevel.dirty())
        {
            int asmLevel = mLegacyAmbientSoundControl.desired ? mLegacyAsmLevel.desired : kLegacyAsmLevelDisabled;
            MDR_CHECK_MSG(asmLevel >= kLegacyAsmLevelDisabled, "Invalid ASM level");
            v1::NcAsmSetParam res;
            res.inquiredType = v1::NcAsmInquiredType::NOISE_CANCELLING_AND_AMBIENT_SOUND_MODE;
            res.ncAsmEffect = mLegacyAmbientSoundControl.desired ? v1::NcAsmEffect::ADJUSTMENT_COMPLETION : v1::NcAsmEffect::OFF;
            res.ncAsmSettingType = v1::NcAsmSettingType::LEVEL_ADJUSTMENT;
            res.dualSingleValue = GetDualSingleForAsmLevel(asmLevel);
            res.asmSettingType = v1::AsmSettingType::LEVEL_ADJUSTMENT;
            res.asmId = mLegacyFocusOnVoice.desired ? v1::AsmId::VOICE : v1::AsmId::NORMAL;
            res.asmLevel = static_cast<UInt8>(asmLevel - 1);
            SendCommandACK(v1::NcAsmSetParam, res);
            mLegacyAmbientSoundControl.commit();
            mLegacyFocusOnVoice.commit();
            mLegacyAsmLevel.commit();
        }

        if (mLegacyVptType.dirty() || mLegacySurroundPosition.dirty())
        {
            v1::VptSetParam res;
            int desiredVpt = mLegacyVptType.desired;
            v1::SoundPositionPreset desiredPos = mLegacySurroundPosition.desired;
            if (desiredVpt != static_cast<int>(v1::VptPresetId::OFF))
            {
                res.inquiredType = v1::VptInquiredType::VPT;
                res.preset = static_cast<UInt8>(desiredVpt);
            }
            else if (desiredPos != v1::SoundPositionPreset::OFF)
            {
                res.inquiredType = v1::VptInquiredType::SOUND_POSITION;
                res.preset = static_cast<UInt8>(desiredPos);
            }
            else
            {
                if (mLegacySurroundPosition.current != v1::SoundPositionPreset::OFF)
                {
                    res.inquiredType = v1::VptInquiredType::SOUND_POSITION;
                    res.preset = static_cast<UInt8>(v1::SoundPositionPreset::OFF);
                }
                else if (mLegacyVptType.current != static_cast<int>(v1::VptPresetId::OFF))
                {
                    res.inquiredType = v1::VptInquiredType::VPT;
                    res.preset = static_cast<UInt8>(v1::VptPresetId::OFF);
                }
                else
                {
                    mLegacyVptType.commit();
                    mLegacySurroundPosition.commit();
                    co_return MDR_HEADPHONES_TASK_COMMIT_OK;
                }
            }
            SendCommandACK(v1::VptSetParam, res);
            mLegacyVptType.commit();
            mLegacySurroundPosition.commit();
        }
        co_return MDR_HEADPHONES_TASK_COMMIT_OK;
    }

    MDRTask MDRHeadphones::RequestInitAuto()
    {
        mProtocolVersion = MDRProtocolVersion::UNKNOWN;
        mProtocol = {};
        std::ranges::fill(mSupport.table1Functions, false);
        std::ranges::fill(mSupport.table2Functions, false);

        SendCommandACK(v2::t1::ConnectGetProtocolInfo);
        int res = co_await Await(AWAIT_PROTOCOL_INFO);
        if (res != MDR_RESULT_OK || !mProtocol.hasTable1)
        {
            co_await RequestInitV1();
            co_return MDR_HEADPHONES_TASK_INIT_OK;
        }
        mProtocolVersion = MDRProtocolVersion::V2;
        co_await RequestInitV2AfterProtocolInfo();
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSyncAuto()
    {
        if (mProtocolVersion == MDRProtocolVersion::V1)
        {
            co_await RequestSyncV1();
            co_return MDR_HEADPHONES_TASK_SYNC_OK;
        }
        co_await RequestSyncV2();
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }

    MDRTask MDRHeadphones::RequestCommitAuto()
    {
        if (mProtocolVersion == MDRProtocolVersion::V1)
        {
            co_await RequestCommitV1();
            co_return MDR_HEADPHONES_TASK_COMMIT_OK;
        }
        co_await RequestCommitV2();
        co_return MDR_HEADPHONES_TASK_COMMIT_OK;
    }
}
