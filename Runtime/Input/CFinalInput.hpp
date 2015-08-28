#ifndef __RETRO_CFINALINPUT_HPP__
#define __RETRO_CFINALINPUT_HPP__

#include "../RetroTypes.hpp"

namespace Retro
{

class CFinalInput
{
    float x0_dt;
    u32 x4_controllerIdx;
    float x8_anaLeftX;
    float xc_anaLeftY;
    float x10_anaRightX;
    float x14_anaRightY;
    float x18_anaLeftTrigger;
    float x1c_anaRightTrigger;
    bool x20_enableAnaLeftXP;
    bool x21_enableAnaLeftYP;
    bool x22_enableAnaRightXP;
    bool x23_enableAnaRightYP;
    float x24_anaLeftTriggerP;
    float x28_anaRightTriggerP;

    bool x2c_b24_A:1;
    bool x2c_b25_B:1;
    bool x2c_b26_X:1;
    bool x2c_b27_Y:1;
    bool x2c_b28_Z:1;
    bool x2c_b29_L:1;
    bool x2c_b30_R:1;
    bool x2c_b31_DPUp:1;
    bool x2d_b24_DPRight:1;
    bool x2d_b25_DPDown:1;
    bool x2d_b26_DPLeft:1;
    bool x2d_b27_Start:1;

    bool x2d_b28_PA:1;
    bool x2d_b29_PB:1;
    bool x2d_b30_PX:1;
    bool x2d_b31_PY:1;
    bool x2e_b24_PZ:1;
    bool x2e_b25_PL:1;
    bool x2e_b26_PR:1;
    bool x2e_b27_PDPUp:1;
    bool x2e_b28_PDPRight:1;
    bool x2e_b29_PDPDown:1;
    bool x2e_b30_PDPLeft:1;
    bool x2e_b31_PStart:1;

public:
    bool PStart() const {return x2e_b31_PStart;}
    bool PR() const {return x2e_b26_PR;}
    bool PL() const {return x2e_b25_PL;}
    bool PZ() const {return x2e_b24_PZ;}
    bool PY() const {return x2d_b31_PY;}
    bool PX() const {return x2d_b30_PX;}
    bool PB() const {return x2d_b29_PB;}
    bool PA() const {return x2d_b28_PA;}
    bool PDPRight() const {return x2e_b28_PDPRight;}
    bool PDPLeft() const {return x2e_b30_PDPLeft;}
    bool PDPDown() const {return x2e_b29_PDPDown;}
    bool PDPUp() const {return x2e_b27_PDPUp;}
    bool PRTrigger() const {return x28_anaRightTriggerP > 0.050000001;}
    bool PLTrigger() const {return x24_anaLeftTriggerP > 0.050000001;}
    bool PRARight() const {return x22_enableAnaRightXP && x10_anaRightX > 0.69999999;}
    bool PRALeft() const {return x22_enableAnaRightXP && x10_anaRightX < -0.69999999;}
    bool PRADown() const {return x23_enableAnaRightYP && x14_anaRightY < -0.69999999;}
    bool PRAUp() const {return x23_enableAnaRightYP && x14_anaRightY > 0.69999999;}
    bool PLARight() const {return x20_enableAnaLeftXP && x8_anaLeftX > 0.69999999;}
    bool PLALeft() const {return x20_enableAnaLeftXP && x8_anaLeftX < -0.69999999;}
    bool PLADown() const {return x21_enableAnaLeftYP && xc_anaLeftY < -0.69999999;}
    bool PLAUp() const {return x21_enableAnaLeftYP && xc_anaLeftY > 0.69999999;}
    bool DStart() const {return x2d_b27_Start;}
    bool DR() const {return x2c_b30_R;}
    bool DL() const {return x2c_b29_L;}
    bool DZ() const {return x2c_b28_Z;}
    bool DY() const {return x2c_b27_Y;}
    bool DX() const {return x2c_b26_X;}
    bool DB() const {return x2c_b25_B;}
    bool DA() const {return x2c_b24_A;}
    bool DDPRight() const {return x2d_b24_DPRight;}
    bool DDPLeft() const {return x2d_b26_DPLeft;}
    bool DDPDown() const {return x2d_b25_DPDown;}
    bool DDPUp() const {return x2c_b31_DPUp;}
    bool DRTrigger() const {return x1c_anaRightTrigger > 050000001;}
    bool DLTrigger() const {return x18_anaLeftTrigger > 050000001;}
    bool DRARight() const {return x10_anaRightX > 0.69999999;}
    bool DRALeft() const {return x10_anaRightX < -0.69999999;}
    bool DRADown() const {return x14_anaRightY < -0.69999999;}
    bool DRAUp() const {return x14_anaRightY > 0.69999999;}
    bool DLARight() const {return x8_anaLeftX > 0.69999999;}
    bool DLALeft() const {return x8_anaLeftX < -0.69999999;}
    bool DLADown() const {return xc_anaLeftY < -0.69999999;}
    bool DLAUp() const {return xc_anaLeftY > 0.69999999;}
    float AStart() const {return x2d_b27_Start ? 1.0 : 0.0;}
    float AR() const {return x2c_b30_R ? 1.0 : 0.0;}
    float AL() const {return x2c_b29_L ? 1.0 : 0.0;}
    float AZ() const {return x2c_b28_Z ? 1.0 : 0.0;}
    float AY() const {return x2c_b27_Y ? 1.0 : 0.0;}
    float AX() const {return x2c_b26_X ? 1.0 : 0.0;}
    float AB() const {return x2c_b25_B ? 1.0 : 0.0;}
    float AA() const {return x2c_b24_A ? 1.0 : 0.0;}
    float ADPRight() const {return x2d_b24_DPRight ? 1.0 : 0.0;}
    float ADPLeft() const {return x2d_b26_DPLeft ? 1.0 : 0.0;}
    float ADPDown() const {return x2d_b25_DPDown ? 1.0 : 0.0;}
    float ADPUp() const {return x2c_b31_DPUp ? 1.0 : 0.0;}
    float ARTrigger() const {return x1c_anaRightTrigger;}
    float ALTrigger() const {return x18_anaLeftTrigger;}
    float ARARight() const {return x10_anaRightX > 0.0 ? x10_anaRightX : 0.0;}
    float ARALeft() const {return x10_anaRightX < 0.0 ? -x10_anaRightX : 0.0;}
    float ARADown() const {return x14_anaRightY < 0.0 ? -x14_anaRightY : 0.0;}
    float ARAUp() const {return x14_anaRightY > 0.0 ? x14_anaRightY : 0.0;}
    float ALARight() const {return x8_anaLeftX > 0.0 ? x8_anaLeftX : 0.0;}
    float ALALeft() const {return x8_anaLeftX < 0.0 ? -x8_anaLeftX : 0.0;}
    float ALADown() const {return xc_anaLeftY < 0.0 ? -xc_anaLeftY : 0.0;}
    float ALAUp() const {return xc_anaLeftY > 0.0 ? xc_anaLeftY : 0.0;}
};

}

#endif // __RETRO_CFINALINPUT_HPP__