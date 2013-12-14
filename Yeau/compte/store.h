#ifndef _EAU_STORE_H_
#define _EAU_STORE_H_

namespace eau
{
    class StoreImpl
    {
    public:
        explicit StoreImpl();
        ~StoreImpl();


    private:
        StoreImpl(const StoreImpl &original);
        void operator =(const StoreImpl &);
    };

    class SmartStore : public StoreImpl<T>
    {
    private:
        SmartStore(const SmartStore &original);
        void operator =(const SmartStore &);
    };

    typedef SmartStore Store;

} // namespace eau

#endif
