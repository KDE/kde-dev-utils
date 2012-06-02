// Qt
#include <QStringList>
#include <QHash>
// C++
#include <stdio.h>
#include <stdlib.h>


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
   QHash<QString,int> dict;
   dict.reserve(20011);

   FILE *map_file = fopen(argv[1], "r");
   if (!map_file)
   {
      fprintf(stderr, "Error opening '%s'\n", argv[1]);
      return 1;
   }
   while(!feof(map_file))
   {
      fgets(buf, 1024, map_file);
      QString line = QString::fromLatin1(buf).trimmed();
      const QStringList split = line.split(QLatin1Char(' '), QString::SkipEmptyParts);
      if (split.count() <= 1)
         return 1;

      if (split.at(1) == QLatin1String("T"))
      {
         dict.insert(split.value(2), i);
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
      QString line = QString::fromLatin1(buf).trimmed();
      if (dict.contains(line))
      {
         qWarning("%s", qPrintable(line));
      }
   }
   fclose(call_file);
   return 0;
}

