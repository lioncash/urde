#ifndef __URDE_CFLUIDPLANEMANAGER_HPP__
#define __URDE_CFLUIDPLANEMANAGER_HPP__

namespace urde
{

class CFluidPlaneManager
{
    bool x121_;
    class CFluidProfile
    {
    public:
        void Clear();
    };
    static CFluidProfile sProfile;
public:
    void StartFrame(bool);
};

}

#endif // __URDE_CFLUIDPLANEMANAGER_HPP__
