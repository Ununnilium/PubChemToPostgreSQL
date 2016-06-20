# filterCASNumbers
Tool to get the CAS numbers for PubChem compounds.

The CAS numbers for the PubChem compounds are available at
ftp://ftp.ncbi.nlm.nih.gov/pubchem/Compound/Extras/CID-Synonym-filtered.gz,
but are mixed there with normal names. This tool takes a text file like 
```CID-Synonym-filtered``` (two columns, tab-separated, first column CID, 
second column name) as input and prints only the entries to STDOUT where the 
name column contains a valid CAS number.
