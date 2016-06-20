/**
 * @file Csv.cc
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
 * Class to output data as tab-separated text, which can be used with 
 * PostgreSQL's COPY function, e.g.:
 * ./sdf2postgres | psql -c "COPY table (columns) FROM stdin" db user
 * 
 * Use the .pgpass file for passwordless login to DB.
 */

#include "Csv.h"

Csv::Csv(const unsigned int numberOfCols) : Output(numberOfCols)
{}

void Csv::addRow(const char *const *row)
{
    std::cout << row[0];
    for(size_t i=1;i<this->numCols;i++)
        std::cout << '\t' << (row[i] == NULL ? "" : row[i]);
    std::cout << std::endl;
}
