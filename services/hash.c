#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../models_functions/hash.h"

char* hash(const char* input) {                                                                        
      if (!input) return NULL;                                                                           
      unsigned long h = 2166136261UL;  // FNV offset basis                                               
      while (*input)                                                                                   
          h = (h ^ (unsigned char)*input++) * 16777619UL;  // FNV prime                                  
                                                                       
      char* out = malloc(17);  // 16 hex chars + '\0'                                                    
      if (!out) return NULL;                                
      snprintf(out, 17, "%016lx", h);                                                                    
      return out;                                                                                        
  }  

