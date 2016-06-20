/**
 * @file Database.cc
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
 * Inserts data directly into a PostgreSQL database.
 */

#include "Database.h"


Database::Database(const char conStr[], const char tabName[], const unsigned int numberOfCols,
                   const char colNames[][100], const Oid oidTypes[]) : Output(numberOfCols), name(tabName)
{
    this->conn = PQconnectdb(conStr);
    if (PQstatus(this->conn) == CONNECTION_BAD)
    {
        std::cerr << "Connection to database failed: " << PQerrorMessage(this->conn) << std::endl;
        PQfinish(this->conn);
        throw std::runtime_error("Connection to database failed.");
    }
    PGresult* res = PQprepare(this->conn, "ins_pc", getInsertString(colNames).c_str(), (int)this->numCols, oidTypes);
    this->exitOnError(res);
    PQclear(res);
}

Database::~Database()
{
    PQfinish(this->conn);
}

void Database::addRow(const char * const *row)
{
    PGresult *res = PQexecPrepared(this->conn, "ins_pc", (int)this->numCols, row, NULL, NULL, 0);
    this->exitOnError(res);
    PQclear(res);
}

void Database::exitOnError(PGresult *res, bool withResult)
{
    int status = withResult ? PGRES_TUPLES_OK : PGRES_COMMAND_OK;
    if (PQresultStatus(res) != status)
    {
        std::cerr << PQerrorMessage(this->conn) << std::endl;
        PQclear(res);
        PQfinish(this->conn);
        throw std::runtime_error(PQerrorMessage(this->conn));
    }
}

unsigned long Database::getRowCount()
{
    char query[1000];
    snprintf(query, 1000, "SELECT reltuples AS approximate_row_count FROM pg_class WHERE relname = '%s';",
             this->name);
    PGresult *res = PQexec(this->conn, query);
    this->exitOnError(res, true);
    unsigned long result =  (unsigned long)std::strtof(PQgetvalue(res, 0, 0), NULL);
    PQclear(res);
    return result;
}

std::string Database::getInsertString(const char colNames[][100])
{
    std::stringstream stmt;
    stmt << "INSERT INTO " << this->name << " (" << colNames[0];
    for(size_t i=1;i<this->numCols;i++)
        stmt << ", " << colNames[i];
    stmt << ") VALUES ($1";
    for(size_t i=2;i<=this->numCols;i++)
        stmt << ", $" << i;
    stmt << ")";
    stmt << " ON CONFLICT (cid) DO NOTHING";
    return stmt.str();
}

