#include <qintdict.h>
#include <stdio.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qsortedlist.h>
#include <qfile.h>
#include <stdlib.h>
#include <ktempfile.h>
#include <kinstance.h>

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
  int signature;
  int count;
  int total_size;
  int backtrace[1];

  bool operator==(const Entry &e) { return total_size == e.total_size; }
  bool operator<(const Entry &e) { return total_size > e.total_size; }
};

QIntDict<Entry> *entryDict = 0;
QIntDict<char> *symbolDict = 0;
QIntDict<char> *formatDict = 0;
QSortedList<Entry> *entryList = 0;

const char *unknown = "<unknown>";
int allocCount = 0;

int fromHex(const char *str);
void parseLine(const QCString &_line, char operation);
void dumpBlocks();

int fromHex(const char *str)
{
   if (*str == '[') str++;
   str += 2; // SKip "0x"
   return strtol(str, NULL, 16);
}

// [address0][address1] .... [address] + base size
void parseLine(const QCString &_line, char operation)
{
  char *line= (char *) _line.data();
  const char *cols[100];
  int i = 0;
  cols[i++] = line;
  while(*line)
  {
     if (*line == ' ')
     {
        *line = 0;
        line++;
        while (*line && (*line==' ')) line++;
        if (*line) cols[i++] = line;
     }
     else line++;
  }  
  int cols_count = i;
  if (cols_count > 81) fprintf(stderr, "Error cols_count = %d\n", cols_count);
  if (cols_count < 4) return;
  switch (operation)
  {
   case '+':
   {
     Entry *entry = (Entry *) malloc((cols_count+3) *sizeof(int));
     entry->base = fromHex(cols[cols_count-2]);
     entry->size = fromHex(cols[cols_count-1]);
     int signature = 0;
     for(int i = cols_count-3; i--;)
     {
       signature += (entry->backtrace[i-1] = fromHex(cols[i]));
     }
     entry->signature = (signature / 4)+cols_count;
     entry->count = 1;
     entry->total_size = entry->size;
     entry->backtrace[cols_count-4] = 0;
     if (entryDict->find(entry->base))
        fprintf(stderr, "Allocated twice: 0x%08x\n", entry->base);
     entryDict->replace(entry->base, entry);
   } break;
   case '-':
   {
     int base = fromHex(cols[cols_count-1]);
     Entry *entry = entryDict->take(base);
     if (!entry)
     {
	if (base)
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

void collectDupes()
{
   QIntDict<Entry> dupeDict;
   QIntDictIterator<Entry> it(*entryDict);
   for(;it.current();)
   {
      Entry *entry = it.current();
      ++it;
      Entry *entry2 = dupeDict.find(entry->signature);
      if (entry2)
      {
         entry2->count++;
         entry2->total_size += entry->size;
         entryDict->remove(entry->base);
      }
      else
      {
         dupeDict.insert(entry->signature, entry);
      }
   }
}

void lookupSymbols(FILE *stream)
{
  int i = 0;
  int symbols = 0;
  char line2[1024]; 
  while(!feof(stream))
  {
     fgets(line2, 1023, stream);
     if (line2[0] == '/')
     {
        char *addr = index(line2, '[');
        if (addr)
        {
           long i_addr = fromHex(addr);
           const char* str = symbolDict->find(i_addr);
           if (str == unknown)
           {
               *addr = 0;
               char *str = qstrdup(line2);
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

void lookupUnknownSymbols(const char *appname)
{
   KTempFile inputFile;
   KTempFile outputFile;
//   inputFile.setAutoDelete();
//   outputFile.setAutoDelete();
   FILE *fInputFile = inputFile.fstream();
   QIntDict<char> oldDict = *symbolDict;
   QIntDictIterator<char> it(oldDict);
   for(;it.current(); ++it)
   {
      if (it.current() == unknown)
      {
         fprintf(fInputFile, "%08x\n", it.currentKey());
      }
   }
   inputFile.close();
   QCString command;
   command.sprintf("addr2line -e %s -f -C -s < %s > %s", appname, 
	QFile::encodeName(inputFile.name()).data(),
	QFile::encodeName(outputFile.name()).data());
fprintf(stderr, "Executing '%s'\n", command.data());
   system(command.data());
   fInputFile = fopen(QFile::encodeName(outputFile.name()), "r");
   if (!fInputFile)
   {
      fprintf(stderr, "Error opening temp file.\n");
      return;
   }
   QIntDictIterator<char> it2(oldDict);
   char buffer1[1024];
   char buffer2[1024];
   for(;it2.current(); ++it2)
   {
      if (feof(fInputFile))
      {
	fprintf(stderr, "Premature end of symbol output.\n");
        fclose(fInputFile);
        return;
      }
      if (it2.current() == unknown)
      {
         fgets(buffer1, 1023, fInputFile);
         fgets(buffer2, 1023, fInputFile);
         buffer1[strlen(buffer1)-1]=0;
         buffer2[strlen(buffer2)-1]=0;
         QCString symbol;
         symbol.sprintf("%s(%s)", buffer2, buffer1);
         symbolDict->insert(it2.currentKey(),qstrdup(symbol.data()));
      }
   }
   fclose(fInputFile);
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
      printf("[%d bytes in %d blocks, 1st. block is %d bytes at 0x%08x] ", entry->total_size, entry->count, entry->size, entry->base);
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
  if (argc != 3)
  {
     fprintf(stderr, "Usage: kmtrace <mtrace-file> <executable>\n");
     exit(1);
  }
  KInstance instance("kmtrace");
  FILE *stream = fopen(argv[1], "r");
  if (!stream)
  {
     fprintf(stderr, "Can't open %s\n", argv[1]);
     exit(1);
  }

  entryDict = new QIntDict<Entry>(9973);
  symbolDict = new QIntDict<char>(9973);
  formatDict = new QIntDict<char>(9973);
  entryList = new QSortedList<Entry>;
 
  fprintf(stderr, "Running\n");
  QCString line;
  char line2[1024];
  while(!feof(stream))
  {
     fgets(line2, 1023, stream);
     line2[strlen(line2)-1] = 0;
     if (line2[0] == '=') 
	printf("%s\n", line2);
     else if (line2[0] == '@')
        line = 0;
     else if (line2[0] == '[')
        line = line + ' ' + line2;
     else if (line2[0] == '/')
     {
        char *addr = index(line2,'[');
        if (addr)
        {
           line = line + ' ' + addr;
        }
     }
     else if (line2[0] == '+')
     {
        allocCount++;
        line = line + ' ' + line2;
        parseLine(line, '+');
        line = 0;
        if (allocCount & 128)
        {
           fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d", allocCount, entryDict->count());
        }
     }
     else if (line2[0] == '-')
     {
        line = line + ' ' + line2;
        parseLine(line, '-');
        line = 0;
     }
     else if (line2[0] == '<')
     {
        line2[0] = '-';
        // First part of realloc (free)
        QCString reline = line + ' ' + line2;
        parseLine(reline, '-');
     }
     else if (line2[0] == '>')
     {
        line2[0] = '+';
        // Second part of realloc (alloc)
        line = line + ' ' + line2;
        parseLine(line, '+');
        line = 0;
     }
  }
  fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d\n", allocCount, entryDict->count());
  printf("Total long term allocs: %d still allocated: %d\n", allocCount, entryDict->count());
  fprintf(stderr, "Collecting duplicates...\n");
  collectDupes();
  fprintf(stderr, "Sorting...\n");
  sortBlocks();
  fprintf(stderr, "Looking up symbols...\n");
  rewind(stream);
  lookupSymbols(stream);
  fprintf(stderr, "Looking up unknown symbols...\n");
  lookupUnknownSymbols(argv[2]);
  fprintf(stderr, "Printing...\n");
  dumpBlocks();
  fprintf(stderr, "Done.\n");
  return 0;
}  
