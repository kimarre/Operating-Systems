#include <stdio.h>
#include <stdlib.h>

typedef struct {
   int num;
   char *name;
} Pet;

int main() {
   Pet *charlie = (Pet *)malloc(sizeof(Pet));
   charlie->num = 2;
   charlie->name = "Charlie-man";
   int num = charlie->num;
   char *name = charlie->name;
   int petSize = sizeof(Pet);

   Pet *moose = (Pet *)malloc(sizeof(Pet));
   moose->num = 3;
   moose->name = "Moose-cat";
   int num2 = moose->num;
   char *moosename = moose->name;

   Pet *maya = (Pet *)malloc(sizeof(Pet));
   maya->num = 2;
   maya->name = "Orange puppay/Peach";
   int mayaNum = maya->num;
   char *mayaname = maya->name;

   Pet *mocha = (Pet *)malloc(sizeof(Pet));
   mocha->num = 4;
   mocha->name = "Mocha moch";
   int mochanum = mocha->num;
   char *mochaname = mocha->name;

   // break for charlie location

   free(moose);
   charlie = realloc(charlie, sizeof(Pet)*2);

   // break for charlie location
   //should combine?
   return 0;
}
