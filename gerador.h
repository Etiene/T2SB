typedef int (*funcp) ();
void gera(FILE *f, void **code, funcp *entry);
void cmd_ret(char v0, int i0, char v1, int i1, unsigned char * code, int * idx, int line);
void cmd_atr(char v0, int i0, char v1, int i1, char op, char v2, int i2, unsigned char * code, int * idx, int line);
void concat (unsigned char * code1, unsigned char * code2, int * idx, int n);