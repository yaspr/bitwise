#include <stdlib.h>
#include <stdio.h>

void affiche_binaire(int M){
	
  char tabin[32];
  int i = 31;
  
  while(i >= 0)
    {
      
      if (M & (1u << 0))
	tabin[i] = 1;
      else
	tabin[i] = 0;
    
      i--;
      
      M = M >> 1;
      
    }
  
  printf("\nbinaire: ");

  for(i = 0; i < 32; i++)
    printf("%d", tabin[i]);
}


// 32 iterations max, variable
char pos_bit(int M){
  
  if(!M) exit(1);	
  
  char position = 31;
  
  while(-~position)
    {
      if(M & (1u << position))
	break;

      position = ~-position;
    }
  
  return position;
}



//5 iterations, constant
char pos_bit_opti(int M){
  
  if(!M) exit(1);
	
  //lm correspond à la "largeur" du masque
  int lm = 0x0000FFFF;
  int mask = 0xFFFF0000;
  int Mg = M;
  
  char pos = 0;
  int shift = 1u << 4;
  
  while(shift)
    {
      Mg = M & mask;
      
      if(Mg) pos += shift;
      
      //on "divise" le decalage par 2
      shift = shift >> 1;		
      //on reajuste la largeur du masque
      lm = lm >> shift;
      //on repositionne le masque
      mask = lm << (pos+shift);
    }
  
  return pos;
  
}

//
char pos_bit_best(int M){

  if (!M) exit(1);

  char pos = 0;

  pos  = ((M &  0xFFFF0000 )       !=0) << 4;
  pos += ((M & (0x0000FF00 << pos))!=0) << 3;
  pos += ((M & (0x000000F0 << pos))!=0) << 2;
  pos += ((M & (0x0000000C << pos))!=0) << 1;
  pos += ((M & (0x00000002 << pos))!=0);

  return pos;
}



int main(){
  
  char position;
  char posOpti;
  char posBest;
  int M = 0x0F008000;
  
  affiche_binaire(M);
  printf("\n\n");
  
  position = pos_bit(M);
  printf("\nPos. du bit le plus a gauche : 2^%d \n", position);
  
  posOpti = pos_bit_opti(M);
  printf("\nPos. Opti du bit le plus a g.: 2^%d \n", posOpti);
  
  posBest = pos_bit_best(M);
  printf("\nPos. Best du bit le plus a g.: 2^%d \n", posBest);
	
  return 0;	
}
