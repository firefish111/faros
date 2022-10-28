#include "text.h"
#include "util.h"
#include "hwinf.h"
#include "fs.h"
#include "ata.h"
#include "defs.h"
// #include "kbd.h"
// because of cyclic include, we declare what we want
void cpu_reset();

extern int prog(int arg);

#ifndef SHELL_H
#define SHELL_H

#define COM_LEN 16

char combuf[COM_LEN]; 

#define OUT_LEN 120
char outbuf[OUT_LEN];

void shexec() {
  unsigned char fmt = COLOUR(BLACK, WHITE);
  memzero(outbuf, OUT_LEN);
  if (strcmp(combuf, "info")) {
    fmt = COLOUR(RED, B_YELLOW); // fmt
    strcpy("FarOS v0.0.1.\n\tVol. label \"________________\"\n\tVol. ID 0x________________\n\tDisk __h\n\tVolume size ", outbuf);
    memcpy(&(csdfs -> label), outbuf + 27, 16); // memcpy because we need to control the length
    to_hex(&(csdfs -> vol_id), 16);
    strcpy(hexbuf, outbuf + 56);
    to_hex(&(hardware -> bios_disk), 2);
    strcpy(hexbuf, outbuf + 79);
    to_dec(csdfs -> fs_size * SECTOR_LEN);
    strcpy(decbuf, outbuf + 96); 
  } else if (strcmp(combuf, "cpu")) {
    fmt = COLOUR(YELLOW, B_GREEN); // fmt
    strcpy("CPUID.\n\t\x10 ____________\n\tFamily __h, Model __h, Stepping _h\n\tBrand \"________________________________________________\"", outbuf);
    memcpy(&(hardware -> vendor), outbuf + 10, 12); // cpu vendor

    to_hex(&(hardware -> c_family), 2); // family
    strcpy(hexbuf, outbuf + 31);
    to_hex(&(hardware -> c_model), 2); // model
    strcpy(hexbuf, outbuf + 42);
    to_hex(&(hardware -> c_stepping), 2); // stepping
    outbuf[56] = hexbuf[1]; // hack
    
    if (hardware -> cpuid_ext_leaves >= 0x80000004) {
      strcpy(&(hardware -> brand), outbuf + 67); // brand string
    }
  } else if (strcmp(combuf, "time")) {
    fmt = COLOUR(RED, B_CYAN);
    strcpy("Time since kernel load: _________ms", outbuf);
    to_dec(countx);
    strcpy(decbuf, outbuf + 24);
  } else if (strcmp(combuf, "reset")) {
    cpu_reset();
  } else if (strcmp(combuf, "clear")) {
    clear_scr();
    set_cur(POS(0, 0));
    return;
  } else if (memcmp(combuf, "exec", 4)) {
    read_pio28(0x100000, KERN_LEN, 1, hardware -> boot_disk_p.dev_path[0] & 0x01); // reads disk, has to get master or slave

    int ar = -1;
    if (strlen(combuf) >= 5) {
      ar = to_uint(combuf + 5);
    }

    int ret = prog(ar);
    if (ret) {
      fmt = COLOUR(BLACK, B_RED);
      strcpy("return code ", outbuf);
      to_dec(ret);
      strcpy(decbuf, outbuf + 12);
    }
  } else {
    fmt = COLOUR(MAGENTA, B_CYAN); // fmt
    strcpy(combuf, outbuf);
  }
  write_str(outbuf, fmt);
  line_feed();
}

void comupd() {
  if (strlen(combuf) >= COM_LEN) {
    write_str(" - FATAL: Command too long\n", COLOUR(BLACK, B_RED));
    memzero(combuf, COM_LEN);
  }

  int comlen = strlen(combuf);

  switch (combuf[comlen - 1]) {
  case '\b':
    combuf[comlen - 2] = '\0';
    combuf[comlen - 1] = '\0';
    clear_ln((get_cur() / 80));
    break;
  case '\n':
    line_feed();
    combuf[comlen - 1] = '\0';

    shexec();
    memzero(combuf, COM_LEN);
    break;
  }

  char printbuf[20] = "\r\x13> "; // 0x13 is the !! symbol
  strcpy(combuf, printbuf + 4);
  write_str(printbuf, COLOUR(BLACK, WHITE));
}

void shell() {
//  set_cur(POS(0, 1)); // new line
  char *headbuf = "FarSH. (c) 2022.\n"; // the underscores are placeholder for the memcpy
  write_str(headbuf, COLOUR(BLUE, B_RED));
//  write_hex(buf, -1);
  
  comupd();

  while (1) {
    asm volatile ("sti; hlt");
  }
}

#endif
