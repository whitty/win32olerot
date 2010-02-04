/*
 */

#include <atlbase.h>
#include "ruby.h"
#include <windows.h>
#include <ocidl.h>
#include <olectl.h>
#include <ole2.h>
#include <stdarg.h>

class HRESULTDecode
{
public:
    HRESULTDecode(HRESULT aResult)
    :   mResult(aResult) {}

    HRESULTDecode()
    :   mResult(S_OK) {}

    // allow as drop-in replacement for HRESULT
    operator HRESULT&() { return mResult; }

    VALUE code() const
    {
        return INT2FIX(mResult);
    }

    VALUE message() const
    {
        char *pMessage = NULL;
        DWORD count = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                    FORMAT_MESSAGE_FROM_SYSTEM |
                                    FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, mResult, LOCALE_SYSTEM_DEFAULT,
                                    (LPTSTR)&pMessage, 0, NULL);
        if (count == 0)
        {
            return Qnil;
        }

        // remove newlines from the end
        char *curr = pMessage + strlen(pMessage);
        while (pMessage <= --curr)
        {
            // TODO - review
//         while (pMessage < curr)
//         {
//             curr--;
            if (*curr == '\r' || *curr == '\n')
                *curr = '\0';
            else
                break;
        }
        VALUE result = Qnil;
        if (pMessage[0] != '\0')
        {
            result = rb_str_new2(pMessage);
        }
        LocalFree(pMessage);
        return result;
    }

private:
    HRESULT mResult;
};


/* Pseudo consts */
static VALUE cWIN32OLE;
static VALUE eWIN32OLE_RUNTIME_ERROR; // TODO - define own exceptions
static VALUE cWIN32OLE_ROT;

class WIN32OLE_RunningObjectTable
{
public:
    WIN32OLE_RunningObjectTable()
    :   pRunningObjectTable(0)
    {
        HRESULTDecode hr;
        if (mCount++ == 0)
        {
            hr = OleInitialize(NULL);
            if(FAILED(hr))
            {
                VALUE message = hr.message();
                rb_raise(eWIN32OLE_RUNTIME_ERROR,
                         "Failed to Initialise OLE/COM: %s", StringValuePtr(message));
            }
        }

        hr = ::GetRunningObjectTable(0, &pRunningObjectTable);
        if (FAILED(hr))
        {
            VALUE message = hr.message();
            rb_raise(eWIN32OLE_RUNTIME_ERROR,
                     "Failed to retrieve RunningObjectTable: %s", StringValuePtr(message));
        }
    }

    ~WIN32OLE_RunningObjectTable()
    {
        if (pRunningObjectTable)
            pRunningObjectTable->Release();

        if (--mCount == 0)
        {
            OleUninitialize();
        }
    }

    VALUE isRunning(VALUE aMoniker, VALUE aInvalidMonikersArentRunning)
    {
        VALUE result = Qfalse;

        CComPtr<IBindCtx> pBindCtx;
        HRESULTDecode hr = CreateBindCtx(0, &pBindCtx);
        if(FAILED(hr))
        {
            VALUE message = hr.message();
            rb_raise(eWIN32OLE_RUNTIME_ERROR,
                     "Failed to create bind context: %s", StringValuePtr(message));
        }

        const int size = ::MultiByteToWideChar(CP_ACP, 0, StringValuePtr(aMoniker), -1, NULL, 0);
        OLECHAR *pBuf = SysAllocStringLen(NULL, size - 1);
        ::MultiByteToWideChar(CP_ACP, 0, StringValuePtr(aMoniker), -1, pBuf, size);

        CComPtr<IMoniker> pMoniker;
        ULONG ignored = 0;
        hr = MkParseDisplayName(pBindCtx, pBuf, &ignored, &pMoniker);
        ::SysFreeString(pBuf);

        if (FAILED(hr))
        {
            if (RTEST(aInvalidMonikersArentRunning))
                return result;

            VALUE message = hr.message();
            rb_raise(eWIN32OLE_RUNTIME_ERROR,
                     "Failed to parse display name of moniker '%s': %s",
                     StringValuePtr(aMoniker),
                     StringValuePtr(message));
        }

        hr = pRunningObjectTable->IsRunning(pMoniker);
        if (hr == S_OK)
        {
            result = Qtrue;
        }
        return result;
    }

