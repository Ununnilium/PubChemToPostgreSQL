/**
 * @file main.cc
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
 * Reads data from PubChem SDF files and inserts into a PostgreSQL database.
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "Database.h"
#include "Csv.h"


// Prints a time in seconds in days/hours/minutes/seconds
void printTime(double seconds);

int main(int argc, char *argv[])
{
    const bool toStdout = true;
    const char connStr[] = "user=? password=? host=? dbname=?";
    const unsigned long maxLen = 102399;  // max line length of input file (line buffer -1 because of '\0')
    const unsigned int numberOfCols = 8;  // number of fields to process
    const char dataNames[numberOfCols][50] = {  // keywords in the SDF file (in same order as database columns)
            "> <PUBCHEM_COMPOUND_CID>",
            "> <PUBCHEM_IUPAC_NAME>",
            "> <PUBCHEM_EXACT_MASS>",
            "> <PUBCHEM_MOLECULAR_FORMULA>",
            "> <PUBCHEM_MOLECULAR_WEIGHT>",
            "> <PUBCHEM_OPENEYE_CAN_SMILES>",
            "> <PUBCHEM_IUPAC_INCHI>",
            "> <PUBCHEM_IUPAC_INCHIKEY>"
    };
    const char tabName[] = "pubchem";  // table name in database to insert data
    const char colNames[numberOfCols][100] = { "cid", "name", "exact_mass", "formula", "molecular_weight", "smiles",
                                               "inchi", "inchi_key" };  // column names in database table
    // OIDs for the database table columns. To get oids, use
    // "SELECT typname, oid FROM pg_type" (23: int/int4, 25: text, 701: double/float8)
    const Oid oidTypes[numberOfCols] = { 23/*int cid*/, 25/*text name*/, 701/*double exact_mass*/,
                                         25/*text formula*/, 701/*double molecular_weight*/, 25/*text Smiles*/,
                                         25/*text inchi*/, 25/*text inchi_key*/ };
    const char endBlockStr[] = "$$$$";  // string which marks the end of a compound block in the SDF file
    const unsigned long totalCompounds = 65928741;  // total compounds to insert (to calculate remaining time)
    const unsigned long outputCount = 10000;  // every time outputCount compounds are processed, a message is printed

    // Use file or STDIN as input
    std::ifstream realFile;
    if(argc == 2)  // read from file
    {
        realFile.open(argv[1], std::ios::in);
        if(!realFile)
        {
            std::cerr << "File \"" << argv[1] << "\"couldn't be opened.\n";
            return 2;
        }
    }
    else if(argc > 2)  // To many arguments
    {
        std::cerr << "Usage: " << argv[0] << " input.sdf\nIf the filename is omitted, data are read from STDIN.";
        return 1;
    }
    std::istream &sdf = (argc == 2 ? realFile : std::cin);

    // Allocate memory for array of strings (for compound data)
    char** dataRow = (char**) std::malloc(numberOfCols * sizeof(char*));
    char** dataRowBackup = (char**) std::malloc(numberOfCols * sizeof(char*));
    if(dataRow == NULL || dataRowBackup == NULL)
        throw std::runtime_error("No free memory available.");
    for(size_t i=0;i<numberOfCols;i++)
    {
        dataRow[i] = (char*) std::malloc((maxLen + 1) * sizeof(char));  // + 1 for 0 termination
        dataRowBackup[i] = dataRow[i];
        if(dataRow[i] == NULL)
            throw std::runtime_error("No free memory available.");
    }



    unsigned long counter = 0;  // number of processed compounds
    unsigned long rowCountStart = 0;
    unsigned long rowsRemainingStart = totalCompounds - rowCountStart;  // remaining compounds
    Output *output;
    if(toStdout)  // write CSV to STDOUT
        output = new Csv(numberOfCols);
    else  // write directly to database
        output = new Database(connStr, tabName, numberOfCols, colNames, oidTypes);
    rowCountStart = output->getRowCount();
    std::time_t last = std::time(NULL);  // for measuring speed
    std::time_t start = last;
    double timeSpent;

    char line[maxLen + 1];  // + 1 for '\0' termination
    while(sdf.getline(line, maxLen))
    {
        for(size_t i=0;i<numberOfCols;i++)  // Replace strings with NULL so NULL is inserted in DB for empty strings
            dataRow[i] = NULL;
        while(sdf.getline(line, maxLen) && std::strcmp(line, endBlockStr) != 0)  // over one compound
        {
            for(size_t i=0;i<numberOfCols && std::strlen(line) != 0;i++)  // compare with all possible fields
            {
                if(std::strcmp(line, dataNames[i]) == 0)
                {
                    sdf.getline(dataRowBackup[i], maxLen);
                    dataRow[i] = dataRowBackup[i];
                }
            }
        }
        if(dataRow[0] == NULL)
        {
            std::cerr << "Compound with empty CID skipped\n";
            continue;
        }
        if((dataRow[1] == NULL) && (dataRow[2] == NULL) && (dataRow[3] == NULL) && (dataRow[4] == NULL) &&
                (dataRow[5] == NULL) && (dataRow[6] == NULL) && (dataRow[7] == NULL))
            std::cerr << "All fields are empty for CID " << dataRow[0] << ". Is the line buffer to low?\n";
        output->addRow((const char**)dataRow);
        if(++counter % outputCount == 0)
        {
            timeSpent = std::difftime(std::time(NULL), last);
            last = std::time(NULL);
            std::time_t currentSpeed = (std::time_t)(outputCount / timeSpent);
            std::fprintf(stderr, "Added %uth compound. Current speed: %u compounds/s. Estimated time to finish: ",
                        (unsigned int)(counter + rowCountStart), (unsigned int)currentSpeed);
            printTime(((std::time_t)(rowsRemainingStart - counter) / currentSpeed));
            std::cerr << std::endl;
        }
    }

    delete output;
    for(size_t i=0;i<numberOfCols;i++)
        free(dataRowBackup[i]);
    free(dataRow);
    free(dataRowBackup);
    std::cerr << "\nInserted " << counter << " compounds in ";
    timeSpent = std::difftime(std::time(NULL), start);
    printTime(timeSpent);
    std::cerr << " (Average: " << (std::time_t)counter / timeSpent << " compounds/s)\n";
    return 0;
}

// Prints a time in seconds in days/hours/minutes/seconds
void printTime(double seconds)
{
    std::time_t days = (std::time_t)(seconds / (60. * 60. * 24.));
    seconds -= days * 60 * 60 * 24;
    std::time_t hours = (std::time_t)(seconds / (60. * 60.));
    seconds -= hours * 60 * 60;
    std::time_t minutes = (std::time_t)(seconds / 60.);
    seconds -= minutes * 60;
    std::fprintf(stderr, "%u d %u h %u min %u s", (unsigned int)days, (unsigned int)hours,
                 (unsigned int)minutes, (unsigned int)seconds);
}
