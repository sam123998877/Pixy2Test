//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <signal.h>
#include "libpixyusb2.h"

Pixy2        pixy;
static bool  run_flag = true;

int Counter = 0 ;
int Num_class1 = 0 ;
int Num_class2 = 0 ;
int Num_class3 = 0 ;

void handle_SIGINT(int unused)
{
  // On CTRL+C - abort! //

  run_flag = false;
}

void  get_blocks()
{
  int  Block_Index;

  // Query Pixy for blocks //
  pixy.ccc.getBlocks();

  // Were blocks detected? //
  if (pixy.ccc.numBlocks)
  {
    // Blocks detected - calculation them! //
    
    Counter = 0 ;     // Reset "Not detected any block Counter"
    
    for (Block_Index = 0; Block_Index < pixy.ccc.numBlocks; ++Block_Index){
      if(pixy.ccc.blocks[Block_Index].report_age() == 5) //Block alive over 5 Frame, then record it.
      {
        if(pixy.ccc.blocks[Block_Index].report_signature() == 1)      //Red
          Num_class1 += 1 ;
        else if(pixy.ccc.blocks[Block_Index].report_signature() == 2) //Green
          Num_class2 += 1 ;
        else if(pixy.ccc.blocks[Block_Index].report_signature() == 3) //Yellow
          Num_class3 += 1 ;
      }
    }
  }
  else{     //Not detected any block
    Counter ++ ;      // "Not detected any block Counter" +1
    
    if (Counter == 1000){
      // No objects passed for a while... Show the result.
      printf ("Result:\n\r");
      printf ("\033[1;31Red:%d\033[0m \n\r",Num_class1);
      printf ("\033[1;32Green:%d\033[0m \n\r",Num_class2);
      printf ("\033[1;33Yellow:%d\033[0m \n\r",Num_class3);
    }
  }
}

int main()
{
  int  Result;

  // Catch CTRL+C (SIGINT) signals //
  signal (SIGINT, handle_SIGINT);

  printf ("=============================================================\n");
  printf ("= PIXY2 Get Blocks Demo                                     =\n");
  printf ("=============================================================\n");

  printf ("Connecting to Pixy2...");

  // Initialize Pixy2 Connection //
  {
    Result = pixy.init();

    if (Result < 0)
    {
      printf ("Error\n");
      printf ("pixy.init() returned %d\n", Result);
      return Result;
    }

    printf ("Success\n");
  }

  // Get Pixy2 Version information //
  {
    Result = pixy.getVersion();

    if (Result < 0)
    {
      printf ("pixy.getVersion() returned %d\n", Result);
      return Result;
    }

    pixy.version->print();
  }

  // Set Pixy2 to color connected components program //
  pixy.changeProg("color_connected_components");

  while(1)
  {
    get_blocks();

    if (run_flag == false)
    {
      // Exit program loop //
      break;
    }
  }

  printf ("PIXY2 Get Blocks Demo Exit\n");
}
