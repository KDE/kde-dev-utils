#include <qintdict.h>
#include <stdio.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qsortedlist.h>
#include <qfile.h>
#include <stdlib.h>


extern "C" {
/* Options passed to cplus_demangle (in 2nd parameter). */

#define DMGL_NO_OPTS	0		/* For readability... */
#define DMGL_PARAMS	(1 << 0)	/* Include function args */
#define DMGL_ANSI	(1 << 1)	/* Include const, volatile, etc */
#define DMGL_JAVA	(1 << 2)	/* Demangle as Java rather than C++. */

#define DMGL_AUTO	(1 << 8)
#define DMGL_GNU	(1 << 9)
#define DMGL_LUCID	(1 << 10)
#define DMGL_ARM	(1 << 11)
#define DMGL_HP 	(1 << 12)       /* For the HP aCC compiler; same as ARM
                                           except for template arguments, etc. */
#define DMGL_EDG	(1 << 13)

extern char *cplus_demangle(const char *mangled, int options);
}

struct Entry {
  int base;
  int size;
  int age;
  int backtrace[1];

  bool operator==(const Entry &e) { return size == e.size; }
  bool operator<(const Entry &e) { return size > e.size; }
};

QIntDict<Entry> *entryDict = 0;
QIntDict<char> *symbolDict = 0;
QIntDict<char> *formatDict = 0;
QSortedList<Entry> *entryList = 0;

const char *unknown = "<unknown>";
int allocCount = 0;

int fromHex(const QString &str);
void parseLine(const QString &line, char operation);
void dumpBlocks();

int fromHex(const QString &string)
{
   const char *str = string.latin1();
   if (*str == '[') str++;
   str += 2; // SKip "0x"
   return strtol(str, NULL, 16);
}

// @ [address0][address1] .... [address] + base size
void parseLine(const QString &line, char operation)
{
  QStringList cols = QStringList::split(' ', line);
  if (cols.count() < 5) return;
  if (cols[0] != "@") return;
  switch (operation)
  {
   case '+':
   {
     Entry *entry = (Entry *) malloc((cols.count()+1) *sizeof(int));
     entry->base = fromHex(cols[cols.count()-2]);
     entry->size = fromHex(cols[cols.count()-1]);
     entry->age = allocCount;
     for(int i = cols.count()-4; i > 0;i--)
     {
       entry->backtrace[i-1] = fromHex(cols[i]);
     }
     entry->backtrace[cols.count()-4] = 0;
     if (entryDict->find(entry->base))
        fprintf(stderr, "Allocated twice: 0x%08x\n", entry->base);
     entryDict->replace(entry->base, entry);
   } break;
   case '-':
   {
     int base = fromHex(cols[cols.count()-1]);
     Entry *entry = entryDict->take(base);
     if (!entry)
     {
        fprintf(stderr, "Freeing unalloacted memory: 0x%08x\n", base);
     }
     else
     {
        free(entry);
     }
   } break;
   default:
     break;
  }
}

void sortBlocks()
{
   QIntDictIterator<Entry> it(*entryDict);
   for(;it.current(); ++it)
   {
      Entry *entry = it.current();
      entryList->append(entry);
      for(int i = 0; entry->backtrace[i]; i++)
      {
         if (!symbolDict->find(entry->backtrace[i]))
             symbolDict->insert(entry->backtrace[i], unknown);
      }
   }
   entryList->sort();
}

void lookupSymbols(QTextStream &stream)
{
  int i = 0;
  int symbols = 0;
  while(!stream.atEnd())
  {
     QString line2 = stream.readLine();
     if (line2[0] == '/')
     {
        int i = line2.findRev('[');
        if (i>0)
        {
           QString addr = line2.mid(i);
           int i_addr = fromHex(addr);
           const char* str = symbolDict->find(i_addr);
           if (str == unknown)
           {
               char *str = qstrdup(line2.left(i).latin1());
               symbolDict->replace(i_addr, str);
               symbols++;
           }
        }
     }
     else if (line2[0] == '+')
     {
        i++;
        if (i & 128)
        {
           fprintf(stderr, "\rLooking up symbols: %d found %d of %d symbols", i, symbols, symbolDict->count());
        }
     }
  }
  fprintf(stderr, "\rLooking up symbols: %d found %d of %d symbols\n", i, symbols, symbolDict->count());
}

