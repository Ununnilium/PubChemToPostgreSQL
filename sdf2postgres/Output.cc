/**
 * @file Output.cc
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
 * Parent class to allow different ways for writing the data into the 
 * PostgreSQL database.
 */

#include "Output.h"

Output::Output(const unsigned int numberOfCols) : numCols(numberOfCols)
{}

unsigned long Output::getRowCount()
{
    return 0;
}

Output::~Output()
{}