#include <fwd/cgi_fcgx_wrapper.h>

// see fcgi-2.4.1/libfcgi/fcgi_studio.c
size_t FCGI_fwrite(const char * str, size_t size, size_t nmemb, FCGX_Stream *stream)
{
    int n;
    if(stream) {
        if((size * nmemb) == 0) {
            return 0;
        }
        n = FCGX_PutStr(str, size * nmemb, stream);
        return (n/size);
    }
    return (size_t)EOF;
}

int FCGI_fprintf(FCGX_Stream *stream, const char *format, ...)
{
    va_list ap;
    int n = 0;
    va_start(ap, format);
    n = FCGX_VFPrintF(stream, format, ap);
    va_end(ap);
    return n;
}
