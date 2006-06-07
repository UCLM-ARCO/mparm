/***************************************************************************
                          trace.h  -  To create an XML trace file
                             -------------------
    begin                : Mon Dec 17 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __TRACE_H__
#define __TRACE_H__

#include <fstream.h>

class TraceFile
{
	public:
		TraceFile(const char *filename)
		{
			file = new ofstream(filename);
			(*file) << "<trace>" << "\n";
		}
		~TraceFile()
		{
			(*file) << "</trace>" << "\n";
			delete file;
		}
		
		void Begin(const char *tagName) { (*file) << "<" << tagName << ">" << "\n"; }
		void End(const char *tagName) { (* file) << "</" << tagName << ">" << "\n"; }
		
		friend TraceFile& operator << (TraceFile& tf, int v);
		friend TraceFile& operator << (TraceFile& tf, unsigned int v);
		friend TraceFile& operator << (TraceFile& tf, char v);
		friend TraceFile& operator << (TraceFile& tf, unsigned char v);
		friend TraceFile& operator << (TraceFile& tf, long v);
		friend TraceFile& operator << (TraceFile& tf, unsigned long v);
		friend TraceFile& operator << (TraceFile& tf, long long v);
		friend TraceFile& operator << (TraceFile& tf, unsigned long long v);
		friend TraceFile& operator << (TraceFile& tf, float v);
		friend TraceFile& operator << (TraceFile& tf, double v);
		friend TraceFile& operator << (TraceFile& tf, const char *s);

		void Value(const char *name, int value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, unsigned int value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, unsigned char value)
		{
			(*file) << "<" << name << ">" << (unsigned int) value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, char value)
		{
			(*file) << "<" << name << ">" << (int) value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, unsigned long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, unsigned long long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, long long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, float value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, double value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << "\n";
		}
		void Value(const char *name, const char *s)
		{
			(*file) << "<" << name << ">" << s << "</" << name << ">" << "\n";
		}
		void Value(const char *name, bool value)
		{
			(*file) << "<" << name << ">" << (value ? "true" : "false") << "</" << name << ">" << "\n";
		}
	
	private:
		ofstream *file;
};

//Poletti: ho aggiunto return tf; all'operatore <<
// non sono sicuro che vada bene ma lameno non mi da piu' 3400 varning in compilazione

inline TraceFile& operator << (TraceFile& tf, int v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, unsigned int v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, char v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, unsigned char v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, long v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, unsigned long v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, long long v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, unsigned long long v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, float v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, double v)
{
 (*tf.file) << v;
 return tf;
}

inline TraceFile& operator << (TraceFile& tf, const char *s)
{
 (*tf.file) << s;
 return tf;
}



extern TraceFile *trace_file;

#endif
