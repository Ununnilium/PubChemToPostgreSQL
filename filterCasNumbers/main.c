/**
 * @file main.c
 * @author  Fabian Heller <heller.fabian@gmail.com>
 *
 * @section LICENSE
 *
 * Copyright (c) 2016, Fabian Heller
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * Checks if a name in the PubChem synonym file is a CAS number. If yes,
 * this line is written to STDOUT. The PubChem synonym file can be found at
 * ftp://ftp.ncbi.nih.gov/pubchem/Compound/Extras/CID-Synonym-filtered.gz.
 * If a PubCHem CID has multiple valid CAS number, the entry which has a
 * "CAS-" prefix is preferred. For multiple CAS numbers without a "CAS-"
 * prefix, the lowest one is taken.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Checks if a string is a valid CAS number. The general format as well as
 * the check digit are checked.
 * @return 1 If string is a valid CAS number, 0 if not a valid CAS number.
 */
int is_cas(const char *cas);

int main()
{
    char line[4096];
    char *tab_pos; /* pointer to the tab character in the line */
    char *text; /* pointer to the second column in the line */
    const long max_cas_str_length = 13; /* a CAS number is not longer than 12 characters */
    char lowest_cas[max_cas_str_length]; /* temporary storage for lowest CAS number of a CID */
    size_t cas_length = 1000;  /* arbitrary value >= max_cas_str_length */
    int cid; /* current CID */
    int last_cid = 0; /* CID of last loop run */
    int good_cas = 0; /* is set to TRUE if "text" has a "CAS-" prefix */

    while(fgets(line, 4096, stdin) != NULL)
    {
        tab_pos = strchr(line,'\t');
        if(tab_pos == NULL)
            break;
        cid = atoi(line);
        if(last_cid != cid) /* new compound, so print best result from last compound */
        {
            if(!good_cas && cas_length < max_cas_str_length)  /* If cas_length is > 12, it's the first loop run */
                printf("%d\t%s", last_cid, lowest_cas);  /* If no prefixed CAS number was printed, print here */
            cas_length = 1000;
            good_cas = 0;
            last_cid = cid;
        } else if(good_cas) /* fast-forward if already a good CAS number for this compound was found */
            continue;
        text = tab_pos + 1;
        if(strncmp(text, "CAS-", 4) == 0 && is_cas(text + 4))
        {
            printf("%d\t%s", cid, text + 4);
            good_cas = 1;
        }
        else if(is_cas(text))
        {
            if(strlen(text) < cas_length || (strlen(text) == cas_length && strcmp(text, lowest_cas) < 0))
            {
                cas_length = strlen(text);
                strcpy(lowest_cas, text);
            }
        }
    }
    if(last_cid && !good_cas && cas_length < 13) /* print result from last compound if not already done */
        printf("%d\t%s", last_cid, lowest_cas);
}


inline int is_cas(const char *cas)
{
    const char *pch = strchr(cas, '-'); /* pointer to first dash */

    if(pch == NULL)
        return 0;
    long dash_pos = (pch - cas); // position of first dash
    /* First group has 2 to 7 digits, the second 2, the last is a single check digit */
    if(dash_pos < 2 || dash_pos > 7 || *(pch + 3) != '-' || ((*(pch + 5) != '\n') && (*(pch + 5) != '\0')))
        return 0;
    int check_digit = *(pch + 4) - 48;  // - 48 to convert character to number (see ASCII table)
    int calculated = (*(pch + 2) - 48) + (*(pch + 1) - 48) * 2; /* see Wikipedia "CAS number" */
    long i;
    for(i=1;i<=dash_pos;i++)
        calculated += (*(pch - i) - 48) * (i + 2);
    return calculated % 10 == check_digit;
}
