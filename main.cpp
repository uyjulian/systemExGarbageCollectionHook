#include "ncbind/ncbind.hpp"
#include <algorithm>
#include <vector>

static std::vector<tTJSVariantClosure> TVPGarbageCollectionHandlerVector;

struct tTVPGarbageCollectionHookCallback : public tTVPCompactEventCallbackIntf
{
	virtual void TJS_INTF_METHOD OnCompact(tjs_int level)
	{
		tTJSVariant vlevel((tjs_int64)level);
		tTJSVariant *pvlevel = &vlevel;
		for (tjs_uint i = 0; i < TVPGarbageCollectionHandlerVector.size(); i++)
		{
			if (TVPGarbageCollectionHandlerVector[i].Object)
			{
				try
				{
					TVPGarbageCollectionHandlerVector[i].FuncCall(0, NULL, NULL, NULL, 1, &pvlevel, NULL);
				}
				catch(...)
				{
					// ignore
				}
			}
		}
	}
} static TVPGarbageCollectionHookCallback;

struct SystemExGarbageCollectionHook
{
	static void addGarbageCollectionHook(tTJSVariant variant)
	{
		tTJSVariantClosure clo = variant.AsObjectClosureNoAddRef();
		std::vector<tTJSVariantClosure>::iterator i;
		i = std::find(TVPGarbageCollectionHandlerVector.begin(),
			TVPGarbageCollectionHandlerVector.end(), clo);
		if (i == TVPGarbageCollectionHandlerVector.end())
		{
			clo.AddRef();
			TVPGarbageCollectionHandlerVector.push_back(clo);
		}
	}
	static void removeGarbageCollectionHook(tTJSVariant variant)
	{
		tTJSVariantClosure clo = variant.AsObjectClosureNoAddRef();
		std::vector<tTJSVariantClosure>::iterator i;
		i = std::find(TVPGarbageCollectionHandlerVector.begin(),
			TVPGarbageCollectionHandlerVector.end(), clo);
		if (i != TVPGarbageCollectionHandlerVector.end())
		{
			if (i->Object)
			{
				i->Release();
				i->Object = i->ObjThis = NULL;
			}

		}
	}

};

NCB_ATTACH_FUNCTION(addGarbageCollectionHook, System, SystemExGarbageCollectionHook::addGarbageCollectionHook);
NCB_ATTACH_FUNCTION(removeGarbageCollectionHook, System, SystemExGarbageCollectionHook::removeGarbageCollectionHook);

static void SystemExGarbageCollectionHook_PreRegistCallback(void)
{
	TVPAddCompactEventHook(&TVPGarbageCollectionHookCallback);
}

static void SystemExGarbageCollectionHook_PostUnregistCallback(void)
{
	TVPRemoveCompactEventHook(&TVPGarbageCollectionHookCallback);
	std::vector<tTJSVariantClosure>::iterator i;
	for (i = TVPGarbageCollectionHandlerVector.begin();
		i != TVPGarbageCollectionHandlerVector.end();
		i++)
	{
		if (i->Object)
		{
			i->Release();
			i->Object = i->ObjThis = NULL;
		}
	}
	TVPGarbageCollectionHandlerVector.clear();
}

NCB_PRE_REGIST_CALLBACK(SystemExGarbageCollectionHook_PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(SystemExGarbageCollectionHook_PostUnregistCallback);
