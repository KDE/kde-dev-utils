#include <tqintdict.h>
#include <stdio.h>
#include <tqstringlist.h>
#include <tqstrlist.h>
#include <tqtextstream.h>
#include <tqsortedlist.h>
#include <tqfile.h>
#include <tqtl.h>
#include <tqvaluelist.h>
#include <stdlib.h>
#include <ktempfile.h>
#include <kinstance.h>
#include <kstandarddirs.h>
#include <kcmdlineargs.h>

int main(int argc, char **argv)
{
   char buf[1024];
   if (argc != 3)
   {
      fprintf(stderr, "Usage: kmmatch <map-file> <call-file>\n");
      fprintf(stderr, "\n<map-file> is a file as output by 'nm'.\n");
      fprintf(stderr, "<call-file> is a file that contains symbols, e.g. a list of all\n"
                      "function calls made by a program.\n");
      fprintf(stderr, "The program will print all symbols from <call-file> that are present\n"
                      "in <map-file>, in the same order as they appear in <call-file>.\n");
      return 1;
   }
   
   int i = 1;
   TQDict<int> dict(20011);

   FILE *map_file = fopen(argv[1], "r");
   if (!map_file)
   {
      fprintf(stderr, "Error opening '%s'\n", argv[1]);
      return 1;
   }
   while(!feof(map_file))
   {
      fgets(buf, 1024, map_file);
      TQString line = TQString::tqfromLatin1(buf).stripWhiteSpace();
      TQStringList split = TQStringList::split(' ', line);
      if (split.count() <= 1)
         return 1;
         
      if (split[1] == "T")
      {
         dict.insert(split[2], &i);  
      }
   }
   fclose(map_file);
   
   FILE *call_file = fopen(argv[2], "r");
   if (!call_file)
   {
      fprintf(stderr, "Error opening '%s'\n", argv[2]);
      return 1;
   }
   
   while(!feof(call_file))
   {
      fgets(buf, 1024, call_file);
      TQString line = TQString::tqfromLatin1(buf).stripWhiteSpace();
      if (dict.tqfind(line))
      {
         qWarning("%s", line.latin1());
      }
   }
   fclose(call_file);
   return 0; 
}

