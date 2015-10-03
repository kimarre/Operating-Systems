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

   int i;
   int count = 100;
   Pet *animals[count];

   for(i = 0; i<count; i++) {
      animals[i] = (Pet *)malloc(sizeof(Pet));
   }

   for(i = 0; i<count; i++) {
      free(animals[i]);
   }



   // break for charlie location

   free(moose);
   charlie = realloc(charlie, sizeof(Pet)*2);

   // break for charlie location
   //should combine?

   // Make the compiler stop yelling at me for unused variables
   if (num || name || petSize || num2 || moosename || mayaNum || mayaname ||
    mochanum || mochaname) {}

   return 0;
}
