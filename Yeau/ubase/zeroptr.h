#ifndef _ZEROPTR_H_
#define _ZEROPTR_H_

namespace eau
{
    /**
     * class zeroptr is using with class RefCount/RefCounted
     * The detail is shown in refcount.h 
     */

    template <class T> class zeroptr
    {
    public:
        zeroptr() : _ptr(NULL) {}

        zeroptr(T* ptr) : _ptr(ptr)
        {
            if (_ptr)   _ptr->AddRef();
        }

        zeroptr(const zeroptr<T> &zptr) : _ptr(zptr._ptr)
        {
            if (_ptr)   _ptr->AddRef();
        }

        template <typename U> zeroptr(const zeroptr<U> &zptr) : _ptr(zptr.get())
        {
            if (_ptr)   _ptr->AddRef();
        }

        virtual ~zeroptr()
        {
            if (_ptr)   _ptr->Release();
        }

        T* get() const {return _ptr;}
        operator T*() const {return _ptr;}
        T* operator ->() const {return _ptr;}

        T* release()
        {
            T* ptr = _ptr;
            _ptr = NULL;
            return ptr;
        }

        zeroptr<T> &operator =(T* ptr)
        {
            if (ptr)    ptr->AddRef();
            if (_ptr)   _ptr->Release();
            _ptr = ptr;
            return *this;
        }
    
        zeroptr<T> &operator =(const zeroptr<T> &zptr)
        {
            return *this = zptr._ptr;
        }

        template <typename U> zeroptr<T> &operator =(const zeroptr<U> &zptr)
        {
            return *this = zptr.get();
        }

        void swap(T** pptr)
        {
            T* ptr = _ptr;
            _ptr = *pptr;
            *pptr = ptr; 
        }

        void swap(zeroptr<T> &zptr)
        {
            swap(&zptr._ptr);
        }

    protected:
        T* _ptr;
    }; // class zeroptr 

} // namespace eau

#endif

