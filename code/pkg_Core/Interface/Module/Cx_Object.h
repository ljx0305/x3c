/*! \file Cx_Object.h
 *  \brief Define the regular implement template class: Cx_Object<cls>
 *  \author Zhang Yun Gui, X3 C++ PluginFramework
 *  \date   2010.10.19
 */
#ifndef X3_PLUGINIMPL_OBJECT_H_
#define X3_PLUGINIMPL_OBJECT_H_

#include "Ix_Object.h"
#include <typeinfo>

HMODULE x3GetModuleHandle();

/*! \ingroup _GROUP_PLUGIN_CORE2_
 *  \brief The regular implement template class used by XDEFINE_CLASSMAP_ENTRY.
 *  \note  The template parameter is a class which implement a interface.
 *  \see   Cx_SingletonObject, XDEFINE_CLASSMAP_ENTRY
 *  \internal
 */
template <class ClsType>
class Cx_Object
    : public ClsType
    , public Ix_Object
{
protected:
    Cx_Object(bool bRef = true) : m_refcount(bRef ? 1 : 0)
    {
        InterlockedIncrement(&ObjectCount());
    }

    virtual ~Cx_Object()
    {
        InterlockedDecrement(&ObjectCount());
    }

protected:
    virtual void AddRef(HMODULE fromdll)
    {
        if (fromdll != x3GetModuleHandle())
        {
            InterlockedIncrement(&RefCountByOthers());
        }
        InterlockedIncrement(&m_refcount);
    }

    virtual void Release(HMODULE fromdll)
    {
        if (fromdll != x3GetModuleHandle())
        {
            InterlockedDecrement(&RefCountByOthers());
        }
        if (0 == InterlockedDecrement(&m_refcount))
        {
            delete this;
        }
    }

    virtual const char* GetClassName() const
    {
        return typeid(*this).name();
    }

public:
    static Ix_Object* CreateObject(HMODULE fromdll)
    {
        Cx_Object<ClsType>* p = new Cx_Object<ClsType>(false);
        p->AddRef(fromdll);
        return p;
    }

    static long GetObjectCount()
    {
        return ObjectCount();
    }

    static long GetRefCountByOthers()
    {
        return RefCountByOthers();
    }

private:
    Cx_Object(const Cx_Object&);
    void operator=(const Cx_Object&);

    long        m_refcount;

    static long& ObjectCount()
    {
        static long s_objcount = 0;
        return s_objcount;
    }

    static long& RefCountByOthers()
    {
        static long s_refcount = 0;
        return s_refcount;
    }
};

#endif // X3_PLUGINIMPL_OBJECT_H_
