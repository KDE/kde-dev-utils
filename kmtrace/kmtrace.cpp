#include <qintdict.h>
#include <stdio.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <qtextstream.h>
#include <qsortedlist.h>
#include <qfile.h>
#include <stdlib.h>
#include <ktempfile.h>
#include <kinstance.h>
#include <kcmdlineargs.h>

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
QStrList *excludes = 0;

const char *unknown = "<unknown>";
const char *excluded = "<excluded>";
int allocCount = 0;
int leakedCount = 0;
int count = 0;
int maxCount;
int totalBytesAlloced = 0;
int totalBytesLeaked = 0;
int totalBytes = 0;
int maxBytes;

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
     totalBytesAlloced += entry->size;
     totalBytes += entry->size;
     count++;
     if (totalBytes > maxBytes)
        maxBytes = totalBytes;
     if (count > maxCount)
        maxCount = count;
     if (entryDict->find(entry->base))
        fprintf(stderr, "\rAllocated twice: 0x%08x                    \n", entry->base);
     entryDict->replace(entry->base, entry);
   } break;
   case '-':
   {
     int base = fromHex(cols[cols_count-1]);
     Entry *entry = entryDict->take(base);
     if (!entry)
     {
	if (base)
           fprintf(stderr, "\rFreeing unalloacted memory: 0x%08x                   \n", base);
     }
     else
     {
        totalBytes -= entry->size;
        count--;
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
      totalBytesLeaked += entry->total_size;
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

int lookupSymbols(FILE *stream)
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
        if (i & 1024)
        {
           fprintf(stderr, "\rLooking up symbols: %d found %d of %d symbols", i, symbols, symbolDict->count());
        }
     }
  }
  fprintf(stderr, "\rLooking up symbols: %d found %d of %d symbols\n", i, symbols, symbolDict->count());
  return symbolDict->count()-symbols;
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
         fprintf(fInputFile, "%08lx\n", it.currentKey());
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
         if (!fgets(buffer1, 1023, fInputFile)) continue;
         if (!fgets(buffer2, 1023, fInputFile)) continue;
         buffer1[strlen(buffer1)-1]=0;
         buffer2[strlen(buffer2)-1]=0;
         QCString symbol;
         symbol.sprintf("%s(%s)", buffer2, buffer1);
         symbolDict->insert(it2.currentKey(),qstrdup(symbol.data()));
      }
   }
   fclose(fInputFile);
}

int match(const char *s1, const char *s2)
{
  register int result;
  while(true)
  {
    result = *s1 - *s2;
    if (result)
       return result;
    s1++;
    s2++;
    if (!*s2) return 0;
    if (!*s1) return -1;
  }
  return 0;
}

const char *lookupAddress(int addr)
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
     int start = s.find('(');
     int end = s.findRev('+');
     if (end < 0)
        end = s.findRev(')');
     if ((start > 0) && (end > start))
     {
       QCString symbol = s.mid(start+1, end-start-1);
       char *res = 0;
       if (symbol.find(')') == -1)
          res = cplus_demangle(symbol.data(), DMGL_PARAMS | DMGL_ANSI);
       if (res)
       {
          symbol = res;
          free(res);
       }
       res = (char *) symbol.data();
       for(const char *it = excludes->first();it;it = excludes->next())
       {
          int i = match(res, it);
          if (i == 0)
          {
             formatDict->insert(addr,excluded);
             return excluded;
          }
       }
       s.replace(start+1, end-start-1, symbol);
     }
   }
   str = qstrdup(s.data());
   formatDict->insert(addr,str);
   return str;
}

