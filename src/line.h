#ifndef LINE_H
#define LINE_H

int read_line(BYTE *str, int size, FILE *fp);
void write_bad_line(FILE *fp, BYTE *str, int size, int line_num, int opt);
void write_bad_char(FILE *fp, BYTE *str, int size);
void results_header(FILE *fp);

#endif /* LINE_H */