char *lookupAddress(int addr)
{
   char *str = formatDict->find(addr);
   if (str) return str;
   QCString s = symbolDict->find(addr);
   if (s.isEmpty())
   {
fprintf(stderr, "Error!\n");
     exit(1);
   }
   else
   {
     int start = s.findRev('(');
     int end = s.findRev('+');
     if (end < 0)
        end = s.findRev(')');
     if ((start > 0) && (end > start))
     {
       QCString symbol = s.mid(start+1, end-start-1);
       char *res = cplus_demangle(symbol.data(), DMGL_PARAMS | DMGL_ANSI);
       if (res)
       {
          symbol = res;
          free(res);
       }
       s.replace(start+1, end-start-1, symbol.data());
     }
   }
   str = qstrdup(s.data());
   symbolDict->insert(addr,str);
   return str;
}

void dumpBlocks()
{
   for(Entry *entry = entryList->first();entry; entry = entryList->next())
   {
      printf("[%d bytes of unfreed memory at 0x%08x]", entry->size, entry->base);
      printf("\n");
      for(int i = 0; entry->backtrace[i]; i++)
      {
         char *str = lookupAddress(entry->backtrace[i]);
         printf("   0x%08x %s\n", entry->backtrace[i], str);
      }
   }
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
     fprintf(stderr, "Usage: kmtrace <mtrace-file>\n");
     exit(1);
  }
  QString mtrace_filename = QFile::decodeName(argv[1]);
  QFile mtrace_file(mtrace_filename);
  if (!mtrace_file.open(IO_ReadOnly))
  {
     fprintf(stderr, "Can't open %s\n", mtrace_filename.local8Bit().data());
     exit(1);
  }

  entryDict = new QIntDict<Entry>(9973);
  symbolDict = new QIntDict<char>(9973);
  formatDict = new QIntDict<char>(9973);
  entryList = new QSortedList<Entry>;
 
  QTextStream stream( &mtrace_file );

  fprintf(stderr, "Running\n");
  QString line;
  while(!stream.atEnd())
  {
     QString line2 = stream.readLine();
     if (line2[0] == '=') 
	printf("%s\n", line2.latin1());
     else if (line2[0] == '@')
        line = line2;
     else if (line2[0] == '[')
        line = line + ' ' + line2;
     else if (line2[0] == '/')
     {
        int i = line2.findRev('[');
        if (i>0)
        {
           QString addr = line2.mid(i);
           line = line + ' ' + addr;
//           char *str = qstrdup(line2.left(i).latin1());
//           symbolDict->replace(fromHex(addr), str);
        }
     }
     else if (line2[0] == '+')
     {
        allocCount++;
        line = line + ' ' + line2;
        parseLine(line, '+');
        line = QString::null;
        if (allocCount & 128)
        {
           fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d", allocCount, entryDict->count());
        }
     }
     else if (line2[0] == '-')
     {
        line = line + ' ' + line2;
        parseLine(line, '-');
        line = QString::null;
     }
     else if (line2[0] == '<')
     {
        line2[0] = '-';
        // First part of realloc (free)
        QString reline = line + ' ' + line2;
        parseLine(reline, '-');
     }
     else if (line2[0] == '>')
     {
        line2[0] = '+';
        // Second part of realloc (alloc)
        line = line + ' ' + line2;
        parseLine(line, '+');
        line = QString::null;
     }
  }
  fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d", allocCount, entryDict->count());
  printf("Number of unfree'ed blocks: %d\n", entryDict->count());
  fprintf(stderr, "Sorting...\n");
  sortBlocks();
  fprintf(stderr, "Looking up symbols...\n");
  mtrace_file.reset();
  lookupSymbols(stream);
  fprintf(stderr, "Printing...\n");
  dumpBlocks();
  fprintf(stderr, "Done.\n");
  return 0;
}  
