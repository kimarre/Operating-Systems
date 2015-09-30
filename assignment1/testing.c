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


   //second
   Pet *moose = (Pet *)malloc(70000);
   moose->num = 3;
   moose->name = "Moose-cat";

   int num2 = moose->num;
   char *moosename = moose->name;
   
   return 0;
}
