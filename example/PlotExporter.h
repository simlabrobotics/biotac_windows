#ifndef __PLOTEXPORTER_H__
#define __PLOTEXPORTER_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

class PlotExporter
{
public:
    PlotExporter(const char* name, int ncols, const char* cols[], bool append = false)
    {
        sprintf(m_filename, "%s.dat", name);
        m_ncols = ncols;
        if (append)
            m_file = fopen(m_filename, "ab");
        else
            m_file = fopen(m_filename, "wb");

        AppendData("#%d", m_ncols);
        for (int i=0; i<m_ncols; i++)
            AppendData("#%s", cols[i]);
    }

    ~PlotExporter() 
    {
        if (m_file)
            fclose(m_file);
    }

    void AppendData(const char* fmt, ...)
    {
        if (!m_file) 
            return;

        va_list arg;
        va_start(arg, fmt);
        char buf[1024];
        int written = vsnprintf_s(buf, 1024, 1024 - 1, fmt, arg);
        if (written != -1)
        {
            fwrite(buf, sizeof(char), strlen(buf), m_file);
	        fwrite("\r\n", sizeof(char), 2, m_file);
        }
        va_end(arg);
    }

    void Flush()
    {
        if (m_file)
        {
            fclose(m_file);
            m_file = fopen(m_filename, "ab");
        }
    }

private:
    char m_filename[MAX_PATH];
    FILE* m_file;
    int m_ncols;
};

#endif // __PLOTEXPORTER_H__
