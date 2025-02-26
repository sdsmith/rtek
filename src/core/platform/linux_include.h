#include "core/logging/logging.h"

namespace rk::platform::linux {

struct Linux_Error_Short_Strs {
    std::array<const char *, 3> desc{nullptr};
};
/**
 * DOC(sdsmith):
 */
Linux_Error_Short_Strs linux_errno_to_short_str(int err) noexcept {
    // NOTE(sdsmith): It's unfortunate, but not every errno define has a unique value... for now assume that the first instance takes precedence.
    Linux_Error_Short_Strs strs = {};
    size_t idx = 0;

#define LINUX_ERR(ERROR)                                                                       \
    if (err == (ERROR)) {                                                                      \
        strs.desc[idx++] = #ERROR;                                                             \
        if (idx >= strs.desc.max_size()) {                                                     \
            LOG_WARN("more than {} dups for linux error code: {}", strs.desc.max_size(), err); \
            return strs;                                                                       \
        }                                                                                      \
    }
#include "core/platform/linux_errs.h"
#undef LINUX_ERR

    if (idx == 0) {
        LOG_WARN("unknown linux errno: {}", errno);
    }

    return strs;
}

} // namespace rk::platform::linux



/*
LINUX_ERR(EPERM               ,  1         )
LINUX_ERR(ENOENT              ,  2         )
LINUX_ERR(ESRCH               ,  3         )
LINUX_ERR(EINTR               ,  4         )
LINUX_ERR(EIO                 ,  5         )
LINUX_ERR(ENXIO               ,  6         )
LINUX_ERR(E2BIG               ,  7         )
LINUX_ERR(ENOEXEC             ,  8         )
LINUX_ERR(EBADF               ,  9         )
LINUX_ERR(ECHILD              , 10         )
LINUX_ERR(EAGAIN              , 11         )
LINUX_ERR(ENOMEM              , 12         )
LINUX_ERR(EACCES              , 13         )
LINUX_ERR(EFAULT              , 14         )
LINUX_ERR(ENOTBLK             , 15         )
LINUX_ERR(EBUSY               , 16         )
LINUX_ERR(EEXIST              , 17         )
LINUX_ERR(EXDEV               , 18         )
LINUX_ERR(ENODEV              , 19         )
LINUX_ERR(ENOTDIR             , 20         )
LINUX_ERR(EISDIR              , 21         )
LINUX_ERR(EINVAL              , 22         )
LINUX_ERR(ENFILE              , 23         )
LINUX_ERR(EMFILE              , 24         )
LINUX_ERR(ENOTTY              , 25         )
LINUX_ERR(ETXTBSY             , 26         )
LINUX_ERR(EFBIG               , 27         )
LINUX_ERR(ENOSPC              , 28         )
LINUX_ERR(ESPIPE              , 29         )
LINUX_ERR(EROFS               , 30         )
LINUX_ERR(EMLINK              , 31         )
LINUX_ERR(EPIPE               , 32         )
LINUX_ERR(EDOM                , 33         )
LINUX_ERR(ERANGE              , 34         )
LINUX_ERR(EDEADLK             , 35         )
LINUX_ERR(ENAMETOOLONG        , 36         )
LINUX_ERR(ENOLCK              , 37         )
LINUX_ERR(ENOSYS              , 38         )
LINUX_ERR(ENOTEMPTY           , 39         )
LINUX_ERR(ELOOP               , 40         )
LINUX_ERR(EWOULDBLOCK         , EAGAIN     )
LINUX_ERR(ENOMSG              , 42         )
LINUX_ERR(EIDRM               , 43         )
LINUX_ERR(ECHRNG              , 44         )
LINUX_ERR(EL2NSYNC            , 45         )
LINUX_ERR(EL3HLT              , 46         )
LINUX_ERR(EL3RST              , 47         )
LINUX_ERR(ELNRNG              , 48         )
LINUX_ERR(EUNATCH             , 49         )
LINUX_ERR(ENOCSI              , 50         )
LINUX_ERR(EL2HLT              , 51         )
LINUX_ERR(EBADE               , 52         )
LINUX_ERR(EBADR               , 53         )
LINUX_ERR(EXFULL              , 54         )
LINUX_ERR(ENOANO              , 55         )
LINUX_ERR(EBADRQC             , 56         )
LINUX_ERR(EBADSLT             , 57         )
LINUX_ERR(EDEADLOCK           , EDEADLK    )
LINUX_ERR(EBFONT              , 59         )
LINUX_ERR(ENOSTR              , 60         )
LINUX_ERR(ENODATA             , 61         )
LINUX_ERR(ETIME               , 62         )
LINUX_ERR(ENOSR               , 63         )
LINUX_ERR(ENONET              , 64         )
LINUX_ERR(ENOPKG              , 65         )
LINUX_ERR(EREMOTE             , 66         )
LINUX_ERR(ENOLINK             , 67         )
LINUX_ERR(EADV                , 68         )
LINUX_ERR(ESRMNT              , 69         )
LINUX_ERR(ECOMM               , 70         )
LINUX_ERR(EPROTO              , 71         )
LINUX_ERR(EMULTIHOP           , 72         )
LINUX_ERR(EDOTDOT             , 73         )
LINUX_ERR(EBADMSG             , 74         )
LINUX_ERR(EOVERFLOW           , 75         )
LINUX_ERR(ENOTUNIQ            , 76         )
LINUX_ERR(EBADFD              , 77         )
LINUX_ERR(EREMCHG             , 78         )
LINUX_ERR(ELIBACC             , 79         )
LINUX_ERR(ELIBBAD             , 80         )
LINUX_ERR(ELIBSCN             , 81         )
LINUX_ERR(ELIBMAX             , 82         )
LINUX_ERR(ELIBEXEC            , 83         )
LINUX_ERR(EILSEQ              , 84         )
LINUX_ERR(ERESTART            , 85         )
LINUX_ERR(ESTRPIPE            , 86         )
LINUX_ERR(EUSERS              , 87         )
LINUX_ERR(ENOTSOCK            , 88         )
LINUX_ERR(EDESTADDRREQ        , 89         )
LINUX_ERR(EMSGSIZE            , 90         )
LINUX_ERR(EPROTOTYPE          , 91         )
LINUX_ERR(ENOPROTOOPT         , 92         )
LINUX_ERR(EPROTONOSUPPORT     , 93         )
LINUX_ERR(ESOCKTNOSUPPORT     , 94         )
LINUX_ERR(EOPNOTSUPP          , 95         )
LINUX_ERR(EPFNOSUPPORT        , 96         )
LINUX_ERR(EAFNOSUPPORT        , 97         )
LINUX_ERR(EADDRINUSE  , 98         )
LINUX_ERR(EADDRNOTAVAIL       , 99         )
LINUX_ERR(ENETDOWN    , 100        )
LINUX_ERR(ENETUNREACH , 101        )
LINUX_ERR(ENETRESET   , 102        )
LINUX_ERR(ECONNABORTED        , 103        )
LINUX_ERR(ECONNRESET  , 104        )
LINUX_ERR(ENOBUFS             , 105        )
LINUX_ERR(EISCONN             , 106        )
LINUX_ERR(ENOTCONN    , 107        )
LINUX_ERR(ESHUTDOWN   , 108        )
LINUX_ERR(ETOOMANYREFS        , 109        )
LINUX_ERR(ETIMEDOUT   , 110        )
LINUX_ERR(ECONNREFUSED        , 111        )
LINUX_ERR(EHOSTDOWN   , 112        )
LINUX_ERR(EHOSTUNREACH        , 113        )
LINUX_ERR(EALREADY    , 114        )
LINUX_ERR(EINPROGRESS , 115        )
LINUX_ERR(ESTALE              , 116        )
LINUX_ERR(EUCLEAN             , 117        )
LINUX_ERR(ENOTNAM             , 118        )
LINUX_ERR(ENAVAIL             , 119        )
LINUX_ERR(EISNAM              , 120        )
LINUX_ERR(EREMOTEIO   , 121        )
LINUX_ERR(EDQUOT              , 122        )
LINUX_ERR(ENOMEDIUM   , 123        )
LINUX_ERR(EMEDIUMTYPE , 124        )
LINUX_ERR(ECANCELED   , 125        )
LINUX_ERR(ENOKEY              , 126        )
LINUX_ERR(EKEYEXPIRED , 127        )
LINUX_ERR(EKEYREVOKED , 128        )
LINUX_ERR(EKEYREJECTED        , 129        )
LINUX_ERR(EOWNERDEAD  , 130        )
LINUX_ERR(ENOTRECOVERABLE     , 131        )
LINUX_ERR(ERFKILL             , 132        )
LINUX_ERR(EHWPOISON   , 133        )

 */
