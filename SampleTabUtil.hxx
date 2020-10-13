#ifdef _WIN32
#define TABUTIL_EXPORT __declspec(dllexport)
#else
#define TABUTIL_EXPORT
#endif

#include "DynVar.hxx"
#include <DpIdentifier.hxx>
#include <DpIdentifierItem.hxx>

extern "C"
{
TABUTIL_EXPORT int tabUtilTranslateDirection(const DynVar &in, DynVar &out);
TABUTIL_EXPORT int tabUtilBar               (const DynVar &in, DynVar &out);
TABUTIL_EXPORT int tabUtilConcate           (const DynVar &in, DynVar &out);
TABUTIL_EXPORT int tabUtilLastComment		(const DynVar& in, DynVar& out);
TABUTIL_EXPORT int tabUtilCustomerNumber	(const DpIdentifier& in, DynVar& out);
}
