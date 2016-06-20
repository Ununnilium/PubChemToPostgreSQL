# PubChemToPostgreSQL
Tool to read PubChem SDF files and store the data in a PostgreSQL database.
The SDF files are available at
ftp://ftp.ncbi.nlm.nih.gov/pubchem/Compound/CURRENT-Full/SDF/.

This tool can insert the data directly into the database. Because this is
slower than importing via ```psql```, below the usage together with ```psql```
is described. If you want to insert the data directly anyway, you have to adapt
the database settings in main.cc before compiling.

The code is partially untested, so bugs can occur.

## Recommended Usage
1. Create a PostgreSQL table like the following:

    CREATE TABLE pubchem
    (
    cid integer NOT NULL,
    name text,
    exact_mass double precision,
    formula text,
    molecular_weight double precision,
    smiles text,
    inchi text,
    inchi_key text,
    CONSTRAINT pk_cid PRIMARY KEY (cid)
    )

2. Configure PostgreSQL for passwordless login with ```.pgpass```
3. Compile this program with cmake or manually with

    g++ main.cc Output.cc Database.cc Csv.cc -lpq -I/usr/pgsql-9.5/include \
        -L/usr/pgsql-9.5/lib -o sdf2postgres

4. Start the importing process with 
    zcat *.sdf.gz | ./sdf2postgres | psql -c "COPY pubchem (cid, name, \
        exact_mass, formula, molecular_weight, smiles, inchi, inchi_key) FROM \
        stdin" DB_NAME USER_NAME
