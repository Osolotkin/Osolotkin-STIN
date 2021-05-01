FILE FORMATS

[Countries.txt]
  - all individual values are line separated
  - the first line contains number of countries included 
  - next 4 lines specifies indexes of the upcoming countries,
    indexing starting from 1
  - rest of the lines supposed to create list of countries in 
    alphabetical order that is at least as long as big is number 
    on the first line of the file
    
[saves/comparasion_data.save]
  - first 8 bytes has to contain time in seconds from year 1970
    as signed int64, supposed to represent first day
  - afterwards comming comparasion data in folowing format, 
    each one represented as int32: 
      [
        czech increase, 
        czech total, 
        czech time diff, 
        world increase, 
        world total, 
        world time diff
      ]
  - file doesn't have to have all days filled, basically only first 
    day value has to be completed to consider file as valid

[saves/vaccination/]
  - path that supposed to contain vaccination data for each day
  - file names supposed to have following format -> YYYY-MM-DD.csv
    each name represent day it has data of
  - files are downloading strictly from following link:
      https://covid19.who.int/who-data/vaccination-data.csv
    data format has to be corresponding with the files from this link
