#include "../memring.h"
#include "../defs.h"

#ifndef SYS_MEMORY_H
#define SYS_MEMORY_H

// String Service 
void sys_1(struct cpu_state *c, unsigned char rout) {
  switch (rout) {

  // Allocate memory
  case 0x00:
    void * addr = malloc(c -> ecx);
    c -> edi = badj((unsigned int) addr);
    break;

  // Free memory
  case 0x01:
    free(adj(c -> edi), c -> ecx);
    break;

  default:
    break;
  }
}

#endif