void dumpBlocks()
{
   int filterBytes = 0;
   int filterCount = 0;
   for(Entry *entry = entryList->first();entry; entry = entryList->next())
   {
      bool exclude = false;
      for(int i = 0; entry->backtrace[i]; i++)
      {
         const char *str = lookupAddress(entry->backtrace[i]);
         if (str == excluded) 
         {
            entry->total_size = 0;
            continue;
         }
      }
      if (!entry->total_size) continue;
      filterBytes += entry->total_size;
      filterCount++;
   }
   printf("Leaked memory after filtering: %d bytes in %d blocks.\n", filterBytes, filterCount);
   for(Entry *entry = entryList->first();entry; entry = entryList->next())
   {
      if (!entry->total_size) continue;
      printf("[%d bytes in %d blocks, 1st. block is %d bytes at 0x%08x] ", entry->total_size, entry->count, entry->size, entry->base);
      printf("\n");
      for(int i = 0; entry->backtrace[i]; i++)
      {
         const char *str = lookupAddress(entry->backtrace[i]);
         printf("   0x%08x %s\n", entry->backtrace[i], str);
      }
   }
}

void readExcludeFile(const char *name)
{
   FILE *stream = fopen(name, "r");
   if (!stream) 
   {
      fprintf(stderr, "Error: Can't open %s.\n", name);
      exit(1);
   }
   char line[1024];
   while(!feof(stream))
   {
      if (!fgets(line, 1023, stream)) break;
      if ((line[0] == 0) || (line[0] == '#')) continue;
      line[strlen(line)-1] = 0;
      excludes->append(line);
fprintf(stderr, "Surpressing traces containing '%s' in output.\n", line);
   }
   fclose(stream);
   excludes->sort();
}

static KCmdLineOptions options[] =
{
  { "x", 0, 0 },
  { "exclude <file>", "File containing symbols to exclude from output.", 0},
  { "e", 0, 0 },
  { "exe <file>", "Executable to use for looking up unknown symbols.", 0},
  { "+<trace-log>", "Log file to investigate.", 0},
  { 0, 0, 0 }
};

int main(int argc, char *argv[])
{
  KInstance instance("kmtrace");

  KCmdLineArgs::init(argc, argv, "kmtrace", "KDE Memory leak tracer.", "v1.0");

  KCmdLineArgs::addCmdLineOptions(options);

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->count() != 1)
     KCmdLineArgs::usage();

  const char *logfile = args->arg(0);
  QCString exe = args->getOption("exe");
  QCString exclude = args->getOption("exclude");

  excludes = new QStrList;

  if (!exclude.isEmpty())
  {
     fprintf(stderr, "Reading %s\n", exclude.data());
     readExcludeFile(exclude);
  }

  FILE *stream = fopen(logfile, "r");
  if (!stream)
  {
     fprintf(stderr, "Can't open %s\n", logfile);
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
           fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d   ", allocCount, entryDict->count());
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
  leakedCount = count;
  fprintf(stderr, "\rTotal long term allocs: %d still allocated: %d(%d)   \n", allocCount, leakedCount, entryDict->count());
  printf("Totals allocated: %d bytes in %d blocks.\n", totalBytesAlloced, allocCount);
  printf("Maximum allocated: %d bytes / %d blocks.\n", maxBytes, maxCount);
  fprintf(stderr, "Collecting duplicates...\n");
  collectDupes();
  fprintf(stderr, "Sorting...\n");
  sortBlocks();
  printf("Totals leaked: %d bytes in %d blocks.\n", totalBytesLeaked, leakedCount);
  fprintf(stderr, "Looking up symbols...\n");
  rewind(stream);
  if (lookupSymbols(stream))
  {
     if (exe.isEmpty())
     {
        fprintf(stderr, "Use --exe option to resolve unknown symbols!\n");
        printf("Use --exe option to resolve unknown symbols!\n");
     }
     else
     {
        fprintf(stderr, "Looking up unknown symbols...\n");
        lookupUnknownSymbols(exe);
     }
  }
  else
  {
     fprintf(stderr, "All symbols found...\n");
  }
  fprintf(stderr, "Printing...\n");
  dumpBlocks();
  fprintf(stderr, "Done.\n");
  return 0;
}  