    VALUE eachDisplayName()
    {
        CComPtr<IEnumMoniker> pEnumMoniker;
        HRESULTDecode hr = pRunningObjectTable->EnumRunning(&pEnumMoniker);

        if (!SUCCEEDED(hr))
        {
            VALUE message = hr.message();
            rb_raise(eWIN32OLE_RUNTIME_ERROR,
                      "Failed to enumerate running monikers: %s",
                     StringValuePtr(message));
        }

        CComPtr<IBindCtx> pBindCtx;
        hr = CreateBindCtx(0, &pBindCtx);
        if(FAILED(hr)) {
            VALUE message = hr.message();
            rb_raise(eWIN32OLE_RUNTIME_ERROR,
                      "Failed to create bind context: %s",
                     StringValuePtr(message));
        }

        VALUE result = Qnil;
        CComPtr<IMoniker> pMoniker = 0;
        hr = pEnumMoniker->Next(1, &pMoniker, 0);
        while (hr == S_OK)
        {
            LPOLESTR pDisplayNameWide;
            if (SUCCEEDED(pMoniker->GetDisplayName(pBindCtx, 0,
                                                   &pDisplayNameWide)))
            {
                int size = ::WideCharToMultiByte(CP_ACP, 0, pDisplayNameWide, -1, NULL, 0, NULL, NULL);
                if (size)
                {
                    char* pDisplayName = ALLOC_N(char, size);
                    ::WideCharToMultiByte(CP_ACP, 0, pDisplayNameWide, -1, pDisplayName, size, NULL, NULL);

                    // Only provide monikers that can be looked up by
                    // display-name
                    CComPtr<IMoniker> pMonikerLookup;
                    ULONG ignored = 0;
                    hr = MkParseDisplayName(pBindCtx, pDisplayNameWide, &ignored, &pMonikerLookup);

                    if(SUCCEEDED(hr))
                    {
                        result = rb_yield(rb_str_new2(pDisplayName));
                    }
                    xfree(pDisplayName);
                }

                ::CoTaskMemFree(pDisplayNameWide);
            }
            hr = pEnumMoniker->Next(1, &pMoniker, 0);
        }
        return result;
    }

private:
    IRunningObjectTable* pRunningObjectTable;
    static unsigned long mCount;
};
unsigned long WIN32OLE_RunningObjectTable::mCount = 0;

static void rot_mark(WIN32OLE_RunningObjectTable* t)
{
}

static void rot_free(WIN32OLE_RunningObjectTable* t)
{
    delete t;
}

static VALUE rot_allocate(VALUE klass)
{
    WIN32OLE_RunningObjectTable* table = new (WIN32OLE_RunningObjectTable);
    if (!table)
        rb_raise(rb_eNoMemError, "Failed to alloc RunningObjectTable");

    return Data_Wrap_Struct(klass, rot_mark, rot_free, table);
}

static VALUE rot_isRunning(VALUE rot, VALUE vArray)
{
    VALUE moniker = rb_ary_shift(vArray);
    if (NIL_P(moniker))
    {
        rb_raise(rb_eArgError, "Missing required parameter 'moniker_name'");
    }

    VALUE exceptionsDontError = Qtrue;

    VALUE options = rb_ary_shift(vArray);
    if (!NIL_P(options))
    {
        VALUE optValue = rb_funcall(options, rb_intern("fetch"),
                                    2, ID2SYM(rb_intern("raise_exception")), Qnil);
        if (RTEST(optValue))
        {
            exceptionsDontError = Qfalse;
        }
    }

    WIN32OLE_RunningObjectTable* pRot;
    Data_Get_Struct(rot, WIN32OLE_RunningObjectTable, pRot);
    return pRot->isRunning(moniker, exceptionsDontError);
}

static VALUE rot_each_display_name(VALUE rot)
{
    WIN32OLE_RunningObjectTable* pRot;
    Data_Get_Struct(rot, WIN32OLE_RunningObjectTable, pRot);
    return pRot->eachDisplayName();
}

void
Init_win32olerot()
{
    (void)rb_require("win32ole");

    // fetch OLE constants
    cWIN32OLE = rb_const_get(rb_cObject, rb_intern("WIN32OLE"));
    eWIN32OLE_RUNTIME_ERROR = rb_const_get(rb_eRuntimeError, rb_intern("WIN32OLERuntimeError"));
    if (NIL_P(cWIN32OLE) || NIL_P(eWIN32OLE_RUNTIME_ERROR))
    {
        rb_raise(rb_eLoadError, "Failed to load prerequisite module 'win32ole'");
        return;
    }

    cWIN32OLE_ROT = rb_define_class_under(cWIN32OLE, "RunningObjectTable", rb_cObject);
    rb_define_alloc_func(cWIN32OLE_ROT, rot_allocate);
    rb_define_method(cWIN32OLE_ROT, "is_running?", RUBY_METHOD_FUNC(rot_isRunning), -2);
    rb_define_method(cWIN32OLE_ROT, "each_display_name", RUBY_METHOD_FUNC(rot_each_display_name), 0);
    rb_define_alias(cWIN32OLE_ROT, "each", "each_display_name");
}
