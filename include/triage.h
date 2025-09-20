#ifndef TRIAGE_H
#define TRIAGE_H

void triage_banner(const char *banner, int port);
void triage_http_banner(const char *banner, int port);
void triage_ssh_banner(const char *banner, int port);
void triage_ftp_banner(const char *banner, int port);

#endif